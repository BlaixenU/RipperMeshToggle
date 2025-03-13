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
using json = nlohmann::json;
namespace fs = std::filesystem;
 
struct BodyStruct {
	std::vector<int> targetBody;

	bool toggleInRipper = true;

	float resizeFactor = 1.f;
	bool resetSize = false;
	float resetSizeRate = 0.01f;

	bool showVisorAtArmstrong = false;
};

struct EventsStruct {
	std::vector<std::string> exitEvents;
	std::vector<std::string> enterEvents;
} Events[12];

struct Part {
	bool toggleInRipper = false;
	bool hideInNormal = false;
	bool hideInRipper = false;
};

struct VisorPart : Part {
	bool visorEnabledInRipper = false;
	bool visorEnabledInNormal = false;
};

BodyStruct Body[12];

Part Hair[12];
Part Sheath[12];
VisorPart Visor[12];
Part Head[12];

int MainWeaponIndex[8][12];

int UniqueWeaponIndex[3][12];


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

inline void initializePartByIndex(json jsonFile, std::string part, int index) {

	std::string bodyIndex = "Body" + std::to_string(index);

	if (jsonFile[bodyIndex].contains(part)) {
		if (part == "Hair") {
			if (jsonFile[bodyIndex]["Hair"].contains("ToggleInRipper")) {
				Hair[index].toggleInRipper = jsonFile[bodyIndex]["Hair"]["ToggleInRipper"];
			}
			if (jsonFile[bodyIndex][part].contains("HideInNormal")) {
				Hair[index].hideInNormal = jsonFile[bodyIndex]["Hair"]["HideInNormal"];
			}
			if (jsonFile[bodyIndex][part].contains("HideInRipper")) {
				Hair[index].hideInRipper = jsonFile[bodyIndex]["Hair"]["HideInRipper"];
			}
		}
		else if (part == "Sheath") {
			if (jsonFile[bodyIndex]["Sheath"].contains("ToggleInRipper")) {
				Sheath[index].toggleInRipper = jsonFile[bodyIndex]["Sheath"]["ToggleInRipper"];
			}
			if (jsonFile[bodyIndex][part].contains("HideInNormal")) {
				Sheath[index].hideInNormal = jsonFile[bodyIndex]["Sheath"]["HideInNormal"];
			}
			if (jsonFile[bodyIndex][part].contains("HideInRipper")) {
				Sheath[index].hideInRipper = jsonFile[bodyIndex]["Sheath"]["HideInRipper"];
			}
		}
		else if (part == "Visor") {
			if (jsonFile[bodyIndex]["Visor"].contains("ToggleInRipper")) {
				Visor[index].toggleInRipper = jsonFile[bodyIndex]["Visor"]["ToggleInRipper"];
			}
			if (jsonFile[bodyIndex][part].contains("HideInNormal")) {
				Visor[index].hideInNormal = jsonFile[bodyIndex]["Visor"]["HideInNormal"];
			}
			if (jsonFile[bodyIndex][part].contains("HideInRipper")) {
				Visor[index].hideInRipper = jsonFile[bodyIndex]["Visor"]["HideInRipper"];
			}

			if (jsonFile[bodyIndex][part].contains("VisorForceEnabledInRipper")) {
				Visor[index].visorEnabledInRipper = jsonFile[bodyIndex]["Visor"]["VisorForceEnabledInRipper"];
			}
			if (jsonFile[bodyIndex][part].contains("VisorForceEnabledInNormal")) {
				Visor[index].visorEnabledInNormal = jsonFile[bodyIndex]["Visor"]["VisorForceEnabledInNormal"];
			}
		}
		else if (part == "Head") {
			if (jsonFile[bodyIndex]["Head"].contains("ToggleInRipper")) {
				Head[index].toggleInRipper = jsonFile[bodyIndex]["Head"]["ToggleInRipper"];
			}
			if (jsonFile[bodyIndex][part].contains("HideInNormal")) {
				Head[index].hideInNormal = jsonFile[bodyIndex]["Head"]["HideInNormal"];
			}
			if (jsonFile[bodyIndex][part].contains("HideInRipper")) {
				Head[index].hideInRipper = jsonFile[bodyIndex]["Head"]["HideInRipper"];
			}
		}
	}
}


bool fileExists(const std::string& filename) {
	std::ifstream file(filename);
	return file.good();
}


void dummy() {};


inline void updateBodyPart(Behavior* BehaviorPart, Part StructPart, bool inRipperMode) {

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
			updateBodyPart(bHair, Hair[bodyJsonIndex], inRipperMode);
			hairExists = true;
		}

		if (bSheath) {
			updateBodyPart(bSheath, Sheath[bodyJsonIndex], inRipperMode);
			sheathExists = true;
		}

		if (bVisor) {
			visorExists = true;
			if ((currentPhase == 0x750) || (currentPhase == 0x740) || (currentPhase == 0x730)) {
				if (Body[bodyJsonIndex].showVisorAtArmstrong) {
					updateBodyPart(bVisor, Visor[bodyJsonIndex], inRipperMode);
				}
				else {
					bVisor->disableRender();
				}
			}
			else {
				updateBodyPart(bVisor, Visor[bodyJsonIndex], inRipperMode);
			}

			if (!inRipperMode) {
				Visor[bodyJsonIndex].visorEnabledInNormal ? Trigger::GameFlags.GAME_PLAYER_VISOR_ENABLED = 1
					: Trigger::GameFlags.GAME_PLAYER_VISOR_ENABLED = 0;
			}
			else {
				Visor[bodyJsonIndex].visorEnabledInRipper ? Trigger::GameFlags.GAME_PLAYER_VISOR_ENABLED = 1
					: Trigger::GameFlags.GAME_PLAYER_VISOR_ENABLED = 0;
			}
		}

		if (bHead) {
			updateBodyPart(bHead, Head[bodyJsonIndex], inRipperMode);
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

	// frouk sent this code so if i ever forget hopefully this comment will remind me

	char cwd[MAX_PATH];
	HMODULE hm = NULL;
	GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCSTR)&dummy, &hm);
	GetModuleFileNameA(hm, cwd, sizeof(cwd));

	char* ptr = strrchr(cwd, '\\'); // remove the executeable name from the path

	if (ptr) {
		*ptr = '\0';
	}



	strcat(cwd, "\\RipperMeshToggle.json");

	std::fstream jsonStream;
	json jsonFile;

	if (fileExists(cwd)) {
		jsonStream.open(cwd);
	}

	if (jsonStream) {

		try {
			jsonFile = json::parse(jsonStream, nullptr, true);
		}
		catch (const nlohmann::json::exception& e) {
			std::ofstream logFile("Error.txt");
			if (logFile.is_open()) {
				logFile << "JSON Parse Error: " << e.what() << std::endl;
				logFile << "Exception ID: " << e.id << std::endl;
				logFile.close();
			}
		}

		int indexB = 0;
		int indexA = 0;

		debugMode = jsonFile["DebugMode"];

		for (indexA = 0; indexA < (jsonFile.size()); ++indexA) {

			std::string bodyPlusIndexA = "Body" + std::to_string(indexA);

			if (jsonFile.contains(bodyPlusIndexA)) {

				if (jsonFile[bodyPlusIndexA].contains("CostumeIndex")) {
					for (int i = 0; i < (jsonFile[bodyPlusIndexA]["CostumeIndex"].size()); i++) {
						Body[indexA].targetBody.push_back(jsonFile[bodyPlusIndexA]["CostumeIndex"][i]);
					}
				}

				if (jsonFile[bodyPlusIndexA].contains("ToggleBodyInRipper")) {
					Body[indexA].toggleInRipper = jsonFile[bodyPlusIndexA]["ToggleBodyInRipper"];
				}

				if (jsonFile[bodyPlusIndexA].contains("RipperSize")) {
					Body[indexA].resizeFactor = jsonFile[bodyPlusIndexA]["RipperSize"];
				}
				if (jsonFile[bodyPlusIndexA].contains("ResetSizeInQTE")) {
					Body[indexA].resetSize = jsonFile[bodyPlusIndexA]["ResetSizeInQTE"];
				}
				if (jsonFile[bodyPlusIndexA].contains("ResetSizeRate")) {
					Body[indexA].resetSizeRate = jsonFile[bodyPlusIndexA]["ResetSizeRate"];
				}

				if (jsonFile[bodyPlusIndexA].contains("ShowVisorAtArmstrong")) {
					Body[indexA].showVisorAtArmstrong = jsonFile[bodyPlusIndexA]["ShowVisorAtArmstrong"];
				}

				if (jsonFile[bodyPlusIndexA].contains("WwiseEvents")) {
					if (jsonFile[bodyPlusIndexA]["WwiseEvents"].contains("OnRipperEnter")) {
						for (int i = 0; i < (jsonFile[bodyPlusIndexA]["WwiseEvents"]["OnRipperEnter"].size()); i++) {
							Events[indexA].enterEvents.push_back(jsonFile[bodyPlusIndexA]["WwiseEvents"]["OnRipperEnter"][i]);
						}
					}
					if (jsonFile[bodyPlusIndexA]["WwiseEvents"].contains("OnRipperExit")) {
						for (int i = 0; i < (jsonFile[bodyPlusIndexA]["WwiseEvents"]["OnRipperExit"].size()); i++) {
							Events[indexA].exitEvents.push_back(jsonFile[bodyPlusIndexA]["WwiseEvents"]["OnRipperExit"][i]);
						}
					}
				}

				initializePartByIndex(jsonFile, "Hair", indexA);
				initializePartByIndex(jsonFile, "Sheath", indexA);
				initializePartByIndex(jsonFile, "Visor", indexA);
				initializePartByIndex(jsonFile, "Head", indexA);



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

					callEvents(Events[bodyJsonIndex].enterEvents);

					player->setSize({ Body[bodyJsonIndex].resizeFactor, Body[bodyJsonIndex].resizeFactor, Body[bodyJsonIndex].resizeFactor, 1.0f});
				}
				else if (!player->m_nRipperModeEnabled && ripperSwitch) {

						ripperSwitch = false;

						updateBody();

						callEvents(Events[bodyJsonIndex].exitEvents);

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

