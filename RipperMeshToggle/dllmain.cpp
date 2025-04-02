#pragma comment(lib, "Shlwapi.lib")
#include "pch.h"
#include <assert.h>
#include "gui.h"
#include <Events.h>
#include "RipperMeshToggle.h"

#include "imgui/imgui.h"
#include <string>

using namespace ImGui;

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
	if (CollapsingHeader(Name)) {

		Value("ToggleInRipper", Part.toggleInRipper);
		Value("HideInNormal", Part.hideInNormal);
		Value("HideInRipper", Part.hideInRipper);
		Spacing();
	}
}

void gui::RenderWindow()
{
	if (windowOpened && debugMode) {

		if (Begin("RipperMeshToggle v2.2.0", nullptr, ImGuiWindowFlags_NoCollapse)) {

			/*Text("JSON DEBUG");*/

			if (BeginTabBar("Costumes")) {
				for (auto index = 0; index < Body.size(); index++) {

					if (Body[index].targetBody.size() < 1) {
						break;
					}

					std::string bruh = std::string("Body") + std::to_string(index);
					const char* bodyNum = bruh.c_str();

					if (BeginTabItem(bodyNum)) {

						Spacing();


						if (CollapsingHeader("Body")) {
							Text("TargetBody:");
							for (int index : Body[index].targetBody) {
								BulletText(std::to_string(index).c_str());
							}
							Spacing();
							Value("ToggleInRipper", Body[index].toggleInRipper);
							Value("RipperSize", Body[index].resizeFactor);
							Value("ResetSizeInQTE", Body[index].resetSize);
							Value("ResetSizeRate", Body[index].resetSizeRate);
							Value("ShowVisorAtArmstrong", Body[index].showVisorAtArmstrong);
						}

						Spacing();

						renderPart(Body[index].Hair, "Hair");
						renderPart(Body[index].Sheath, "Sheath");

						if (CollapsingHeader("Visor")) {

							Value("ToggleInRipper", Body[index].Visor.toggleInRipper);
							Value("HideInNormal", Body[index].Visor.hideInNormal);
							Value("HideInRipper", Body[index].Visor.hideInRipper);
							Spacing();
							Value("VisorEnabledInRipper", Body[index].Visor.visorEnabledInRipper);
							Value("VisorEnabledInNormal", Body[index].Visor.visorEnabledInNormal);
							Spacing();
						}

						renderPart(Body[index].Head, "Head");

						Spacing();

						if ((Body[index].Events.enterEvents.size() > 0) || (Body[index].Events.exitEvents.size() > 0)) {
							if (CollapsingHeader("Events")) {
								if (Body[index].Events.enterEvents.size() > 0) {
									Text("OnRipperEnter:");
									for (auto it = Body[index].Events.enterEvents.rbegin(); it != Body[index].Events.enterEvents.rend(); ++it) {
										BulletText(it->c_str());
									}
								}
								Spacing();
								if (Body[index].Events.exitEvents.size() > 0) {
									Text("OnRipperExit:");
									for (auto it = Body[index].Events.exitEvents.rbegin(); it != Body[index].Events.exitEvents.rend(); ++it) {
										BulletText(it->c_str());
									}
								}
							}
							Spacing();
						}

						EndTabItem();
					}
				}
				EndTabBar();
			}

			Spacing();
			Separator();
			Spacing();

			if (pCurrentCostume) {
				Value("Current Costume ID", **pCurrentCostume);
			}
			if (currentPhase) {
				Value("Current Phase", currentPhase);
			}

			End();
		}
	}
}