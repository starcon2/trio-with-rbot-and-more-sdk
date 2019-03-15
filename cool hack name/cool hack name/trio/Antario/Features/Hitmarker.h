#pragma once
#include "..\SDK\IGameEvent.h"
#include "..\SDK\Vector.h"
#include "..\SDK\IClientEntity.h"
#include "..\SDK\CGlobalVarsBase.h"
#include "..\SDK\IClientEntityList.h"
#include "..\Utils\GlobalVars.h"
#include "..\SDK\ISurface.h"
#include "..\SDK\Singleton.h"
#include "..\Settings.h"
#include "..\SDK\IViewRenderBeams.h"

/* definitions */
#define create_event_listener(class_name)\
class class_name : public IGameEventListener2\
{\
public:\
	~class_name() { g_pEventManager->RemoveListener(this); }\
\
	virtual void FireGameEvent(IGameEvent* p_event);\
};\

/* functions */
class hitmarker : public singleton <hitmarker>
{
public:
	void run();
	void listener();
	create_event_listener(player_hurt_event);
protected:
	void draw();
};

/* extern hitmarker */
extern hitmarker g_hitmarker;
