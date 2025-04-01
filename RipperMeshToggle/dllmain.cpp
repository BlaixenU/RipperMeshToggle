#pragma comment(lib, "Shlwapi.lib")
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

inline void renderPart(PartStruct Part, const char* Name) {
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

		if (ImGui::Begin("RipperMeshToggle v2.2.0", nullptr, ImGuiWindowFlags_None)) {

			ImGui::Text("JSON DEBUG");

			if (ImGui::BeginTabBar("Costumes")) {
				for (auto index = 0; index < Body.size(); index++) {

					if (Body[index].targetBody.size() < 1) {
						break;
					}

					std::string bruh = std::string("Body") + std::to_string(index);
					const char* bodyNum = bruh.c_str();

					if (ImGui::BeginTabItem(bodyNum)) {

						ImGui::Text("\n");

						if ((Body[index].Events.enterEvents.size() > 0) || (Body[index].Events.exitEvents.size() > 0)) {
							if (ImGui::CollapsingHeader("Events")) {
								if (Body[index].Events.enterEvents.size() > 0) {
									ImGui::Text("OnRipperEnter:");
									for (auto it = Body[index].Events.enterEvents.rbegin(); it != Body[index].Events.enterEvents.rend(); ++it) {
										ImGui::Text(it->c_str());
									}
								}
								if (Body[index].Events.exitEvents.size() > 0) {
									ImGui::Text("\n");
									ImGui::Text("OnRipperExit:");
									for (auto it = Body[index].Events.exitEvents.rbegin(); it != Body[index].Events.exitEvents.rend(); ++it) {
										ImGui::Text(it->c_str());
									}
								}
							}
							ImGui::Text("\n");
						}


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

						ImGui::Text("\n");

						renderPart(Body[index].Hair, "Hair");
						renderPart(Body[index].Sheath, "Sheath");

						if (ImGui::CollapsingHeader("Visor")) {

							ImGui::Value("ToggleInRipper", Body[index].Visor.toggleInRipper);
							ImGui::Value("HideInNormal", Body[index].Visor.hideInNormal);
							ImGui::Value("HideInRipper", Body[index].Visor.hideInRipper);
							ImGui::Text("\n");
							ImGui::Value("VisorEnabledInRipper", Body[index].Visor.visorEnabledInRipper);
							ImGui::Value("VisorEnabledInNormal", Body[index].Visor.visorEnabledInNormal);
							ImGui::Text("\n");
						}

						renderPart(Body[index].Head, "Head");

						ImGui::EndTabItem();
					}
				}
				ImGui::EndTabBar();
			}

			ImGui::Text("\n");
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
}