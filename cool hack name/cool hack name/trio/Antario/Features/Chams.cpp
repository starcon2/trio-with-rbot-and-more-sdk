#include "Chams.h"
#include "..\Legit\Backtrack.h"

Chams g_chams;

void Chams::Render()
{
	static bool initilized = false;

	/* return if we are not connected & in game */
	if (!g_pEngine->IsConnected() && !g_pEngine->IsInGame())
		return;

	/* material */
	IMaterial* material               = nullptr;
	IMaterial* materialZ              = nullptr;

	IMaterial* materialLocal		  = nullptr;

	static IMaterial* materialRegular;
	static IMaterial* materialRegularIgnoreZ;
	static IMaterial* materialFlatIgnoreZ;
	static IMaterial* materialFlat;
	static IMaterial* materialRubberZ;
	static IMaterial* materialRubber;
	static IMaterial* materialMetallicZ;
	static IMaterial* materialMetallic;

	if (!initilized)
	{
		std::ofstream("csgo\\materials\\chamsMetallic.vmt") << R"#("VertexLitGeneric"
		{
		  "$basetexture" "vgui/white_additive"
		  "$ignorez"      "0"
		  "$envmap"       "env_cubemap"
		  "$nofog"        "1"
		  "$model"        "1"
		  "$nocull"       "0"
		  "$selfillum"    "1"
		  "$halflambert"  "1"
		  "$znearer"      "0"
		  "$flat"         "1"
		  "$rimlight"     "1"
		  "$rimlightexponent"  "2"
		  "$rimlightboost"     ".2"	
		  "$reflectivity" "[1 1 1]"
		}
		)#";

		std::ofstream("csgo\\materials\\chamsZMetallic.vmt") << R"#("UnlitGeneric"
		{
		  "$basetexture" "vgui/white_additive"
		  "$ignorez"      "1"
		  "$envmap"       "env_cubemap"
		  "$nofog"        "1"
		  "$model"        "1"
		  "$nocull"       "0"
		  "$selfillum"    "1"
		  "$halflambert"  "1"
		  "$znearer"      "0"
		  "$flat"         "1"
		  "$rimlight"     "1"
		  "$rimlightexponent"  "2"
		  "$rimlightboost"     ".2"	
		  "$reflectivity" "[1 1 1]"
		}
		)#";

		std::ofstream("csgo\\materials\\cherry_regular.vmt") << R"#("VertexLitGeneric"
		{
		  "$basetexture" "vgui/white_additive"
		  "$ignorez"      "0"
		  "$envmap"       ""
		  "$nofog"        "1"
		  "$model"        "1"
		  "$nocull"       "0"
		  "$selfillum"    "1"
		  "$halflambert"  "1"
		  "$znearer"      "0"
		  "$flat"         "1"
		  "$reflectivity" "[1 1 1]"
		}
		)#";
		std::ofstream("csgo\\materials\\cherry_ignorez.vmt") << R"#("VertexLitGeneric"
		{
		  "$basetexture" "vgui/white_additive"
		  "$ignorez"      "1"
		  "$envmap"       ""
		  "$nofog"        "1"
		  "$model"        "1"
		  "$nocull"       "0"
		  "$selfillum"    "1"
		  "$halflambert"  "1"
		  "$znearer"      "0"
		  "$flat"         "1"
		  "$reflectivity" "[1 1 1]"
		}
		)#";
		std::ofstream("csgo\\materials\\cherry_flat.vmt") << R"#("UnlitGeneric"
		{
		  "$basetexture" "vgui/white_additive"
		  "$ignorez"      "0"
		  "$envmap"       ""
		  "$nofog"        "1"
		  "$model"        "1"
		  "$nocull"       "0"
		  "$selfillum"    "1"
		  "$halflambert"  "1"
		  "$znearer"      "0"
		  "$flat"         "1"
		}
		)#";
		std::ofstream("csgo\\materials\\cherry_flat_ignorez.vmt") << R"#("UnlitGeneric"
		{
		  "$basetexture" "vgui/white_additive"
		  "$ignorez"      "1"
		  "$envmap"       ""
		  "$nofog"        "1"
		  "$model"        "1"
		  "$nocull"       "0"
		  "$selfillum"    "1"
		  "$halflambert"  "1"
		  "$znearer"      "0"
		  "$flat"         "1"
		}
		)#";

		materialRegular          = g_MatSystem->FindMaterial("cherry_regular",       TEXTURE_GROUP_MODEL);
		materialRegularIgnoreZ   = g_MatSystem->FindMaterial("cherry_ignorez",       TEXTURE_GROUP_MODEL);
		materialFlatIgnoreZ      = g_MatSystem->FindMaterial("cherry_flat_ignorez",  TEXTURE_GROUP_MODEL);
		materialFlat		     = g_MatSystem->FindMaterial("cherry_flat",          TEXTURE_GROUP_MODEL);
		materialMetallicZ	     = g_MatSystem->FindMaterial("chamsZMetallic",       TEXTURE_GROUP_MODEL);
		materialMetallic	     = g_MatSystem->FindMaterial("chamsMetallic",        TEXTURE_GROUP_MODEL);
		initilized			     = true;
	}

	if      (g_Settings.visuals.iMaterial == 0)		 { material = materialFlat; materialZ = materialFlatIgnoreZ;       }
	else if (g_Settings.visuals.iMaterial == 1)		 { material = materialRegular; materialZ = materialRegularIgnoreZ; }
	else if (g_Settings.visuals.iMaterial == 2)		 { material = materialMetallic; materialZ = materialMetallicZ;     }

	if      (g_Settings.visuals.iMaterialLocal == 0) { materialLocal = materialFlat;     }
	else if (g_Settings.visuals.iMaterialLocal == 1) { materialLocal = materialRegular;  }
	else if (g_Settings.visuals.iMaterialLocal == 2) { materialLocal = materialMetallic; }

	for (auto i = 0; i < g_pEntityList->GetHighestEntityIndex(); i++)
	{
		/* entity pointer */
		C_BaseEntity* entity = g_pEntityList->GetClientEntity(i);

		/* local player pointer */
		C_BaseEntity* localplayer = g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer());

		/* return if not entity, or teammate */
		if (!entity || entity->IsDormant() || entity->GetHealth() <= 0)
			continue;

		if (entity->GetClientClass()->ClassID != CCSPlayer)
			continue;

		/* this is quite aids */
		static bool scopeblend;

		if (localplayer->GetScoped() && g_Settings.visuals.bChamsWhileScoped)
			scopeblend = false;
		else
			scopeblend = true;


		/* local player chams*/
		if (entity == localplayer && localplayer != nullptr && scopeblend) /* <- remove this if you dont want the chams while scoped */
		{
			// vis
			if (g_Settings.visuals.bLocalChams) {
				g_pRenderView->SetColorModulation(g_Settings.visuals.cLocalChams.red / 255.f, g_Settings.visuals.cLocalChams.green / 255.f, g_Settings.visuals.cLocalChams.blue / 255.f);
				g_MdlRender->ForcedMaterialOverride(materialLocal);
				entity->DrawModel(0x1, RenderableInstance_t{ 255 });
			}
		}

		/* enemy chams */
		else if (entity->GetTeam() != localplayer->GetTeam() && entity != nullptr)
		{
			// invis
			if (g_Settings.visuals.bChamsZ) {
				g_pRenderView->SetColorModulation(g_Settings.visuals.cChamsZ.red / 255.f, g_Settings.visuals.cChamsZ.green / 255.f, g_Settings.visuals.cChamsZ.blue / 255.f);
				g_MdlRender->ForcedMaterialOverride(materialZ);
				entity->DrawModel(0x1, RenderableInstance_t{ 255 });
			}

			// vis
			if (g_Settings.visuals.bChams) {
				g_pRenderView->SetColorModulation(g_Settings.visuals.cChams.red / 255.f, g_Settings.visuals.cChams.green / 255.f, g_Settings.visuals.cChams.blue / 255.f);
				g_MdlRender->ForcedMaterialOverride(material);
				entity->DrawModel(0x1, RenderableInstance_t{ 255 });
			}
		}

	/*	Vector oldOrigin = entity->GetVecOrigin();
		Vector oldAngle = entity->GetEyeAngles();

		if (g_Settings.visuals.bBackTrackChams && g_Settings.rage.bBacktrack)
		{
			if (entity != localplayer && entity->GetTeam() != localplayer->GetTeam())
			{
				for (int r = 0; r < 12; r++)
				{
					backtrack_data currentrecord = entity_data[i][r];

					if (!g_Backtrack->IsValidBackTrackTick(currentrecord.simtime))
						continue;

					entity->SetOrigin(currentrecord.Angle);
					entity->SetOrigin(currentrecord.Origin);
					Vector temp = entity->GetVecOrigin();
					g_pRenderView->SetColorModulation(g_Settings.visuals.cBackTrack.red / 255.f, g_Settings.visuals.cBackTrack.green / 255.f, g_Settings.visuals.cBackTrack.blue / 255.f);
					g_MdlRender->ForcedMaterialOverride(materialZ);
					g_pRenderView->SetBlend(.15f);
					entity->DrawModel(0x1, RenderableInstance_t{ 255 });
				}

				entity->SetAngles(oldAngle);
				entity->SetOrigin(oldOrigin);
			}
		}*/

		/* set material to null */
		g_MdlRender->ForcedMaterialOverride(nullptr);
	}
}