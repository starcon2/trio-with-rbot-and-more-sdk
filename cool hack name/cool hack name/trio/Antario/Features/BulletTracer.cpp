#include "BulletTracer.h"

/* declarations */
bullettracer::player_hurt_event player_hurt_listener;
bullettracer::bullet_impact_event bullet_impact_listener;

/* bulletracer */
bullettracer g_bullettracer;

/* trace logs */
std::vector<trace_info> trace_logs;

/* color used for the tracers */
Color color;

/* bullet tracer draw */
void bullettracer::draw_beam(Vector src, Vector end, Color color)
{
	BeamInfo_t info;
	info.m_nType = TE_BEAMPOINTS;
	info.m_pszModelName = "sprites/purplelaser1.vmt";
	//info.m_pszHaloName = -1;
	info.m_nHaloIndex = -1;
	info.m_flHaloScale = 0.f;
	info.m_flLife = g_Settings.visuals.iTracerTime;
	info.m_flWidth = 4;
	info.m_flEndWidth = 6;
	info.m_flFadeLength = 0;
	info.m_flAmplitude = 0.f;
	info.m_flBrightness = color.a();
	info.m_flSpeed = 1.f;
	info.m_nStartFrame = 0;
	info.m_flFrameRate = 0;
	info.m_flRed = color.r();
	info.m_flGreen = color.g();
	info.m_flBlue = color.b();
	info.m_nSegments = -1;
	info.m_bRenderable = true;
	info.m_nFlags = FBEAM_ONLYNOISEONCE | FBEAM_NOTILE;
	info.m_vecStart = src - Vector(0, 0, 5); // Vector(0, 0, 5);
	info.m_vecEnd = end - Vector(0, 0, 5);

	Beam_t* beam = g_pViewRenderBeams->CreateBeamPoints(info);

	if (beam)
		g_pViewRenderBeams->DrawBeam(beam);
}

/* listeners */
void bullettracer::listener()
{
	g_pEventManager->AddListener(&bullet_impact_listener, "bullet_impact", false);

	g_pEventManager->AddListener(&player_hurt_listener, "player_hurt", false);
}

/* bullet impact */
void bullettracer::bullet_impact_event::FireGameEvent(IGameEvent * p_event)
{
	/* check if we are in game */
	if (!g_pEngine->IsInGame() || !g_pEngine->IsConnected())
		return;
	
	/* return if not event */
	if (!p_event)
		return;

	/* return if localplayer is null */
	if (g::pLocalEntity == nullptr)
		return;

	/* check if we have the bullet tracers enabled */
	if (!g_Settings.visuals.bBulletTracer)
		return;

	/* bullet impact */
	if (strstr(p_event->GetName(), "bullet_impact"))
	{
		Vector position(p_event->GetFloat("x"), p_event->GetFloat("y"), p_event->GetFloat("z"));

		/* get the shooter */
		auto shooter = g_pEntityList->GetClientEntity(g_pEngine->GetPlayerForUserID(p_event->GetInt("userid")));

		if (shooter == nullptr)
			return;

		if (shooter->IsDormant())
			return;

		if (shooter)
			trace_logs.push_back(trace_info(shooter->GetEyePosition(), position, g_pGlobalVars->curtime, p_event->GetInt("userid")));

		if (shooter == g::pLocalEntity)
			color = g_Settings.visuals.cLocalTracer;
		else if (shooter->GetTeam() != g::pLocalEntity->GetTeam())
			color = g_Settings.visuals.cEnemyTracer;
		else
			color = Color(0, 0, 0, 0);
	}
}

/* player hurt */
void bullettracer::player_hurt_event::FireGameEvent(IGameEvent * p_event)
{
	/* return if not event */
	if (!p_event)
		return;

	/* local player */
	auto localplayer = g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer());

	/* get the attacker */
	auto attacker = g_pEntityList->GetClientEntity(g_pEngine->GetPlayerForUserID(p_event->GetInt("attacker")));

	/* check if we are attacker */
	if (attacker == localplayer)
	{
		color = g_Settings.visuals.cLocalHurt;
	}
}

/* draw */
void bullettracer::draw()
{
	/* check if we have the bullet tracers enabled */
	if (!g_Settings.visuals.bBulletTracer)
		return;

	/* if we are connected */
	if (!g_pEngine->IsConnected() || !g_pEngine->IsInGame())
		return;

	for (unsigned int i = 0; i < trace_logs.size(); i++)
	{
		/* draw the beam */
		g_bullettracer.draw_beam(trace_logs[i].start, trace_logs[i].position, color);

		trace_logs.erase(trace_logs.begin() + i);
	}
}