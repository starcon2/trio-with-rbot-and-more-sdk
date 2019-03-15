#pragma once
#include "..\Utils\Interfaces.h"
#include "..\Utils\GlobalVars.h"
#include "..\Utils\Math.h"
#include "..\SDK\CEntity.h"
#include "..\SDK\IClientEntity.h"
#include "..\SDK\CGlobalVarsBase.h"
#include "..\SDK\CPrediction.h"
#include "..\SDK\Studio.h"
#include "../Settings.h"
#include "..\GUI\Keybind.h"
#include "Autowall.h"


struct PointInfo
{
public:
	PointInfo(float damage = 0.f, Vector point = Vector(0.f, 0.f, 0.f))
	{
		this->damage = damage;
		this->point = point;
	}

	float damage;
	Vector point;
};

struct Target_t
{
public:
	Target_t(C_BaseEntity* pEnt = nullptr, Vector vPos = Vector(0.f, 0.f, 0.f))
	{
		this->pEnt = pEnt;
		this->vPos = vPos;
	}

	C_BaseEntity* pEnt = nullptr;
	Vector vPos = { 0.f, 0.f, 0.f };
};


class Ragebot
{
public:
	void extrapolation();
	void DoRagebot();

private:
	PointInfo GetBestPointByHitbox(C_BaseEntity* pBaseEntity, int iHitbox, matrix3x4_t BoneMatrix[128]);
	bool HitChance(C_BaseEntity* pEnt, C_BaseCombatWeapon* pWeapon, Vector Angle, float chance);
	void FindTarget();
	void Fire();
	float GetInterp();
	std::vector<int> GetHitboxes();
	PointInfo HitScan(C_BaseEntity* pBaseEntity);

	Target_t tTarget;

	Vector m_oldangle;
	float m_oldforward, m_oldsidemove;
};

extern Ragebot g_ragebot;