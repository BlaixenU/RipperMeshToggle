#include "pch.h"
#include <assert.h>
#include "gui.h"
#include <Events.h>
#include "RipperMeshToggle.h"

#include "imgui/imgui.h"
#include <string>

bool debugMode = false;

void swapbool() {
	if (check) {
		check = false;
	}
	else {
		check = true;
	}
}

class Plugin
{
public:
	static inline void InitGUI()
	{
		Events::OnDeviceReset.before += gui::OnReset::Before;
		Events::OnDeviceReset.after += gui::OnReset::After;
		Events::OnEndScene += gui::OnEndScene; 
		/* // Or if you want to switch it to Present
		Esvents::OnPresent += gui::OnEndScene;
		*/
	}

	Plugin()
	{
		InitGUI();

		// and here's your code
		Events::OnSceneStartupEvent += ripperReinit;
		Events::OnSceneStartupEvent += swapbool;
		Events::OnApplicationStartEvent += mainInit;
		Events::OnUpdateEvent += ripperTick;
	}
} plugin;


void gui::RenderWindow()
{
	if (debugMode) {
		ImGui::Begin("Values");

		ImGui::Value("targetBody", targetBody);
		ImGui::Value("IncludeHair", IncludeHair);
		ImGui::Value("IncludeSheath", IncludeSheath);
		ImGui::Value("IncludeVisor", IncludeVisor);
		ImGui::Value("IncludeHead", IncludeHead);
		ImGui::Value("IncludeMainWeapon", IncludeMainWeapon);
		ImGui::Value("IncludeUniqueWeapon", IncludeUniqueWeapon);
		ImGui::Value("HideHair", HideHair);
		ImGui::Value("HideSheath", HideSheath);
		ImGui::Value("HideVisor", HideVisor);
		ImGui::Value("HideHead", HideHead);
		ImGui::Value("MainWeaponIndex", MainWeaponIndex);
		ImGui::Value("UniqueWeaponIndex", UniqueWeaponIndex);
		ImGui::Value("ripperSwitch", ripperSwitch);
		ImGui::Value("hasInitialized", hasInitialized);
		ImGui::Value("i", i);
		ImGui::Value("num", num);
		ImGui::Value("check", check);
		ImGui::Value("bodyModelIndex", bodyModelIndex);
		ImGui::Value("resetSize", resetSize);
		ImGui::Value("resizeFactor", resizeFactor);
		ImGui::Value("resizeFactorEase", resizeFactorEase);



		ImGui::End();
	}
	
}