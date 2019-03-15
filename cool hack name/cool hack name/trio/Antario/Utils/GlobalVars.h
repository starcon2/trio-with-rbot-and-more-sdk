#pragma once
#include "..\SDK\CInput.h"
#include "..\SDK\CEntity.h"
#define TICK_INTERVAL (g_pGlobalVars->intervalPerTick)
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )
template<class T, class U>
T clamp(T in, U low, U high) { if (in <= low)return low; if (in >= high)return high; return in; }
// tick to clamp is all compatibilty stuff

namespace g
{
    extern CUserCmd*      pCmd;
    extern C_BaseEntity*  pLocalEntity;
    extern std::uintptr_t uRandomSeed;
	extern Vector         OriginalView;
	extern Vector         AntiAimView;
	extern bool           bSendPacket;
	extern float          fixedforwardmove;
	extern float          fixedsidemove;
	extern bool           fakelag;
	extern bool           autostoping;
	extern Vector         RealAngle;
	using msg_t = void(__cdecl*)(const char*, ...);
	extern msg_t		  pMsg;
}
