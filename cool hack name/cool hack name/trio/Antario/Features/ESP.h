#pragma once
#include "..\Utils\DrawManager.h"
#include "..\Utils\GlobalVars.h"
#include "..\Utils\Utils.h"
#include "..\Utils\Math.h"
#include "..\SDK\IVEngineClient.h"
#include "..\SDK\PlayerInfo.h"
#include "..\SDK\Definitions.h"
#include "..\SDK\CGlobalVarsBase.h"
#include "..\SDK\ClientClass.h"
#include "..\SDK\CDebugOverlay.h"
#include "..\SDK\IMaterialSystem.h"
#include "..\SDK\IViewRenderBeams.h"
#include "..\SDK\IEngineTrace.h"
#include "..\SDK\IWeaponSystem.h"
#include "..\SDK\Cvar.h"
#include "..\SDK\IVModelInfoClient.h"
#include "..\SDK\SpoofedConVar.h"
#include "..\Settings.h"

class ESP
{
public:
	void Render();
private:
	float esp_alpha_fade[64];

	struct box_data
	{
		int x, y, w, h;
	};

	void PaintBox(box_data box, Color colour, int alpha, int outline_alpha);
	void PaintName(box_data box, C_BaseEntity * entity, Color colour, int alpha, int outline_alpha);
	void PaintHealth(box_data box, C_BaseEntity * entity, Color colour, int alpha, int outline_alpha, int inline_alpha);
	void PaintBomb(C_BaseEntity * entity);
	void PaintDroppedWeapons(C_BaseEntity * entity, box_data box);
	void PaintProjectiles(C_BaseEntity * entity);
	void OffscreenESP(Color colour);
	void PaintGrenadePrediction();
	void TraceCircle(Vector position, Color color);
	void PaintAmmo(box_data box, C_BaseEntity * entity, Color colour, int alpha, int outline_alpha, int inline_alpha);
	void PaintWeaponName(box_data box, C_BaseEntity * entity, Color colour, int alpha, int outline_alpha);
	void PaintFlags(box_data box, C_BaseEntity * entity, int alpha, int outline_alpha);
	void PaintSkeleton(C_BaseEntity * entity, Color colour, int alpha);
};
extern ESP g_ESP;
