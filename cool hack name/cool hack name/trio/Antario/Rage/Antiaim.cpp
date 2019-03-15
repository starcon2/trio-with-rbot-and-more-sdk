#pragma once
#include "Antiaim.h"

Antiaim g_antiaim;


//this is not pretty


float random_bet(float a, float b)
{
	srand(time(NULL));

	int r = rand() % 2;
	if (r == 0)
		return a;
	else
		return b;
}



bool target_meets_requirements(C_BaseEntity * e) {
	if (e && !e->IsDormant() && e->IsAlive() && e->GetTeam() != g::pLocalEntity->GetTeam())
		return true;
	else
		return false;
}

int closest() {
	int index = -1;
	float lowest_fov = 180.f, fov;
	Vector aimAngle, angles, local_position;
	C_BaseEntity* local_player = g::pLocalEntity;

	if (!local_player)
		return -1;

	if (!local_player->IsAlive())
		return -1;

	local_position = g::pLocalEntity->GetEyePosition();
	g_pEngine->GetViewAngles(angles);
	for (auto i = 1; i < g_pEntityList->GetHighestEntityIndex(); i++)
	{
		C_BaseEntity* e = g_pEntityList->GetClientEntity(i);

		if (!target_meets_requirements(e))
			continue;

		aimAngle = g_Math.CalcAngle(local_position, e->GetEyePosition());
		fov = 90;
		if (fov < lowest_fov)
		{
			lowest_fov = fov;
			index = i;
		}
	}
	return index;
}



float setup_desync(int mode) {
	float max_delta = 0;

	if (!g::bSendPacket)
		return 0;

	auto get_max_desync_delta_reversed = [&]() -> float {
		auto animstate = g::pLocalEntity->AnimState();
		float v49;
		float v46;
		float v51;

		if (animstate->m_flFeetSpeedForwardsOrSideWays >= 0.0)
			v46 = fminf(animstate->m_flFeetSpeedForwardsOrSideWays, 1.0);
		else
			v46 = 0.0;

		float v47 = (float)((animstate->m_flStopToFullRunningFraction * -0.30000001f) - 0.19999999f) * v46;
		v49 = v47 + 1.0;
		if (animstate->m_fDuckAmount > 0.0)
		{
			if (animstate->m_flFeetSpeedForwardsOrSideWays >= 0.0)
				v51 = fminf(animstate->m_flFeetSpeedForwardsOrSideWays, 1.0);
			else
				v51 = 0.0;
			float v52 = animstate->m_fDuckAmount * v51;
			v49 = v49 + (float)(v52 * (float)(0.5 - v49));
		}

		float v53 = *(float*)(animstate + 0x334) * v49;
		return v53;
	};

	switch (mode)
	{
	case 1:  // balance
		if (get_max_desync_delta_reversed() >= -58.f || get_max_desync_delta_reversed() <= 58.f)
			max_delta = get_max_desync_delta_reversed();
		else
			max_delta = 58.f;
		break;
	case 2:  // stretch
		max_delta = 58.f;
		break;
	case 3:  // fake jitter
		max_delta = rand() % -58 + 58;
		break;
	case 4:  // switch
		max_delta = (-58.0f / 2.f + std::fmodf(g_pGlobalVars->curtime * 150, 58.0f));
		break;
	}

	*g::pLocalEntity->AnimState()->feetyaw() = -118; //stupid meme but it works bitch 
	return max_delta;
}




// pasted from inure


float quick_normalize(float degree, const float min, const float max) {
	while (degree < min)
		degree += max - min;
	while (degree > max)
		degree -= max - min;

	return degree;
} // ooo baby inure :smirk: love u bb no homo ;) ;)

struct angle_data {
	float angle;
	float thickness;
	angle_data(const float angle, const float thickness) : angle(angle), thickness(thickness) {}
};

void AngleVectors_imnotgay(const QAngle& angles, Vector* forward)
{
	float sp, sy, cp, cy;

	g_Math.SinCos(DEG2RAD(angles.y), &sy, &cy);
	g_Math.SinCos(DEG2RAD(angles.x), &sp, &cp);

	forward->x = cp * cy;
	forward->y = cp * sy;
	forward->z = -sp;
}

void freestanding_v2() {

	std::vector< angle_data > points;

	auto local_position = g::pLocalEntity->GetEyePos();
	std::vector< float > scanned = {};

	for (auto i = 0; i <= 64; i++) {
		auto enemy = g_pEntityList->GetClientEntity(i);
		if (enemy == nullptr) continue;
		if (enemy == g::pLocalEntity) continue;
		if (!enemy->IsAlive()) continue;
		if (enemy->GetTeam() == g::pLocalEntity->GetTeam()) continue;
		if (enemy->IsDormant()) continue;

		QAngle view;
		local_position.AngleTo(enemy->GetEyePos());

		std::vector< angle_data > angs;

		for (auto y = 0; y < 8; y++) {
			auto ang = quick_normalize((y * 45) + view.y, -180.f, 180.f);
			auto found = false; // check if we already have a similar angle

			for (auto i2 : scanned)
				if (abs(quick_normalize(i2 - ang, -180.f, 180.f)) < 20.f)
					found = true;

			if (found)
				continue;

			points.emplace_back(ang, -1.f);
			scanned.push_back(ang);
		}
		//points.push_back(base_angle_data(view.y, angs)); // base yaws and angle data (base yaw needed for lby breaking etc)

		auto found = false;
		auto points_copy = points; // copy data so that we compare it to the original later to find the lowest thickness
		auto enemy_eyes = enemy->GetEyePos();

		for (auto& z : points_copy) // now we get the thickness for all of the data
		{
			const QAngle tmp(10, z.angle, 0.0f);
			Vector head;
			AngleVectors_imnotgay(tmp, &head);
			head *= ((16.0f + 3.0f) + ((16.0f + 3.0f) * sin(DEG2RAD(10.0f)))) + 7.0f;
			head += local_position;
			float distance = -1;
			auto enemy_weapon = enemy->GetActiveWeapon();
			if (enemy_weapon) {
				auto weapon_data = enemy_weapon->GetCSWeaponData();
				if (weapon_data)
					distance = weapon_data->flRange;
			}
			float local_thickness = autowall->get_thickness(head, enemy_eyes, distance);
			z.thickness = local_thickness;

			if (local_thickness > 0) // if theres a thickness of 0 dont use this data
			{
				found = true;
			}
		}

		if (!found) // dont use, completely visible to this player or data is invalid
			continue;

		for (unsigned int z = 0; points_copy.size() > z; z++)
			if (points_copy[z].thickness < points[z].thickness || points[z].thickness == -1)
				// find the lowest thickness so that we can hide our head best for all entities
				points[z].thickness = points_copy[z].thickness;
	}
	float best = 0;
	for (auto& i : points)
		if ((i.thickness > best || i.thickness == -1) && i.thickness != 0)
			// find the best hiding spot (highest thickness)
		{
			best = i.thickness;
			if (g::bSendPacket) {
				g::pCmd->viewangles.y = i.angle + setup_desync(g_Settings.ragetab.AntiAimDesync);
			}
			else {
				g::pCmd->viewangles.y = i.angle;
			}
		}
}

//this is the freestanding inure posted on uc, i optimized(so you dont get 0fps).


Vector m_oldangle;
float m_oldforward;
float m_oldsidemove;
void Antiaim::StartMoveFix()
{
		m_oldangle = g::pCmd->viewangles;
		m_oldforward = g::pCmd->forwardmove;
		m_oldsidemove = g::pCmd->sidemove;
}

bool isSlowWalking;

float slowwalkfmove;
float slowwalksmove;

void DoSlowWalk() {
	bool SlowWalkBinder;
	Keybind(g_Settings.misc.bSlowWalkKeybind, g_Settings.misc.bSlowWalkBindType, SlowWalkBinder);
	if (SlowWalkBinder) {
		isSlowWalking = true;
	} else {
		isSlowWalking = false;
	}
}

float autostopfmove;
float autostopsmove;

void Antiaim::EndMoveFix()
{
		float f1, f2, yaw_delta = g::pCmd->viewangles.y - m_oldangle.y;
		if (m_oldangle.y < 0.f)
			f1 = 360.0f + m_oldangle.y;
		else
			f1 = m_oldangle.y;

		if (g::pCmd->viewangles.y < 0.0f)
			f2 = 360.0f + g::pCmd->viewangles.y;
		else
			f2 = g::pCmd->viewangles.y;

		if (f2 < f1)
			yaw_delta = abs(f2 - f1);
		else
			yaw_delta = 360.0f - abs(f1 - f2);

		yaw_delta = 360.0f - yaw_delta;

		if (g::autostoping) {
			if (!isSlowWalking) {
				autostopfmove = g::fixedforwardmove * 0.0034;
				autostopsmove = g::fixedsidemove * 0.0034;
				g::fixedforwardmove = autostopfmove;
				g::fixedsidemove = autostopsmove;
				return;
				// omg bro p1000000000 autostop selfcoded
			}
		}

		if (!isSlowWalking) {
			g::fixedforwardmove = cos(DEG2RAD(yaw_delta)) * m_oldforward + cos(DEG2RAD(yaw_delta + 90.f)) * m_oldsidemove;
			g::fixedsidemove = sin(DEG2RAD(yaw_delta)) * m_oldforward + sin(DEG2RAD(yaw_delta + 90.f)) * m_oldsidemove;
			return;
		} else {
			slowwalkfmove = cos(DEG2RAD(yaw_delta)) * m_oldforward + cos(DEG2RAD(yaw_delta + 90.f)) * m_oldsidemove;
			slowwalksmove = sin(DEG2RAD(yaw_delta)) * m_oldforward + sin(DEG2RAD(yaw_delta + 90.f)) * m_oldsidemove * 0.0034 * g_Settings.misc.bSlowWalkSpeed;
			
			slowwalkfmove = slowwalkfmove * 0.0034 * g_Settings.misc.bSlowWalkSpeed;

			g::fixedforwardmove = slowwalkfmove;
			g::fixedsidemove = slowwalksmove;
			return;
		}

}

void FakeDuck() // bad fakeduck
{
	CUserCmd* cmd = g::pCmd;
	bool& bSendPackets = g::bSendPacket;
	bool FakeDuckBinder;
	Keybind(g_Settings.misc.bFakeDuckKeybind, g_Settings.misc.bFakeDuckBindType, FakeDuckBinder);

	if (FakeDuckBinder) {
		g::pCmd->buttons |= IN_DUCK;
		if (cmd->buttons & IN_DUCK)
		{
			g::pCmd->buttons |= IN_BULLRUSH;
			static bool counter = false;
			static int counters = 0;
			if (counters == 9)
			{
				counters = 0;
				counter = !counter;
			}
			counters++;
			if (counter)
			{
				cmd->buttons |= IN_DUCK;
				bSendPackets = true;
			}
			else
				cmd->buttons &= ~IN_DUCK;
		}
	}
}

int lag_comp_break() { //brakes lagcomp i think?
	Vector velocity = g::pLocalEntity->GetVelocity(); velocity.z = 0;
	float speed = velocity.Length();

	if (speed > 0.f) {
		auto distance_per_tick = speed * g_pGlobalVars->intervalPerTick;
		int choked_ticks = std::ceilf(65.f / distance_per_tick);
		return std::min<int>(choked_ticks, g_Settings.ragetab.AntiAimFakelagAmt);
	}
	return 1;
}

void fakelag() {
	static int ticks = 0; //fakelag ticks???????
	int choke; //fakelag amount
	bool should_fakelag = false; //should fakelag, self explanatory

	if (!g_pEngine->IsInGame())
		return;

	if (!g::pLocalEntity->IsAlive())
		return;

	if (!g_Settings.ragetab.AntiAimFakelagWhile[3]) {
		if (g::pCmd->buttons & IN_ATTACK)
			return;
	}

	bool fakelagtype = g_Settings.ragetab.AntiAimFakelagType;
	switch (fakelagtype) {
	case 0:
		choke = g_Settings.ragetab.AntiAimFakelagAmt; //factor
		break;
	case 1:
		choke = std::min<int>(static_cast<int>(std::ceilf(64 / (g::pLocalEntity->GetVelocity().Length() * g_pGlobalVars->intervalPerTick))), g_Settings.ragetab.AntiAimFakelagAmt); //dynamic
		break;
	case 2:
		choke = lag_comp_break(); //break
		break;
	}


	 if (g_Settings.ragetab.AntiAimFakelagWhile[0]) { //fakelag while standing
		if (g::pLocalEntity->GetFlags() & FL_ONGROUND)
			should_fakelag = true;
	}

	if (g_Settings.ragetab.AntiAimFakelagWhile[1]) { //fakelag while moving
		if (g::pLocalEntity->GetVelocity().Length() > 0.1f && g::pLocalEntity->GetFlags() & FL_ONGROUND)
			should_fakelag = true;
	}

	if (g_Settings.ragetab.AntiAimFakelagWhile[2]) { // fakelag while in air
		if (!(g::pLocalEntity->GetFlags() & FL_ONGROUND))
			should_fakelag = true;
	}

	if (should_fakelag) {
		if (ticks > choke) {
			ticks = 0;
			g::bSendPacket = true;
		}
		else {
			g::bSendPacket = false;
			ticks++;
		}
		g::fakelag = true;
	}
	else
		g::fakelag = false;
}

void Antiaim::DoAntiAim()
{
	if (!g_pEngine->IsInGame())
		return;

	if (!g::pLocalEntity->IsAlive())
		return;

	if (g::pCmd->buttons & IN_ATTACK) //so that you actually aim at who you shoot(kinda)
		return;

	if (g::pCmd->buttons & IN_USE) //so you can open doors and fuck bitches
		return;

	switch (g_Settings.ragetab.AntiAimPitch) // pitches
	{
	case 1:
		g::pCmd->viewangles.x = -89.9f; //up pitch
		break;
	case 2:
		g::pCmd->viewangles.x = 89.9f; //down pitch
		break;
	case 3:
		g::pCmd->viewangles.x = random_bet(-89.0f, 89.0f); //mixed pitch
		break;
	case 4:
		g::pCmd->viewangles.x = 0; //zero pitch
		break;
	case 5:
		g::pCmd->viewangles.x = 1080; //fake zero down pitch
		break;
	case 6:
		g::pCmd->viewangles.x = -540; //fake down pitch
		break;
	case 7:
		g::pCmd->viewangles.x = 540; //fake up pitch
		break;
	case 8:
		g::pCmd->viewangles.x = random_bet(-540.0f, 540.0f); //fake mixed pitch
		break;
	}
	// half of these make the movefix go crazy
	if (g_Settings.ragetab.AntiAimFreestanding) {
		freestanding_v2();
	}
	else
	{
		// manual aa
		static bool down, left, right;

		bool backon;
		bool lefton;
		bool righton;

		Keybind(g_Settings.ragetab.AntiAimBackward, g_Settings.misc.bSlowWalkBindType, backon);

		Keybind(g_Settings.ragetab.AntiAimLeft, g_Settings.misc.bSlowWalkBindType, lefton);

		Keybind(g_Settings.ragetab.AntiAimRight, g_Settings.misc.bSlowWalkBindType, righton);

		if (backon) { down = true; left = false; right = false; }
		if (lefton) { down = false; left = true; right = false; }
		if (righton) { down = false; left = false; right = true; }
		if (down) {
			if (g::bSendPacket) { // when to do desync
				g::pCmd->viewangles.y -= 180.f + setup_desync(g_Settings.ragetab.AntiAimDesync);
			}
			else {
				g::pCmd->viewangles.y -= 180.f;
			}
		}
		if (left) {
			if (g::bSendPacket) { // when to do desync
				g::pCmd->viewangles.y += 90.f + setup_desync(g_Settings.ragetab.AntiAimDesync);
			}
			else {
				g::pCmd->viewangles.y += 90.f;
			}
		}
		if (right) {
			if (g::bSendPacket) { // when to do desync
				g::pCmd->viewangles.y -= 90.f + setup_desync(g_Settings.ragetab.AntiAimDesync);
			}
			else {
				g::pCmd->viewangles.y -= 90.f;
			}
		}

		}

		if (g_Settings.misc.bInfDuck)
		{
			g::pCmd->buttons |= IN_BULLRUSH;
		}

		DoSlowWalk();
		FakeDuck();
		fakelag();
	}