#include "Logs.h"

/* declarations */
Logs::player_hurt_event player_hurt_listener;

/* logs */
Logs g_logs;

typedef void(__cdecl* MsgFn)(const char* msg, va_list);
inline void ConMsg(const char* msg, ...)
{
	if (msg == nullptr)
		return; //If no string was passed, or it was null then don't do anything
	static MsgFn fn = (MsgFn)GetProcAddress(GetModuleHandleA("tier0.dll"), "Msg"); //This gets the address of export "Msg" in the dll "tier0.dll". The static keyword means it's only called once and then isn't called again (but the variable is still there)
	char buffer[989];
	va_list list; //Normal varargs stuff http://stackoverflow.com/questions/10482960/varargs-to-printf-all-arguments
	va_start(list, msg);
	vsprintf_s(buffer, msg, list);
	va_end(list);
	fn(buffer, list); //Calls the function, we got the address above.
}

/* listeners */
void Logs::listener()
{
	/* return if we don't have enabled */
	if (!g_Settings.misc.bEventLogs)
		return;

	g_pEventManager->AddListener(&player_hurt_listener, "player_hurt", false);
}

/* getting player info */
PlayerInfo_s GetInfo(int Index) {
	PlayerInfo_s Info;
	g_pEngine->GetPlayerInfo(Index, &Info);
	return Info;
}

/* hitgroup */
auto HitgroupToString = [](int hitgroup) -> char*
{
	if (hitgroup == 1)
		return "head";
	else if (hitgroup == 2)
		return "chest";
	else if (hitgroup == 3)
		return "stomach";
	else if (hitgroup == 4)
		return "left arm";
	else if (hitgroup == 5)
		return "right arm";
	else if (hitgroup == 6)
		return "left leg";
	else if (hitgroup == 7)
		return "right leg";
	else if (hitgroup == 8)
		return "body";
};

/* player hurt */
void Logs::player_hurt_event::FireGameEvent(IGameEvent * p_event)
{
	/* check if we are in game */
	if (!g_pEngine->IsInGame() || !g_pEngine->IsConnected())
		return;

	/* player hurt */
	if (strstr(p_event->GetName(), "player_hurt"))
	{
		/* local player */
		auto localplayer = g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer());

		/* get the attacker */
		auto attacker = p_event->GetInt("attacker");
		
		/* get the attacker for checking */
		auto attackercheck = g_pEntityList->GetClientEntity(g_pEngine->GetPlayerForUserID(p_event->GetInt("attacker")));

		/* used for checking if we are getting damaged */
		auto shooter = g_pEntityList->GetClientEntity(g_pEngine->GetPlayerForUserID(p_event->GetInt("userid")));


		/* get the guy who we are shooting */
		auto dead = p_event->GetInt("userid");

		int hitgroup = p_event->GetInt("hitgroup");

		if (attackercheck == localplayer)
		{
			if (attacker || dead)
			{
				if (!g_Settings.misc.bEventLogs[0])
					return;

				char* szHitgroup = HitgroupToString(hitgroup);
				PlayerInfo_s killed_info = GetInfo(g_pEngine->GetPlayerForUserID(dead));
				PlayerInfo_s killer_info = GetInfo(g_pEngine->GetPlayerForUserID(attacker));

				g_pCvar->console_printf("[");
				g_pCvar->console_color_printf(g_Settings.config.cMenuColor, "drifttwohook");
				g_pCvar->console_printf("] ");

				/* top left */
				ConMsg("hit %s for %s dmg in the %s ( %s health remaining )\n",
					killed_info.szName, p_event->GetString("dmg_health"), szHitgroup, p_event->GetString("health"));
			}
		}

		if (attacker || dead)
		{
			if (!g_Settings.misc.bEventLogs[1])
				return;

			char* szHitgroup = HitgroupToString(hitgroup);
			PlayerInfo_s killed_info = GetInfo(g_pEngine->GetPlayerForUserID(dead));
			PlayerInfo_s killer_info = GetInfo(g_pEngine->GetPlayerForUserID(attacker));

			if (shooter == localplayer)
			{
				g_pCvar->console_printf("[");
				g_pCvar->console_color_printf(g_Settings.config.cMenuColor, "drifttwohook");
				g_pCvar->console_printf("] ");

				/* top left */
				ConMsg("harmed by %s for %s dmg in the %s\n",
					killer_info.szName, p_event->GetString("dmg_health"), szHitgroup);
			}
		}
	}
}