#pragma once
#include "..\Utils\GlobalVars.h"
#include "..\Settings.h"

/* functions */
class triggerbot
{
public:
	void trigger(CUserCmd *pCmd);
};

extern triggerbot g_triggerbot;