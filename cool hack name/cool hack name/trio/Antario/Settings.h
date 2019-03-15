#pragma once
#include "GUI\GUI.h"
#include <filesystem>
#include "Utils\GlobalVars.h"
#include <fstream>
#include <iostream>
#include <functional>
#include <ShlObj.h>
#include "Hooks.h"

using namespace ui;
namespace fs = std::experimental::filesystem;

class Settings : public MenuMain
{
public:
    void Initialize(MenuMain* pMenuObj);

    void SaveSettings(const std::string& strFileName, MenuMain* pMenuObj);
    void LoadSettings(const std::string& strFileName, MenuMain* pMenuObj);

private:
    void UpdateDirectoryContent(const fs::path& fsPath);
    inline fs::path GetFolderPath();

    fs::path                 fsPath{};
    std::vector<std::string> vecFileNames{};

public:
    /* All our settings variables will be here  *
    * The best would be if they'd get          *
    * initialized in the class itself.         */

	int   iAlpha;

    bool  bCheatActive = true;
    bool  bMenuOpened  = false;
	bool  bMenuFullFade = false;

	class rage {
	public:

		/* Legit */

		/* Checkboxes */
		bool  bBacktrack;
		/* Checkboxes */

		/* Legit */
	};

	class visuals {
	public:


		/* Players */

		/* Colors */
		Color cBox            = { 255, 255, 255, 255 };
		Color cName           = { 255, 255, 255, 255 };
		Color cWeaponIcon     = { 255, 255, 255, 255 };
		Color cWeaponName     = { 255, 255, 255, 255 };
		Color cWeaponAmmo     = { 66, 182, 244, 255 };
		Color cSound          = { 233, 88, 243, 255 };
		Color cOffscreenESP   = { 255, 255, 255, 255 };
		Color cGlow			  = { 255, 255, 255, 255 };
		Color cSkeleton		  = { 255, 255, 255, 255 };
		/* Colors */

		/* Checkboxes */
		bool  bBox;
		bool  bDynamic;
		bool  bName;
		bool  bHealth;
		bool  bWeaponIcon;
		bool  bWeaponName;
		bool  bWeaponAmmo;
		bool  bSound;
		bool  bOffscreenESP;
		bool  bGlow;
		bool  bSkeleton;
		bool  bDormant;
		/* Checkboxes */

		/* Multibox */
		bool  bFlags[5];
		/* Multibox */

		/* Players */


		/* Player Models */

		/* Colors */
		Color cBackTrack  = { 255, 255, 255, 255 };
		Color cChams      = { 244, 241, 66, 255 };
		Color cChamsZ     = { 229, 66, 244, 255 };
		Color cLocalChams = { 66, 244, 137, 255 };
		/* Colors */

		/* Checkboxes */
		bool bChams;
		bool bChamsZ;
		bool bLocalChams;
		bool bBlend;
		bool bChamsWhileScoped;
		bool bBackTrackChams;
		/* Checkboxes */

		/* Comboboxes */
		int iTransparency = 100;
		int iMaterial;
		int iMaterialLocal;
		/* Comboboxes */

		/* Player Models */


		/* Effects */

		/*Checkboxes*/
		bool  bNoFlash;
		bool  bRemoveSmoke;
		bool  bRemoveRecoil;
		bool  bRemoveScope;
		bool  bDisablePostProcessing;
		/*Checkboxes*/

		/* Effects */

		
		/* World */

		/* Colors */
		Color cItems = { 255, 255, 255, 255 };
		Color cProjectiles = { 255, 255, 255, 255 };
		Color cBomb = { 255, 255, 255, 255 };
		/* Colors */

		/* Checkboxes */
		bool  bItems;
		bool  bProjectiles;
		bool  bBomb;
		bool  bNightmode;
		bool  bFullBright;
		/* Checkboxes */

		/* World */


		/* Other */

		/* Sliders */
		int iTracerTime;
		int iThirdpersonDistance;
		int iFov;
		int iThirdpersonFov;
		/* Sliders */

		/* Modes */
		int iThirdpersonKeybind = 2;
		/* Modes */

		/* Keybinds */
		ButtonCode_t btThirdperonKeybind;
		/* Keybinds */
		
		/* Colors */
		Color cLocalTracer = { 233, 88, 243, 255 };
		Color cLocalHurt   = { 244, 158, 66, 255 };
		Color cEnemyTracer = { 0, 255, 255, 255 };
		Color cGrenadePred = { 255, 255, 255, 255 };
		/* Colors */

		/*Checkboxes*/
		bool  bBulletTracer;
		bool  bHitmarker;
		bool  bGrenadePrediction;
		bool  bForceCrosshair;
		bool  bHitmarkerSound;
		bool  bSpecThirdperson;
		/*Checkboxes*/

		/* Other */
	};

	class misc {
	public:
		bool  bWatermark = true;
		bool  bEventLogs[2];
		bool  bBhopEnabled;
		bool  bAutoStrafer;
		bool bInfDuck;
		bool bFakeDuck;
		bool bFakeStand;
		int bFakeDuckBindType = 1;
		int bFakeStandBindType = 1;
		bool bSlowWalk;
		int bSlowWalkSpeed;
		int bSlowWalkBindType = 1;
		bool bClanTag;
		ButtonCode_t bFakeDuckKeybind;
		ButtonCode_t bFakeStandKeybind;
		ButtonCode_t bSlowWalkKeybind;
	};

	class config {
	public:
		Color cMenuColor = { 188, 26, 66, 255 };
		int cfgNumber;
	};

	class ragetab { // delete me!
	public:
		//anti aim stuff
		int AntiAimPitch;
		int AntiAimDesync;
		int AntiAimFakelagAmt;
		bool AntiAimFakelagWhile[4];
		int  AntiAimFakelagType;
		bool AntiAimFreestanding;
		ButtonCode_t AntiAimBackward;
		ButtonCode_t AntiAimLeft;
		ButtonCode_t AntiAimRight;

		// rage stuff
		bool RageEnabled;
		int RageHitchance;
		int RageMinimumDamage;
		bool RageAutostop;
		bool RageAutoscope;
		int RagePointScale;
		int RagePointScaleHead;
     	bool RageHitscan[7];
		bool RagePlaceholder;
		bool RageResolver;
	};

	rage     rage;
	visuals  visuals;
	misc     misc;
	config   config;
	ragetab     ragetab;
};

extern Settings g_Settings;

