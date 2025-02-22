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

inline void renderStruct(Part Part, const char* Name) {
	if (ImGui::CollapsingHeader(Name)) {

		ImGui::Value("ToggleInRipper", Part.toggleInRipper);
		ImGui::Value("HideInNormal", Part.hideInNormal);
		ImGui::Value("HideInRipper", Part.hideInRipper);

	}
}

void gui::RenderWindow()
{
	if (windowOpened && debugMode) {

		ImGui::Begin("Debug - Toggle with L", nullptr, ImGuiWindowFlags_None);

		ImGui::Text("Bodies");
		if (ImGui::BeginTabBar("Costumes")) {
			for (int index = 0; index < 12; index++) {

				std::string bruh = std::to_string(index);
				const char* bodyIndex = bruh.c_str();

				if (ImGui::BeginTabItem(bodyIndex)) {

					ImGui::Text("\n");

					ImGui::Value("targetBody", targetBody[index]);
					ImGui::Value("resizeFactor", resizeFactor[index]);
					ImGui::Value("visorBypass", visorBypass[index]);
					ImGui::Value("resetSize", resetSize[index]);
					ImGui::Value("resetSizeRate", resetSizeRate[index]);

					ImGui::Text("\n");

					renderStruct(Hair[index], "Hair");
					renderStruct(Sheath[index], "Sheath");
					renderStruct(Visor[index], "Visor");
					renderStruct(Head[index], "Head");

					ImGui::EndTabItem();
				}
			}
			ImGui::EndTabBar();
		}
		
		ImGui::Text("\n");
		if (pCurrentCostume) {
			ImGui::Value("Current Costume ID", **pCurrentCostume);
		}
		if (currentPhase) {
			ImGui::Value("Current Phase", currentPhase);
		}

		ImGui::End();
	}
	
}