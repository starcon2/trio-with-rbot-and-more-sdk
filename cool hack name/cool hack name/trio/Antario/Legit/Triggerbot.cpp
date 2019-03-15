#include "Triggerbot.h"

triggerbot g_triggerbot;

void trigger(CUserCmd *pCmd)
{
	/* check if we are in game and connected */
	if (!g_pEngine->IsInGame() && !g_pEngine->IsConnected())
		return;

	/* return if localplayer is nullptr */
	if (g::pLocalEntity == nullptr)
		return;
}