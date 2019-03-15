#include "hitmarker.h"
#pragma comment(lib, "Winmm.lib")

//=====================================================================//
// most of this stuff friggin pasted from floody software & subliminal //
//=====================================================================//

/* variables */
int hitmarkerTime = 0;

/* declarations */
hitmarker::player_hurt_event player_hurt_listener;

/* hitmarker */
hitmarker g_hitmarker;

/* functions */
void hitmarker::listener()
{
	g_pEventManager->AddListener(&player_hurt_listener, "player_hurt", false);
}

void hitmarker::run()
{
	if (!g_pEngine->IsConnected() && !g_pEngine->IsInGame())
		return;

	/* draw hitmarker if enabled */
	if (g_Settings.visuals.bHitmarker || g_Settings.visuals.bHitmarkerSound)
		this->draw();
}

void hitmarker::player_hurt_event::FireGameEvent(IGameEvent * p_event)
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
		/* set the hitmarker time to 255 */
		hitmarkerTime = 255;

		if (g_Settings.visuals.bHitmarkerSound)
			g_pSurface->play_sound("buttons\\arena_switch_press_02.wav");
	}
}

void hitmarker::draw()
{
	/* screen size */
	int screenWidth, screenHeight;
	g_pEngine->GetScreenSize(screenWidth, screenHeight);

	/* screen middle */
	int ScreenWidthMid = screenWidth / 2;
	int ScreenHeightMid = screenHeight / 2;

	/* draw the hitmarker */
	if (hitmarkerTime > 0)
	{
		/* set alpha to hitmarker time */
		float alpha = hitmarkerTime;

		/* screen */
		if (g_Settings.visuals.bHitmarker)
		{
			g_Render.Line(ScreenWidthMid - 5, ScreenHeightMid - 5, ScreenWidthMid + 5, ScreenHeightMid + 5, Color(255, 255, 255, alpha));
			g_Render.Line(ScreenWidthMid - 6, ScreenHeightMid - 6, ScreenWidthMid + 6, ScreenHeightMid + 6, Color(255, 255, 255, alpha));
			g_Render.Line(ScreenWidthMid + 5, ScreenHeightMid - 5, ScreenWidthMid - 5, ScreenHeightMid + 5, Color(255, 255, 255, alpha));
			g_Render.Line(ScreenWidthMid + 6, ScreenHeightMid - 6, ScreenWidthMid - 6, ScreenHeightMid + 6, Color(255, 255, 255, alpha));
		}

		/* hitmarker fade animation */
		hitmarkerTime -= 2;
	}
}