#pragma once
#include "..\Utils\Interfaces.h"
#include "..\Utils\GlobalVars.h"
#include "..\Utils\Math.h"
#include "..\SDK\CEntity.h"
#include "..\SDK\IClientEntity.h"
#include "..\SDK\CGlobalVarsBase.h"
#include "..\SDK\IVModelInfoClient.h"
#include "..\SDK\Studio.h"

/* functions */
class Zeusbot
{
public:
	void run();

public:
	void target(C_BaseEntity * pEnt, float fov, Vector location, float simulationtime, bool backtrack);
};

extern Zeusbot g_zeusbot;