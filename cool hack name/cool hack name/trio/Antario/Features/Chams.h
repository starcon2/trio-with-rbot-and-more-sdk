#pragma once
#include "..\Settings.h"
#include "..\Utils\Interfaces.h"
#include "..\SDK\IMaterialSystem.h"
#include "..\SDK\IRenderView.h"

class Chams
{
public:
	void Render();
};

extern Chams g_chams;