#include "GlobalVars.h"

namespace g
{
    CUserCmd*      pCmd         = nullptr;
    C_BaseEntity*  pLocalEntity = nullptr;
    std::uintptr_t uRandomSeed  = NULL;
	Vector         OriginalView;
	Vector         AntiAimView;
	bool           bSendPacket = true;
	float          fixedforwardmove;
	float          fixedsidemove;
	bool           fakelag;
	bool           autostoping;
	Vector         RealAngle;
	using msg_t = void(__cdecl*)(const char*, ...);
	msg_t		   pMsg = reinterpret_cast< msg_t >(GetProcAddress(GetModuleHandleA("tier0.dll"), "Msg"));
}