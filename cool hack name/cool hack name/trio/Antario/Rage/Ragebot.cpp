#pragma once
#include "Ragebot.h"


Ragebot g_ragebot;

float DotProduct(const float* a, const float* b)
{
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}


// lots of math stuff, i need to fix the math.cpp file because you cant access like 50% of stuff from it. sorry for ugly code. -drifttwo/voozez
// fixed this

// this is so fucking ugly, but ya know what, who cares.

float Ragebot::GetInterp()
{
	int updaterate = g_pCvar->FindVar("cl_updaterate")->GetInt();
	ConVar* minupdate = g_pCvar->FindVar("sv_minupdaterate");
	ConVar* maxupdate = g_pCvar->FindVar("sv_maxupdaterate");

	if (minupdate && maxupdate)
		updaterate = maxupdate->GetInt();

	float ratio = g_pCvar->FindVar("cl_interp_ratio")->GetFloat();

	if (ratio == 0)
		ratio = 1.0f;

	float lerp = g_pCvar->FindVar("cl_interp")->GetFloat();
	ConVar * cmin = g_pCvar->FindVar("sv_client_min_interp_ratio");
	ConVar * cmax = g_pCvar->FindVar("sv_client_max_interp_ratio");

	if (cmin && cmax && cmin->GetFloat() != 1)
		ratio = std::clamp(ratio, cmin->GetFloat(), cmax->GetFloat());

	return max(lerp, (ratio / updaterate));
}

std::vector<int> Ragebot::GetHitboxes()
{
	std::vector<int> hitboxes;
	bool baim;

	if (g_Settings.ragetab.RageHitscan[0])
		hitboxes.push_back((int)Hitboxes::HITBOX_HEAD);
	if (g_Settings.ragetab.RageHitscan[1])
		hitboxes.push_back((int)Hitboxes::HITBOX_NECK);
	if (g_Settings.ragetab.RageHitscan[2])
	{
		hitboxes.push_back((int)Hitboxes::HITBOX_UPPER_CHEST);
		hitboxes.push_back((int)Hitboxes::HITBOX_CHEST);
		hitboxes.push_back((int)Hitboxes::HITBOX_LOWER_CHEST);
	}
	if (g_Settings.ragetab.RageHitscan[3])
		hitboxes.push_back((int)Hitboxes::HITBOX_STOMACH);
	if (g_Settings.ragetab.RageHitscan[4])
		hitboxes.push_back((int)Hitboxes::HITBOX_PELVIS);
	if (g_Settings.ragetab.RageHitscan[5])
	{ 
		hitboxes.push_back((int)Hitboxes::HITBOX_LEFT_FOREARM);
		hitboxes.push_back((int)Hitboxes::HITBOX_RIGHT_FOREARM);
		hitboxes.push_back((int)Hitboxes::HITBOX_LEFT_UPPER_ARM);
		hitboxes.push_back((int)Hitboxes::HITBOX_RIGHT_UPPER_ARM);
		hitboxes.push_back((int)Hitboxes::HITBOX_LEFT_HAND);
		hitboxes.push_back((int)Hitboxes::HITBOX_RIGHT_HAND);
	}
	if (g_Settings.ragetab.RageHitscan[6])
	{
		hitboxes.push_back((int)Hitboxes::HITBOX_RIGHT_CALF);
		hitboxes.push_back((int)Hitboxes::HITBOX_LEFT_CALF);
		hitboxes.push_back((int)Hitboxes::HITBOX_RIGHT_THIGH);
		hitboxes.push_back((int)Hitboxes::HITBOX_LEFT_THIGH);
		hitboxes.push_back((int)Hitboxes::HITBOX_LEFT_FOOT);
		hitboxes.push_back((int)Hitboxes::HITBOX_RIGHT_FOOT);
	}

	return hitboxes;
}

PointInfo Ragebot::GetBestPointByHitbox(C_BaseEntity* pBaseEntity, int iHitbox, matrix3x4_t boneMatrix[128])
{
	studiohdr_t* pStudioModel = g_MdlInfo->GetStudiomodel(pBaseEntity->GetModel());
	mstudiohitboxset_t* set = pStudioModel->pHitboxSet(0);

	mstudiobbox_t* hitbox = set->pHitbox(iHitbox);

	std::vector<Vector> points;

	points.push_back((hitbox->bbmax + hitbox->bbmin) * 0.5f); //center

	float radius = hitbox->m_flRadius;

	Vector bbmin = hitbox->bbmin, bbmax = hitbox->bbmax;
	Vector bbminhead = hitbox->bbmin, bbmaxhead = hitbox->bbmax;

	if (hitbox->m_flRadius != -1.f)
	{
		bbmin -= radius;
		bbmax += radius;
		bbminhead -= radius;
		bbmaxhead += radius;
	}

	int adjusted_pointscale = g_Settings.ragetab.RagePointScale / 10;

	bbmin *= adjusted_pointscale;
	bbmax *= adjusted_pointscale;

	if (iHitbox == (int)Hitboxes::HITBOX_HEAD)
		points.push_back((Vector(bbmax.x, bbmin.y, bbmin.z) + Vector(bbmax.x, bbmin.y, bbmax.z)) * 0.5f); //top/back (for down/backwards AA)

	points.push_back((Vector(bbmax.x, bbmin.y, bbmin.z) + Vector(bbmax.x, bbmax.y, bbmax.z)) * 0.5f); //top		
	points.push_back((Vector(bbmin.x, bbmin.y, bbmax.z) + Vector(bbmax.x, bbmax.y, bbmax.z)) * 0.5f); //right		
	points.push_back((Vector(bbmax.x, bbmin.y, bbmin.z) + Vector(bbmin.x, bbmax.y, bbmin.z)) * 0.5f); //left

	PointInfo bestpoint;

	for (auto& point : points)
	{
		Vector hitbox_point;
		g_Math.VectorTransform_2(point, boneMatrix[hitbox->bone], hitbox_point);

		float tempdamage = autowall->CanHit(hitbox_point);

		if (tempdamage < g_Settings.ragetab.RageMinimumDamage) continue;

		//if (tempdamage > bestpoint.damage)
		if (bestpoint.damage < tempdamage)
		{
			bestpoint.damage = tempdamage;
			bestpoint.point = hitbox_point;
		}

		if (bestpoint.damage > pBaseEntity->GetHealth())
			return bestpoint;
	}

	if (bestpoint.damage < 0.01f) return PointInfo();

	return bestpoint;
}


PointInfo Ragebot::HitScan(C_BaseEntity * pBaseEntity)
{
	auto hitboxes = GetHitboxes();

	matrix3x4_t matrix[128];

	pBaseEntity->SetupBones(matrix, 128, BONE_USED_BY_HITBOX, g_pGlobalVars->curtime);

	PointInfo bestPoint;

	for (auto hitbox : hitboxes)
	{
		auto point = GetBestPointByHitbox(pBaseEntity, hitbox, matrix);

		if (point.damage > bestPoint.damage)
		{
			bestPoint = point;
		}
	}

	return bestPoint;
}

void Ragebot::FindTarget()
{
	float bestDistance = 8192.f;
	Target_t bestTarget;
	PointInfo bestInfo;

	for (int it = 0; it < g_pEngine->GetMaxClients(); ++it)
	{
		C_BaseEntity* pEntity = g_pEntityList->GetClientEntity(it);

		if
			(
				!pEntity
				|| pEntity == g::pLocalEntity
				|| pEntity->IsDormant()
				|| !pEntity->IsAlive()
				|| pEntity->GetTeam() == g::pLocalEntity->GetTeam()
				)
		{
			continue;
		}

		float distance = pEntity->GetEyePosition().DistTo(g::pLocalEntity->GetEyePosition());

		if (distance < bestDistance)
		{
			auto pointinfo = HitScan(pEntity);

			if (pointinfo.damage > bestInfo.damage)
			{
				bestDistance = distance;
				bestInfo = pointinfo;
				bestTarget = Target_t(pEntity, pointinfo.point);
			}
		}
	}

	if (bestTarget.pEnt != nullptr && bestTarget.vPos != Vector(0.f, 0.f, 0.f))
	{
		this->tTarget = bestTarget;
	}
}

void Autostop()
{
	if (!g_Settings.ragetab.RageAutostop)
	return;

	g::autostoping = true;
	// P1000000 SELF CODE DUDE!!!!!!!11
}


bool Ragebot::HitChance(C_BaseEntity* pEnt, C_BaseCombatWeapon* pWeapon, Vector Angle, float chance)
{
	float Seeds = (chance <= 1.f) ? 356.f : 256.f;

	Vector forward, right, up;

	g_Math.AngleVectors(Angle, &forward, &right, &up);

	int Hits = 0, neededHits = (Seeds * (chance / 100.f));

	float weapSpread = pWeapon->GetSpread(), weapInaccuracy = pWeapon->GetInaccuracy();

	for (int i = 0; i < Seeds; i++)
	{
		float Inaccuracy = g_Math.RandomFloat(0.f, 1.f) * weapInaccuracy;
		float Spread = g_Math.RandomFloat(0.f, 1.f) * weapSpread;

		Vector spreadView((cos(g_Math.RandomFloat(0.f, 2.f * M_PI)) * Inaccuracy) + (cos(g_Math.RandomFloat(0.f, 2.f * M_PI)) * Spread), (sin(g_Math.RandomFloat(0.f, 2.f * M_PI)) * Inaccuracy) + (sin(g_Math.RandomFloat(0.f, 2.f * M_PI)) * Spread), 0), direction;
		direction = Vector(forward.x + (spreadView.x * right.x) + (spreadView.y * up.x), forward.y + (spreadView.x * right.y) + (spreadView.y * up.y), forward.z + (spreadView.x * right.z) + (spreadView.y * up.z)).Normalize();

		Vector viewanglesSpread, viewForward;

		g_Math.VectorAngles(direction, up, viewanglesSpread);
		g_Math.NormalizeAngle(viewanglesSpread);

		g_Math.AngleVectors(viewanglesSpread, &viewForward);
		viewForward.NormalizeInPlace();

		viewForward = g::pLocalEntity->GetEyePosition() + (viewForward * pWeapon->GetCSWpnData()->flRange);

		trace_t tr;
		Ray_t ray;

		ray.Init(g::pLocalEntity->GetEyePosition(), viewForward);
		g_pEngineTrace->ClipRayToEntity(ray, MASK_SHOT | CONTENTS_GRATE, pEnt, &tr);

		if (tr.hit_entity == pEnt)
			Hits++;

		if (((Hits / Seeds) * 100.f) >= chance)
			return true;

		if ((Seeds - i + Hits) < neededHits)
			return false;
	}

	return false;
}


bool CanFire()
{

	if (g::pLocalEntity->GetActiveWeapon()->IsReloading() || g::pLocalEntity->GetActiveWeapon()->GetAmmo() <= 0)
		return false;

	auto flServerTime = g::pLocalEntity->GetTickBase() * g_pGlobalVars->intervalPerTick;

	return g::pLocalEntity->GetActiveWeapon()->GetNextPrimaryAttack() <= flServerTime;
} 

void Autoscope() {
	if (g_Settings.ragetab.RageAutoscope) {
		if (g::pLocalEntity->GetActiveWeapon()->isSniper()) {
			if (!g::pLocalEntity->GetScoped()) {
				g::pCmd->buttons |= IN_ATTACK2;
			}
		}
	} //autoscope

}

Vector VelocityExtrapolate(C_BaseEntity* player, Vector aimPos) {
	return aimPos + (player->GetVelocity() * g_pGlobalVars->intervalPerTick);
} // lol pasted !

void Ragebot::Fire()
{
	auto weapon = g::pLocalEntity->GetActiveWeapon();

	if (!(g::pCmd->buttons & IN_ATTACK2) && CanFire() && this->tTarget.pEnt != nullptr && this->tTarget.vPos != Vector(0.f, 0.f, 0.f))
	{
		Vector result; // shot angle

		g_Math.CalcAngle(g::pLocalEntity->GetEyePosition(), this->tTarget.vPos, result); //calculates shot angle

		//result = VelocityExtrapolate(tTarget.pEnt, result); //velocity extrapolate thing, unsure on how it works/how it helps(may just make it worse) but whatever ill keep it L m a o

			auto qang = g::pLocalEntity->GetAimPunch(); //gets aim punch for no recoil

			auto aimpunchangle = Vector(qang.x, qang.y, qang.z); // gets it again but in vector

			result -= (aimpunchangle * g_pCvar->FindVar("weapon_recoil_scale")->GetFloat()); // more recoilkstuff

		bool hitchance = HitChance(this->tTarget.pEnt, weapon, result, g_Settings.ragetab.RageHitchance); // hitchance calculation

		Autostop();
		Autoscope();

		float damage = autowall->CanHit(this->tTarget.vPos);

			if (hitchance) // if hitchance is true(aka can hit)
			{
				if (damage > g_Settings.ragetab.RageMinimumDamage) { // second min damage calculation, for accuracy.
					Autostop(); // lol run it twice, twice the headshots!
					g::pCmd->viewangles = result; // sets to result aka where it aims
					g::pCmd->buttons |= IN_ATTACK; //shoots
					g::pCmd->tick_count = TIME_TO_TICKS(this->tTarget.pEnt->GetSimTime() + GetInterp()); // lag comp stuff(or helps to hit moving enemies maybe? dont know tbh.
				}

				//G::RealAngles = G::pCmd->viewangles;
			}
		}
	else {
		g::autostoping = false;
	}

	//drop target
	this->tTarget = Target_t(nullptr, Vector(0.f, 0.f, 0.f));
}

// i really hate this as a ragebot, it confuses me but whatever what can i do



void Ragebot::extrapolation() {
	for (int i = 1; i < g_pGlobalVars->maxClients; i++) {
		C_BaseEntity* e = g_pEntityList->GetClientEntity(i);
		if (!e
			|| !e->IsAlive()
			|| e->IsDormant()) {
			continue;
		}

		float simtime_delta = e->GetSimulationTime() - e->GetOldSimTime();
		int choked_ticks = simtime_delta;
		Vector lastOrig;

		if (lastOrig.Length() != e->GetVecOrigin().Length())
			lastOrig = e->GetVecOrigin();

		float delta_distance = (e->GetVecOrigin() - lastOrig).LengthSqr();
		if (delta_distance > 4096.f) {
			Vector velocity_per_tick = e->GetVelocity() * g_pGlobalVars->intervalPerTick;
			auto new_origin = e->GetVecOrigin() + (velocity_per_tick * choked_ticks);
			e->SetOrigin(new_origin);
		}
	}
} //vro extrapolation

void Ragebot::DoRagebot() { // runs ragebot stuff
	if (!g_Settings.ragetab.RageEnabled)
		return;
	if (!g::pLocalEntity)
		return;
	if (!g_pEngine->IsInGame())
		return;
	if (!g::pLocalEntity->IsAlive())
		return;
	// checks if localentity exists, if is ingame, and is alive
	if (g_Settings.ragetab.RageResolver) {
		for (int i = 1; i < g_pGlobalVars->maxClients; i++)
		{
			auto p_entity = g_pEntityList->GetClientEntity(i);
			if (p_entity && !p_entity->IsDormant())
			{
				auto feet_yaw = p_entity->AnimState()->m_flCurrentFeetYaw;
				auto body_max_rotation = p_entity->AnimState()->pad10[516];
				if (feet_yaw <= 58)
				{
					if (-58 > feet_yaw)
						p_entity->GetLowerBodyYaw() = body_max_rotation + p_entity->GetLowerBodyYaw();
				}
				else
				{
					p_entity->GetLowerBodyYaw() = body_max_rotation - p_entity->GetLowerBodyYaw();
				}
				if (p_entity->AnimOverlays()->m_flPlaybackRate > 0.1)
				{
					for (int resolve_delta = 58.f; resolve_delta < -58.f; resolve_delta = resolve_delta - 20.f)
					{
						p_entity->GetLowerBodyYaw() = resolve_delta;
					}
				}
			}
		}
	} //pasted from uc, https://www.unknowncheats.me/forum/counterstrike-global-offensive/317797-desync-resolver.html
	// not great but its better than nothing

	this->FindTarget(); // gets target for aimbot
	this->Fire(); // shoots at it
}