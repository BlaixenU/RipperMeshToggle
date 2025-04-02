#pragma comment(lib, "Shlwapi.lib")
#include "pch.h"
#include <assert.h>
#include "gui.h"
#include <Events.h>
#include "imgui/imgui.h"
#include <cWeaponSelectMenu.h>
#include <PlayerManagerImplement.h>
#include <string>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <random>
#include <Trigger.h>
#include <GameMenuStatus.h>
#include <Pl0000.h>
#include <EntitySystem.h>
#include <Entity.h>
#include <cGameUIManager.h>
#include "cSlowRateManager.h"
#include <AnimationMapManagerImplement.h>
#include <WinUser.h>
#include <shared.h>
#include <Hw.h>
#include "d3dx9.h"
#include "cObj.h"
#include <BehaviorEmBase.h>
#include <EmBaseDLC.h>
#include <iostream>
#include <XInput.h>
#include <format>
#include "IniReader.h"
#include "json.hpp"
#include <vector>
#include <filesystem>
#include <algorithm>
#include <Shlwapi.h>
using json = nlohmann::json;
namespace fs = std::filesystem;
 

struct EventsStruct { // Events Structure. Mirrors the "Events" object in the JSON file.
	std::vector<std::string> exitEvents;
	std::vector<std::string> enterEvents;
};

struct PartStruct { // Generic Part Structure. Mirrors the "Hair", "Sheath", and "Head" objects in the JSON file.
	bool toggleInRipper = false;
	bool hideInNormal = false;
	bool hideInRipper = false;
};

struct VisorStruct : PartStruct { // Visor Structure. Mirrors the "Visor" object in the JSON file.
	bool visorEnabledInRipper = false;
	bool visorEnabledInNormal = false;
};

struct BodyStruct { // Body Structure. Mirrors the "Body" object in the JSON file.
	std::vector<int> targetBody;

	bool toggleInRipper = true;

	float resizeFactor = 1.f;
	bool resetSize = false;
	float resetSizeRate = 0.01f;

	bool showVisorAtArmstrong = false;

	PartStruct Hair;
	PartStruct Sheath;
	VisorStruct Visor;
	PartStruct Head;

	EventsStruct Events;
};

std::vector<BodyStruct> Body;



int bodyCount;
int bodyJsonIndex;
float resizeFactorEase = 1.f;
bool hasEnteredQTE = false;
bool ripperSwitch = false;
bool hasInitialized = false;
int i;
int num;
bool check = false;
int bodyModelIndex;
int blademodetype;
int remainingUpdates;

int** pCurrentCostume;
bool debugMode;
int currentPhase = 1;
bool hairExists = false;
bool sheathExists = false;
bool visorExists = false;
bool headExists = false;
int callCount = 0;

namespace Wwise {
	int MGR_PlaySound(const char* se, int a2)
	{
		return ((int(__cdecl*)(const char* se, int a2))(shared::base + 0xA5E050))(se, a2);
	} //SE Events

	static int __cdecl PlaySound(const char* se, void* a2, int a3, int a4)
	{
		return ((int(_cdecl*)(const char*, void* a2, int a3, int a4))(shared::base + 0xA5E0C0))(se, a2, a3, a4);
	} // SE ATTR Events

	void __cdecl Se_PlayEvent(const char* event)
	{
		((void(__cdecl*)(const char*))(shared::base + 0xA5E1B0))(event);
	} //BGM Events
}


//inline void initializePartByIndex(json jsonFile, std::string part, int index) { // why did i do this?? :sob:
//
//	std::string bodyIndex = "Body" + std::to_string(index);
//
//	if (jsonFile[bodyIndex].contains(part)) {
//		if (jsonFile[bodyIndex][part].is_object()) {
//			if (part == "Hair") {
//				if (jsonFile[bodyIndex]["Hair"].contains("ToggleInRipper")) {
//					if (jsonFile[bodyIndex]["Hair"]["ToggleInRipper"].is_boolean()) {
//						Body[index].Hair.toggleInRipper = jsonFile[bodyIndex]["Hair"]["ToggleInRipper"];
//					}
//				}
//				if (jsonFile[bodyIndex]["Hair"].contains("HideInNormal")) {
//					if (jsonFile[bodyIndex]["Hair"]["HideInNormal"].is_boolean()) {
//						Body[index].Hair.hideInNormal = jsonFile[bodyIndex]["Hair"]["HideInNormal"];
//					}
//				}
//				if (jsonFile[bodyIndex]["Hair"].contains("HideInRipper")) {
//					if (jsonFile[bodyIndex]["Hair"]["HideInRipper"].is_boolean()) {
//						Body[index].Hair.hideInRipper = jsonFile[bodyIndex]["Hair"]["HideInRipper"];
//					}
//				}
//			}
//			else if (part == "Sheath") {
//				if (jsonFile[bodyIndex]["Sheath"].contains("ToggleInRipper")) {
//					if (jsonFile[bodyIndex]["Sheath"]["ToggleInRipper"].is_boolean()) {
//						Body[index].Sheath.toggleInRipper = jsonFile[bodyIndex]["Sheath"]["ToggleInRipper"];
//					}
//				}
//				if (jsonFile[bodyIndex]["Sheath"].contains("HideInNormal")) {
//					if (jsonFile[bodyIndex]["Sheath"]["HideInNormal"].is_boolean()) {
//						Body[index].Sheath.hideInNormal = jsonFile[bodyIndex]["Sheath"]["HideInNormal"];
//					}
//				}
//				if (jsonFile[bodyIndex]["Sheath"].contains("HideInRipper")) {
//					if (jsonFile[bodyIndex]["Sheath"]["HideInRipper"].is_boolean()) {
//						Body[index].Sheath.hideInRipper = jsonFile[bodyIndex]["Sheath"]["HideInRipper"];
//					}
//				}
//			}
//			else if (part == "Visor") {
//				if (jsonFile[bodyIndex]["Visor"].contains("ToggleInRipper")) {
//					if (jsonFile[bodyIndex]["Visor"]["ToggleInRipper"].is_boolean()) {
//						Body[index].Visor.toggleInRipper = jsonFile[bodyIndex]["Visor"]["ToggleInRipper"];
//					}
//				}
//				if (jsonFile[bodyIndex]["Visor"].contains("HideInNormal")) {
//					if (jsonFile[bodyIndex]["Visor"]["HideInNormal"].is_boolean()) {
//						Body[index].Visor.hideInNormal = jsonFile[bodyIndex]["Visor"]["HideInNormal"];
//					}
//				}
//				if (jsonFile[bodyIndex]["Visor"].contains("HideInRipper")) {
//					if (jsonFile[bodyIndex]["Visor"]["HideInRipper"].is_boolean()) {
//						Body[index].Visor.hideInRipper = jsonFile[bodyIndex]["Visor"]["HideInRipper"];
//					}
//				}
//
//				if (jsonFile[bodyIndex]["Visor"].contains("VisorForceEnabledInRipper")) {
//					if (jsonFile[bodyIndex]["Visor"]["VisorForceEnabledInRipper"].is_boolean()) {
//						Body[index].Visor.visorEnabledInRipper = jsonFile[bodyIndex]["Visor"]["VisorForceEnabledInRipper"];
//					}
//				}
//				if (jsonFile[bodyIndex]["Visor"].contains("VisorForceEnabledInNormal")) {
//					if (jsonFile[bodyIndex]["Visor"]["VisorForceEnabledInNormal"].is_boolean()) {
//						Body[index].Visor.visorEnabledInNormal = jsonFile[bodyIndex]["Visor"]["VisorForceEnabledInNormal"];
//					}
//				}
//			}
//			else if (part == "Head") {
//				if (jsonFile[bodyIndex]["Head"].contains("ToggleInRipper")) {
//					if (jsonFile[bodyIndex]["Head"]["ToggleInRipper"].is_boolean()) {
//						Body[index].Head.toggleInRipper = jsonFile[bodyIndex]["Head"]["ToggleInRipper"];
//					}
//				}
//				if (jsonFile[bodyIndex]["Head"].contains("HideInNormal")) {
//					if (jsonFile[bodyIndex]["Head"]["HideInNormal"].is_boolean()) {
//						Body[index].Head.hideInNormal = jsonFile[bodyIndex]["Head"]["HideInNormal"];
//					}
//				}
//				if (jsonFile[bodyIndex]["Head"].contains("HideInRipper")) {
//					if (jsonFile[bodyIndex]["Head"]["HideInRipper"].is_boolean()) {
//						Body[index].Head.hideInRipper = jsonFile[bodyIndex]["Head"]["HideInRipper"];
//					}
//				}
//			}
//		}
//	}
//}


bool fileExists(const std::string& filename) {
	std::ifstream file(filename);
	return file.good();
}


void dummy() {};


inline void updateBodyPart(Behavior* BehaviorPart, PartStruct StructPart, bool inRipperMode) {

	if (inRipperMode) {
		if (StructPart.hideInRipper) {
			BehaviorPart->disableRender();
		}
		else {
			BehaviorPart->enableRender();
			if (StructPart.toggleInRipper && !StructPart.hideInRipper) {
				BehaviorPart->toggleAnyMesh("normal", !inRipperMode);
				BehaviorPart->toggleAnyMesh("ripper", inRipperMode);
			}
		}
	}
	else {
		if (StructPart.hideInNormal) {
			BehaviorPart->disableRender();
		}
		else {
			BehaviorPart->enableRender();
			if (StructPart.toggleInRipper && !StructPart.hideInRipper) {
				BehaviorPart->toggleAnyMesh("normal", !inRipperMode);
				BehaviorPart->toggleAnyMesh("ripper", inRipperMode);
			}
		}                                   
	}
}


inline void updateBody() {

	Pl0000* player = cGameUIManager::Instance.m_pPlayer;

	if (player) {

		bool inRipperMode = player->m_nRipperModeEnabled;

		Behavior* bHair = nullptr; 
		Behavior* bSheath = nullptr; 
		Behavior* bVisor = nullptr; 
		Behavior* bHead = nullptr;

		if (Entity* entity = player->m_HairHandle.getEntity(); entity) {
			bHair = (Behavior*)entity->m_pInstance;
		}

		if (Entity* entity = player->m_SheathHandle.getEntity(); entity) {
			bSheath = (Behavior*)entity->m_pInstance;
		}

		if (Entity* entity = player->m_HelmetHandle.getEntity(); entity) {
			bVisor = (Behavior*)entity->m_pInstance;
		}

		if (Entity* entity = player->m_FaceHandle.getEntity(); entity) {
			bHead = (Behavior*)entity->m_pInstance;
		}

		// make this toggleAnyMesh code more clamplicated, gotta make dt for weapons 

		if (Body[bodyJsonIndex].toggleInRipper) {
			player->toggleAnyMesh("normal", !inRipperMode);
			player->toggleAnyMesh("ripper", inRipperMode);
		}


		if (bHair) {
			updateBodyPart(bHair, Body[bodyJsonIndex].Hair, inRipperMode);
			hairExists = true;
		}

		if (bSheath) {
			updateBodyPart(bSheath, Body[bodyJsonIndex].Sheath, inRipperMode);
			sheathExists = true;
		}

		if (bVisor) {
			visorExists = true;
			if ((currentPhase == 0x750) || (currentPhase == 0x740) || (currentPhase == 0x730)) {
				if (Body[bodyJsonIndex].showVisorAtArmstrong) {
					updateBodyPart(bVisor, Body[bodyJsonIndex].Visor, inRipperMode);
				}
				else {
					bVisor->disableRender();
				}
			}
			else {
				updateBodyPart(bVisor, Body[bodyJsonIndex].Visor, inRipperMode);
			}

			if (!inRipperMode) {
				Body[bodyJsonIndex].Visor.visorEnabledInNormal ? Trigger::GameFlags.GAME_PLAYER_VISOR_ENABLED = 1
					: Trigger::GameFlags.GAME_PLAYER_VISOR_ENABLED = 0;
			}
			else {
				Body[bodyJsonIndex].Visor.visorEnabledInRipper ? Trigger::GameFlags.GAME_PLAYER_VISOR_ENABLED = 1
					: Trigger::GameFlags.GAME_PLAYER_VISOR_ENABLED = 0;
			}
		}

		if (bHead) {
			updateBodyPart(bHead, Body[bodyJsonIndex].Head, inRipperMode);
			headExists = true;
		}
	}
}


inline void callEvents(std::vector<std::string>& eventList) {
	for (std::string event : eventList) {
		Wwise::MGR_PlaySound(event.c_str(), 0);
		Wwise::PlaySound(event.c_str(), 0, -1, 0);
		Wwise::Se_PlayEvent(event.c_str());
		callCount += 1;
	} 
}


void mainInit() {

	// frouk sent this code so if i ever forget hopefully this comment will remind me // i kinda started understanding it!!!

	char jsonPath[MAX_PATH];
	HMODULE hm = NULL;
	GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCSTR)&dummy, &hm);
	GetModuleFileNameA(hm, jsonPath, sizeof(jsonPath));

	//char* ptr = strrchr(jsonPath, '\\'); // remove the executeable name from the path // by finding & of last '\' in path 

	//if (ptr) {
	//	*ptr = '\0'; // truncate the path at the last '\' character	(ASI filename removed from path, CWD achieved)
	//}

	PathRemoveExtensionA(jsonPath);

	strcat(jsonPath, ".json");

	std::fstream jsonStream;
	json jsonFile;

	if (fileExists(jsonPath)) {
		jsonStream.open(jsonPath);
	}

	if (jsonStream) {

		try {
			jsonFile = json::parse(jsonStream, nullptr, true);
		}
		catch (const nlohmann::json::exception& e) {
			std::ofstream logFile("RMT_Error.txt");
			if (logFile.is_open()) {
				logFile << "JSON Parse Error: " << e.what() << std::endl;
				logFile << "Exception ID: " << e.id << std::endl;
				logFile.close();
			}
		}

		int indexB = 0;
		int indexA = 0;

		if (jsonFile.contains("DebugMode")) {
			if (jsonFile["DebugMode"].is_boolean()) {
				debugMode = jsonFile["DebugMode"];
			}
		}

		for (indexA = 0; indexA < (jsonFile.size()); ++indexA) {

			std::string bodyPlusIndexA = "Body" + std::to_string(indexA);

			if (jsonFile.contains(bodyPlusIndexA)) {

				Body.resize(Body.size() + 1);

				if (jsonFile[bodyPlusIndexA].contains("CostumeIndex")) {
					if (jsonFile[bodyPlusIndexA]["CostumeIndex"].is_array()) {
						for (int i = 0; i < (jsonFile[bodyPlusIndexA]["CostumeIndex"].size()); i++) {
							if (jsonFile[bodyPlusIndexA]["CostumeIndex"][i].is_number_integer()) {
								Body[indexA].targetBody.push_back(jsonFile[bodyPlusIndexA]["CostumeIndex"][i]);
							}
						}
					}
				}

				if (jsonFile[bodyPlusIndexA].contains("ToggleBodyInRipper")) {
					if (jsonFile[bodyPlusIndexA]["ToggleBodyInRipper"].is_boolean()) {
						Body[indexA].toggleInRipper = jsonFile[bodyPlusIndexA]["ToggleBodyInRipper"];
					}
				}

				if (jsonFile[bodyPlusIndexA].contains("RipperSize")) {
					if (jsonFile[bodyPlusIndexA]["RipperSize"].is_number()) {
						Body[indexA].resizeFactor = jsonFile[bodyPlusIndexA]["RipperSize"];
					}
				}
				if (jsonFile[bodyPlusIndexA].contains("ResetSizeInQTE")) {
					if (jsonFile[bodyPlusIndexA]["ResetSizeInQTE"].is_boolean()) {
						Body[indexA].resetSize = jsonFile[bodyPlusIndexA]["ResetSizeInQTE"];
					}
				}
				if (jsonFile[bodyPlusIndexA].contains("ResetSizeRate")) {
					if (jsonFile[bodyPlusIndexA]["ResetSizeRate"].is_number()) {
						Body[indexA].resetSizeRate = jsonFile[bodyPlusIndexA]["ResetSizeRate"];
					}
				}

				if (jsonFile[bodyPlusIndexA].contains("ShowVisorAtArmstrong")) {
					if (jsonFile[bodyPlusIndexA]["ShowVisorAtArmstrong"].is_boolean()) {
						Body[indexA].showVisorAtArmstrong = jsonFile[bodyPlusIndexA]["ShowVisorAtArmstrong"];
					}
				}

				if (jsonFile[bodyPlusIndexA].contains("WwiseEvents")) {
					if (jsonFile[bodyPlusIndexA]["WwiseEvents"].is_object()) {
						if (jsonFile[bodyPlusIndexA]["WwiseEvents"].contains("OnRipperEnter")) {
							if (jsonFile[bodyPlusIndexA]["WwiseEvents"]["OnRipperEnter"].is_array()) {
								for (int i = 0; i < (jsonFile[bodyPlusIndexA]["WwiseEvents"]["OnRipperEnter"].size()); i++) {
									if (jsonFile[bodyPlusIndexA]["WwiseEvents"]["OnRipperEnter"][i].is_string()) {
										Body[indexA].Events.enterEvents.push_back(jsonFile[bodyPlusIndexA]["WwiseEvents"]["OnRipperEnter"][i]);
									}
								}
							}
						}
						if (jsonFile[bodyPlusIndexA]["WwiseEvents"].contains("OnRipperExit")) {
							if (jsonFile[bodyPlusIndexA]["WwiseEvents"]["OnRipperExit"].is_array()) {
								for (int i = 0; i < (jsonFile[bodyPlusIndexA]["WwiseEvents"]["OnRipperExit"].size()); i++) {
									if (jsonFile[bodyPlusIndexA]["WwiseEvents"]["OnRipperExit"][i].is_string()) {
										Body[indexA].Events.exitEvents.push_back(jsonFile[bodyPlusIndexA]["WwiseEvents"]["OnRipperExit"][i]);
									}
								}
							}
						}
					}
				}

				/*initializePartByIndex(jsonFile, "Hair", indexA);
				initializePartByIndex(jsonFile, "Sheath", indexA);
				initializePartByIndex(jsonFile, "Visor", indexA);
				initializePartByIndex(jsonFile, "Head", indexA);*/


				if (jsonFile[bodyPlusIndexA].contains("Hair")) {
					if (jsonFile[bodyPlusIndexA]["Hair"].is_object()) {
						if (jsonFile[bodyPlusIndexA]["Hair"].contains("ToggleInRipper")) {
							if (jsonFile[bodyPlusIndexA]["Hair"]["ToggleInRipper"].is_boolean()) {
								Body[indexA].Hair.toggleInRipper = jsonFile[bodyPlusIndexA]["Hair"]["ToggleInRipper"];
							}
						}
						if (jsonFile[bodyPlusIndexA]["Hair"].contains("HideInNormal")) {
							if (jsonFile[bodyPlusIndexA]["Hair"]["HideInNormal"].is_boolean()) {
								Body[indexA].Hair.hideInNormal = jsonFile[bodyPlusIndexA]["Hair"]["HideInNormal"];
							}
						}
						if (jsonFile[bodyPlusIndexA]["Hair"].contains("HideInRipper")) {
							if (jsonFile[bodyPlusIndexA]["Hair"]["HideInRipper"].is_boolean()) {
								Body[indexA].Hair.hideInRipper = jsonFile[bodyPlusIndexA]["Hair"]["HideInRipper"];
							}
						}
					}
				}

				if (jsonFile[bodyPlusIndexA].contains("Sheath")) {
					if (jsonFile[bodyPlusIndexA]["Sheath"].is_object()) {
						if (jsonFile[bodyPlusIndexA]["Sheath"].contains("ToggleInRipper")) {
							if (jsonFile[bodyPlusIndexA]["Sheath"]["ToggleInRipper"].is_boolean()) {
								Body[indexA].Sheath.toggleInRipper = jsonFile[bodyPlusIndexA]["Sheath"]["ToggleInRipper"];
							}
						}
						if (jsonFile[bodyPlusIndexA]["Sheath"].contains("HideInNormal")) {
							if (jsonFile[bodyPlusIndexA]["Sheath"]["HideInNormal"].is_boolean()) {
								Body[indexA].Sheath.hideInNormal = jsonFile[bodyPlusIndexA]["Sheath"]["HideInNormal"];
							}
						}
						if (jsonFile[bodyPlusIndexA]["Sheath"].contains("HideInRipper")) {
							if (jsonFile[bodyPlusIndexA]["Sheath"]["HideInRipper"].is_boolean()) {
								Body[indexA].Sheath.hideInRipper = jsonFile[bodyPlusIndexA]["Sheath"]["HideInRipper"];
							}
						}
					}
				}

				if (jsonFile[bodyPlusIndexA].contains("Visor")) {
					if (jsonFile[bodyPlusIndexA]["Visor"].is_object()) {
						if (jsonFile[bodyPlusIndexA]["Visor"].contains("ToggleInRipper")) {
							if (jsonFile[bodyPlusIndexA]["Visor"]["ToggleInRipper"].is_boolean()) {
								Body[indexA].Visor.toggleInRipper = jsonFile[bodyPlusIndexA]["Visor"]["ToggleInRipper"];
							}
						}
						if (jsonFile[bodyPlusIndexA]["Visor"].contains("HideInNormal")) {
							if (jsonFile[bodyPlusIndexA]["Visor"]["HideInNormal"].is_boolean()) {
								Body[indexA].Visor.hideInNormal = jsonFile[bodyPlusIndexA]["Visor"]["HideInNormal"];
							}
						}
						if (jsonFile[bodyPlusIndexA]["Visor"].contains("HideInRipper")) {
							if (jsonFile[bodyPlusIndexA]["Visor"]["HideInRipper"].is_boolean()) {
								Body[indexA].Visor.hideInRipper = jsonFile[bodyPlusIndexA]["Visor"]["HideInRipper"];
							}
						}

						if (jsonFile[bodyPlusIndexA]["Visor"].contains("VisorForceEnabledInRipper")) {
							if (jsonFile[bodyPlusIndexA]["Visor"]["VisorForceEnabledInRipper"].is_boolean()) {
								Body[indexA].Visor.visorEnabledInRipper = jsonFile[bodyPlusIndexA]["Visor"]["VisorForceEnabledInRipper"];
							}
						}
						if (jsonFile[bodyPlusIndexA]["Visor"].contains("VisorForceEnabledInNormal")) {
							if (jsonFile[bodyPlusIndexA]["Visor"]["VisorForceEnabledInNormal"].is_boolean()) {
								Body[indexA].Visor.visorEnabledInNormal = jsonFile[bodyPlusIndexA]["Visor"]["VisorForceEnabledInNormal"];
							}
						}
					}
				}

				if (jsonFile[bodyPlusIndexA].contains("Head")) {
					if (jsonFile[bodyPlusIndexA]["Head"].is_object()) {
						if (jsonFile[bodyPlusIndexA]["Head"].contains("ToggleInRipper")) {
							if (jsonFile[bodyPlusIndexA]["Head"]["ToggleInRipper"].is_boolean()) {
								Body[indexA].Head.toggleInRipper = jsonFile[bodyPlusIndexA]["Head"]["ToggleInRipper"];
							}
						}
						if (jsonFile[bodyPlusIndexA]["Head"].contains("HideInNormal")) {
							if (jsonFile[bodyPlusIndexA]["Head"]["HideInNormal"].is_boolean()) {
								Body[indexA].Head.hideInNormal = jsonFile[bodyPlusIndexA]["Head"]["HideInNormal"];
							}
						}
						if (jsonFile[bodyPlusIndexA]["Head"].contains("HideInRipper")) {
							if (jsonFile[bodyPlusIndexA]["Head"]["HideInRipper"].is_boolean()) {
								Body[indexA].Head.hideInRipper = jsonFile[bodyPlusIndexA]["Head"]["HideInRipper"];
							}
						}
					}
				}
				


				/*for (indexB = 0; indexB < jsonFile[bodyPlusIndexA]["MainWeaponIndex"].size(); ++indexB) {
					MainWeaponIndex[indexA][indexB] = jsonFile[bodyPlusIndexA]["MainWeaponIndex"][indexB];
				}

				for (indexB = 0; indexB < jsonFile[bodyPlusIndexA]["UniqueWeaponIndex"].size(); ++indexB) {
					UniqueWeaponIndex[indexA][indexB] = jsonFile[bodyPlusIndexA]["UniqueWeaponIndex"][indexB];
				}*/
			}
		}

		jsonStream.close();
	}
}



void ripperInit() {

	Pl0000* player = cGameUIManager::Instance.m_pPlayer;
	
	int*& gScenarioManagerImplement = *(int**)(shared::base + 0x17E9A30);

	if (gScenarioManagerImplement && gScenarioManagerImplement[45]) {

		currentPhase = *(int*)(gScenarioManagerImplement[45] + 4);
	};


	if (player) {

		Behavior* Visor = nullptr;

		if (Entity* entity = player->m_HelmetHandle.getEntity(); entity) {
			Visor = (Behavior*)entity->m_pInstance;
		}

		int*& currentPlSkin = *(int**)(shared::base + 0x17EA01C);

		bodyJsonIndex = -1;

		bool indexFound = false;

		for (int bodyIndex = 0; bodyIndex < 12 && !indexFound; ++bodyIndex) {
			for (auto it = Body[bodyIndex].targetBody.rbegin(); it != Body[bodyIndex].targetBody.rend(); ++it) {
				if (*it == *currentPlSkin) {
					bodyJsonIndex = bodyIndex;
					indexFound = true;
				}
			}
		}

		if (bodyJsonIndex > -1) {

			updateBody();

			player->setSize({ 1.0f, 1.0f, 1.0f, 1.0f });

			ripperSwitch = false;

			hasInitialized = true;

			if ((currentPhase == 0x750) || (currentPhase == 0x740) || (currentPhase == 0x730)) {
				remainingUpdates = 5;
			}
		}
	}
}


void ripperTick() {

	Pl0000* player = cGameUIManager::Instance.m_pPlayer;


	if (player) {

		int*& currentPlSkin = *(int**)(shared::base + 0x17EA01C);
		pCurrentCostume = &currentPlSkin;

		blademodetype = player->m_nBladeModeType;
		bodyModelIndex = player->m_nModelIndex;

		if (hasInitialized) {

			if (bodyJsonIndex > -1) {

				if (player->m_nRipperModeEnabled && !ripperSwitch) {

					ripperSwitch = true;

					updateBody();

					callEvents(Body[bodyJsonIndex].Events.enterEvents);

					player->setSize({ Body[bodyJsonIndex].resizeFactor, Body[bodyJsonIndex].resizeFactor, Body[bodyJsonIndex].resizeFactor, 1.0f});
				}
				else if (!player->m_nRipperModeEnabled && ripperSwitch) {

						ripperSwitch = false;

						updateBody();

						callEvents(Body[bodyJsonIndex].Events.exitEvents);

						player->setSize({ 1.0f, 1.0f, 1.0f, 1.0f });

						hasEnteredQTE = false;
				}

				if (Body[bodyJsonIndex].resetSize) {
					if (Trigger::StaFlags.STA_QTE || player->m_nBladeModeType == 8) {
						player->setSize({ 1.0f, 1.0f, 1.0f, 1.0f });
						resizeFactorEase = 1.f;
						hasEnteredQTE = true;
					}
					else {
						if (player->m_nRipperModeEnabled && hasEnteredQTE) {
							if (Body[bodyJsonIndex].resizeFactor > 1.0f) {
								if ((resizeFactorEase + Body[bodyJsonIndex].resetSizeRate) < Body[bodyJsonIndex].resizeFactor) {
									resizeFactorEase += Body[bodyJsonIndex].resetSizeRate;
								}
								else {
									resizeFactorEase = Body[bodyJsonIndex].resizeFactor;
								}
							}
							else {
								if ((resizeFactorEase - Body[bodyJsonIndex].resetSizeRate) > Body[bodyJsonIndex].resizeFactor) {
									resizeFactorEase -= Body[bodyJsonIndex].resetSizeRate;
								}
								else {
									resizeFactorEase = Body[bodyJsonIndex].resizeFactor;
								}
							}

							player->setSize({ resizeFactorEase, resizeFactorEase, resizeFactorEase, 1.0f });
						}
					}
				}

				if (remainingUpdates > 0) {
					updateBody();
					remainingUpdates -= 1;
				}
			}
		}
		else {
			ripperInit();
		}
	}
}



void ripperReinit() {
	hasInitialized = false;
}

