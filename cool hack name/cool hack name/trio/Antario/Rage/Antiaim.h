#pragma once
#include "..\Utils\Interfaces.h"
#include "..\Utils\GlobalVars.h"
#include "..\Utils\Math.h"
#include "..\SDK\CEntity.h"
#include "..\SDK\IClientEntity.h"
#include "..\SDK\CGlobalVarsBase.h"
#include "..\SDK\IVModelInfoClient.h"
#include "..\SDK\CPrediction.h"
#include "..\SDK\Studio.h"
#include "../Settings.h"
#include "..\GUI\Keybind.h"
#include "Autowall.h"



class Antiaim
{
public:
	void MovementFix(QAngle vOldAngles, CUserCmd* pCmd, float fOldForward, float fOldSidemove);
	void DoAntiAim();
	void StartMoveFix();
	void EndMoveFix();

};

extern Antiaim g_antiaim;