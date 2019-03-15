#include "Nightmode.h"

nightmode g_nightmode;

nightmode::client_disconnect_event client_disconnect_listener;

bool executed = false;

ConVar * sv_skyname = nullptr;
std::string fallback_skybox = "";

std::vector<MaterialBackup> materials;
std::vector<MaterialBackup> skyboxes;

/* listeners */
void nightmode::listener()
{
	g_pEventManager->AddListener(&client_disconnect_listener, "client_disconnect", false);
}

/* client disconnects */
void nightmode::client_disconnect_event::FireGameEvent(IGameEvent * p_event)
{
	/* client disconnects (again lol) */
	if (strstr(p_event->GetName(), "client_disconnect"))
	{
		/* if we disconnect, remove the material (no red text) */
		g_nightmode.remove();
	}
}

/* clear materials */
void nightmode::clear_stored_materials()
{
	fallback_skybox = "";
	sv_skyname = nullptr;
	materials.clear();
	skyboxes.clear();
}

/* modulate materials */
void nightmode::modulate(MaterialHandle_t i, IMaterial *material, bool backup = false) 
{
	if (strstr(material->GetTextureGroupName(), "World")) {
		if (backup) materials.push_back(MaterialBackup(i, material));

		material->ColorModulate(0.2f, 0.2f, 0.2f);
	}
	else if (strstr(material->GetTextureGroupName(), "StaticProp")) {
		if (backup)
			materials.push_back(MaterialBackup(i, material));

		material->ColorModulate(0.5f, 0.5f, 0.5f);
	}
	else if (strstr(material->GetTextureGroupName(), "SkyBox")) {
		if (backup)
			materials.push_back(MaterialBackup(i, material));

		material->ColorModulate(196 / 255.f, 113 / 255.f, 193 / 255.f);
	}
}

/* apply to materials */
void nightmode::apply() 
{
	executed = true;

	if (!sv_skyname) {
		sv_skyname = g_pCvar->FindVar("sv_skyname");
		sv_skyname->nFlags &= ~FCVAR_CHEAT;
	}

	auto drawspecificstaticprop = g_pCvar->FindVar("r_drawspecificstaticprop");
	auto drawspecificstaticpropspoof = new SpoofedConvar(drawspecificstaticprop);
	drawspecificstaticpropspoof->SetInt(0);

	if (materials.size()) {
		for (int i = 0; i < (int)materials.size(); i++)
			modulate(materials[i].handle, materials[i].material);

		return;
	}

	materials.clear();

	for (MaterialHandle_t i = g_MatSystem->FirstMaterial(); i != g_MatSystem->InvalidMaterial(); i = g_MatSystem->NextMaterial(i)) {
		IMaterial* material = g_MatSystem->GetMaterial(i);

		if (!material || material->IsErrorMaterial()) {
			continue;
		}

		if (material->GetReferenceCount() <= 0) {
			continue;
		}

		modulate(i, material, true);
	}
}

/* remove the materials */
void nightmode::remove() 
{
	executed = false;
	g_Settings.visuals.bNightmode = false;

	if (sv_skyname) 
	{
		sv_skyname->SetValue(fallback_skybox.c_str());
	}

	auto drawspecificstaticprop = g_pCvar->FindVar("r_drawspecificstaticprop");
	auto drawspecificstaticpropspoof = new SpoofedConvar(drawspecificstaticprop);
	drawspecificstaticpropspoof->SetInt(1);

	for (int i = 0; i < materials.size(); i++) {
		if (materials[i].material->GetReferenceCount() <= 0) continue;

		materials[i].restore();
		materials[i].material->Refresh();
	}

	materials.clear();

	sv_skyname = nullptr;
	fallback_skybox = "";
}