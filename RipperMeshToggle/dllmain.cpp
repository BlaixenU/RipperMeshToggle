#include "pch.h"
#include <assert.h>
#include "gui.h"
#include <Events.h>
#include "RipperMeshToggle.h"

#include "imgui/imgui.h"
#include <string>

static bool windowOpened = false;

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
		Events::OnSceneStartupEvent.after += ripperReinit;
		Events::OnApplicationStartEvent.after += mainInit;
		Events::OnUpdateEvent.after += ripperTick;

		Events::OnUpdateEvent.after += []() {
			if (shared::IsKeyPressed('L', false) && debugMode) {
				windowOpened ? windowOpened = false : windowOpened = true;
			}
		};
	}
} plugin;


void gui::RenderWindow()
{
	if (windowOpened && debugMode) {

		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowPadding = ImVec2(8, 8); 
		style.WindowRounding = 4.0f; 

		ImGui::Begin("Debug - Toggle with L", nullptr, ImGuiWindowFlags_None);

		ImGui::Text("Bodies");
		if (ImGui::BeginTabBar("Costumes")) {
			for (int index = 0; index < 12; index++) {

				std::string bruh = std::to_string(index);
				const char* bodyIndex = bruh.c_str();

				if (ImGui::BeginTabItem(bodyIndex)) {
					ImGui::Value("targetBody", targetBody[index]);
					ImGui::Value("resizeFactor", resizeFactor[index]);

					ImGui::Text("\n");

					ImGui::Value("resetSize", resetSize[index]);
					ImGui::Value("resetSizeRate", resetSizeRate[index]);

					ImGui::Text("\n");

					ImGui::Value("IncludeHair", IncludeHair[index]);
					ImGui::Value("IncludeSheath", IncludeSheath[index]);
					ImGui::Value("IncludeVisor", IncludeVisor[index]);
					ImGui::Value("IncludeHead", IncludeHead[index]);
					ImGui::Value("IncludeMainWeapon", IncludeMainWeapon[index]);
					ImGui::Value("IncludeUniqueWeapon", IncludeUniqueWeapon[index]);
					
					ImGui::Text("\n");


					ImGui::Value("HideHair", HideHair[index]);
					ImGui::Value("HideSheath", HideSheath[index]);
					ImGui::Value("HideVisor", HideVisor[index]);
					ImGui::Value("HideHead", HideHead[index]);

					ImGui::EndTabItem();
				}
			}
			ImGui::EndTabBar();
		}
		
		ImGui::Text("\n");
		if (pCurrentCostume) {
			ImGui::Value("Current Costume ID", **pCurrentCostume);
		}

		ImGui::End();
	}
	
}