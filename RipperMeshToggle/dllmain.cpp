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

inline void renderPart(Part Part, const char* Name) {
	if (ImGui::CollapsingHeader(Name)) {

		ImGui::Value("ToggleInRipper", Part.toggleInRipper);
		ImGui::Value("HideInNormal", Part.hideInNormal);
		ImGui::Value("HideInRipper", Part.hideInRipper);
		ImGui::Text("\n");
	}
}

void gui::RenderWindow()
{
	if (windowOpened && debugMode) {

		ImGui::Begin("Debug - v2.2.0", nullptr, ImGuiWindowFlags_None);

		ImGui::Text("Bodies");
		ImGui::Separator();

		if (ImGui::BeginTabBar("Costumes")) {
			for (int index = 0; index < 12; index++) {

				std::string bruh = std::to_string(index);
				const char* bodyIndex = bruh.c_str();

				if (ImGui::BeginTabItem(bodyIndex)) {

					ImGui::Text("\n");

					if (ImGui::CollapsingHeader("Events")) {
						ImGui::Text("OnRipperEnter:");
						for (auto it = Events[index].enterEvents.rbegin(); it != Events[index].enterEvents.rend(); ++it) {
							ImGui::Text(it->c_str());
						}
						ImGui::Text("\n");
						ImGui::Text("OnRipperExit:");
						for (auto it = Events[index].exitEvents.rbegin(); it != Events[index].exitEvents.rend(); ++it) {
							ImGui::Text(it->c_str());
						}
					}

					ImGui::Text("\n");
					ImGui::Separator();
					ImGui::Text("\n");

					if (ImGui::CollapsingHeader("Body")) {
						for (int index : Body[index].targetBody) {
							ImGui::Value("Costume ID", index);
						}
						ImGui::Value("ToggleInRipper", Body[index].toggleInRipper);
						ImGui::Value("RipperSize", Body[index].resizeFactor);
						ImGui::Value("ResetSizeInQTE", Body[index].resetSize);
						ImGui::Value("ResetSizeRate", Body[index].resetSizeRate);
						ImGui::Value("ShowVisorAtArmstrong", Body[index].showVisorAtArmstrong);
					}

					ImGui::Separator();
					ImGui::Text("\n");

					renderPart(Hair[index], "Hair");
					renderPart(Sheath[index], "Sheath");

					if (ImGui::CollapsingHeader("Visor")) {

						ImGui::Value("ToggleInRipper", Visor[index].toggleInRipper);
						ImGui::Value("HideInNormal", Visor[index].hideInNormal);
						ImGui::Value("HideInRipper", Visor[index].hideInRipper);
						ImGui::Text("\n");
						ImGui::Value("VisorEnabledInRipper", Visor[index].visorEnabledInRipper);
						ImGui::Value("VisorEnabledInNormal", Visor[index].visorEnabledInNormal);
						ImGui::Text("\n");
					}

					renderPart(Head[index], "Head");

					ImGui::EndTabItem();
				}
			}
			ImGui::EndTabBar();
		}

		ImGui::Text("\n");
		ImGui::Separator();
		ImGui::Text("\n");

		if (pCurrentCostume) {
			ImGui::Value("Current Costume ID", **pCurrentCostume);
		}
		if (currentPhase) {
			ImGui::Value("Current Phase", currentPhase);
		}

		ImGui::Text("\n");
		ImGui::Text("Part Existence");
		ImGui::Text("\n");
		ImGui::Value("Hair Exists", hairExists);
		ImGui::Value("Sheath Exists", sheathExists);
		ImGui::Value("Visor Exists", visorExists);
		ImGui::Value("Head Exists", headExists);
		ImGui::Text("\n");
		ImGui::Value("Event Call Count", callCount);

		ImGui::End();
	}
	
}