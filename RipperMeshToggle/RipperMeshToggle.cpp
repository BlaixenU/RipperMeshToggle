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
 
struct Part {
	bool toggleInRipper = false;
	bool hideInNormal = false;
	bool hideInRipper = false;
};

// json allocations

Part Hair[12];
Part Sheath[12];
Part Visor[12];
Part Head[12];

int targetBody[12];
float resizeFactor[12];

bool resetSize[12];
float resetSizeRate[12];

bool visorBypass[12];

int MainWeaponIndex[8][12];

int UniqueWeaponIndex[3][12];


// not json allocations

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
bool resizeUp;
int blademodetype;
int** pCurrentCostume;
bool debugMode;
int currentPhase;

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

template <typename type> bool inArray(type array[], int size, type target) {

	for (auto index = 0; index < size; index++) {
		if (array[index] == target) {
			return true;
		}
	}
	return false;
}

inline void updateBodyPart(Behavior* BehaviorPart, Part StructPart, bool inRipperMode) {

	//if (IncludePart) {
	//	if (HidePart) {
	//		if (inRipperMode) {
	//			Part->disableRender();
	//		}
	//		else {
	//			Part->enableRender();
	//		}
	//	}
	//	else {
	//		Part->toggleAnyMesh("normal", !inRipperMode);
	//		Part->toggleAnyMesh("ripper", inRipperMode);
	//	}
	//}

	if (inRipperMode) {
		if (StructPart.hideInRipper) {
			BehaviorPart->disableRender();
		}
		else {
			BehaviorPart->enableRender();
		}
		
		if (StructPart.toggleInRipper && !StructPart.hideInRipper) {
			BehaviorPart->toggleAnyMesh("normal", !inRipperMode);
			BehaviorPart->toggleAnyMesh("ripper", inRipperMode);
		}
	}
	else {
		if (StructPart.hideInNormal) {
			BehaviorPart->disableRender();
		}
		else {
			BehaviorPart->enableRender();
		}
	}

}

inline void updateBody() {

	Pl0000* player = cGameUIManager::Instance.m_pPlayer;

	if (player)
	{
		bool inRipperMode = player->m_nRipperModeEnabled;

		Behavior* bHair = nullptr; 
		Behavior* bSheath = nullptr; 
		Behavior* bVisor = nullptr; 
		Behavior* bHead = nullptr;

		if (Entity* entity = player->m_HairHandle.getEntity(); entity)
			bHair = (Behavior*)entity->m_pInstance;

		if (Entity* entity = player->m_SheathHandle.getEntity(); entity)
			bSheath = (Behavior*)entity->m_pInstance;

		if (Entity* entity = player->m_HelmetHandle.getEntity(); entity)
			bVisor = (Behavior*)entity->m_pInstance;

		if (Entity* entity = player->m_FaceHandle.getEntity(); entity)
			bHead = (Behavior*)entity->m_pInstance;

		// make this toggleAnyMesh code more clamplicated, gotta make dt for weapons 

		player->toggleAnyMesh("normal", !inRipperMode);
		player->toggleAnyMesh("ripper", inRipperMode);


		if (Hair) {
			updateBodyPart(bHair, Hair[bodyJsonIndex], inRipperMode);
		}

		if (Sheath) {
			updateBodyPart(bSheath, Sheath[bodyJsonIndex], inRipperMode);
		}

		if (Visor) {
			updateBodyPart(bVisor, Visor[bodyJsonIndex], inRipperMode);
		}

		if (Head) {
			updateBodyPart(bHead, Head[bodyJsonIndex], inRipperMode);
		}
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

		for (indexA = 0; indexA < (jsonFile.size() - 1); ++indexA) { // add try-catch statement to this for value access error
			 
			std::string bodyPlusIndexA = "Body" + std::to_string(indexA);
			 
			targetBody[indexA] = jsonFile[bodyPlusIndexA]["ModelIndex"];
			// visorBypass[indexA] = jsonFile[bodyPlusIndexA]["ShowVisorAtArmstrong"];

			resizeFactor[indexA] = jsonFile[bodyPlusIndexA]["RipperSize"];
			resetSize[indexA] = jsonFile[bodyPlusIndexA]["ResetSizeInQTE"];
			resetSizeRate[indexA] = jsonFile[bodyPlusIndexA]["ResetSizeRate"];

			/*IncludeHair[indexA] = jsonFile[bodyPlusIndexA]["Include"]["Hair"];
			IncludeSheath[indexA] = jsonFile[bodyPlusIndexA]["Include"]["Sheath"];
			IncludeVisor[indexA] = jsonFile[bodyPlusIndexA]["Include"]["Visor"];
			IncludeHead[indexA] = jsonFile[bodyPlusIndexA]["Include"]["Head"];
			IncludeMainWeapon[indexA] = jsonFile[bodyPlusIndexA]["Include"]["MainWeapon"];
			IncludeUniqueWeapon[indexA] = jsonFile[bodyPlusIndexA]["Include"]["UniqueWeapon"];

			HideHair[indexA] = jsonFile[bodyPlusIndexA]["Hide"]["Hair"];
			HideSheath[indexA] = jsonFile[bodyPlusIndexA]["Hide"]["Sheath"];
			HideVisor[indexA] = jsonFile[bodyPlusIndexA]["Hide"]["Visor"];
			HideHead[indexA] = jsonFile[bodyPlusIndexA]["Hide"]["Head"];*/

			initializePartByIndex(jsonFile, "Hair", indexA);
			initializePartByIndex(jsonFile, "Sheath", indexA);
			initializePartByIndex(jsonFile, "Visor", indexA);
			initializePartByIndex(jsonFile, "Head", indexA);

			for (indexB = 0; indexB < jsonFile[bodyPlusIndexA]["MainWeaponIndex"].size(); ++indexB) {
				MainWeaponIndex[indexA][indexB] = jsonFile[bodyPlusIndexA]["MainWeaponIndex"][indexB];
			}

			for (indexB = 0; indexB < jsonFile[bodyPlusIndexA]["UniqueWeaponIndex"].size(); ++indexB) {
				UniqueWeaponIndex[indexA][indexB] = jsonFile[bodyPlusIndexA]["UniqueWeaponIndex"][indexB];
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

		for (int index = 0; index < 12; ++index) {
			if (targetBody[index] == *currentPlSkin) {
				bodyJsonIndex = index;
			}
		}

		if (inArray<int>(targetBody, 12, *currentPlSkin)) {

			updateBody();

			player->setSize({ 1.0f, 1.0f, 1.0f, 1.0f });

			ripperSwitch = false;

			hasInitialized = true;
		}
	}

}


void ripperTick() {

	Pl0000* player = cGameUIManager::Instance.m_pPlayer;

	if (player) {

		Behavior* Visor = nullptr;

		if (Entity* entity = player->m_HelmetHandle.getEntity(); entity) {
			Visor = (Behavior*)entity->m_pInstance;
		}

		int*& currentPlSkin = *(int**)(shared::base + 0x17EA01C);
		pCurrentCostume = &currentPlSkin;

		blademodetype = player->m_nBladeModeType;
		bodyModelIndex = player->m_nModelIndex;

		if (hasInitialized) {

			if (*currentPlSkin == targetBody[bodyJsonIndex]) {

				if (player->m_nRipperModeEnabled && !ripperSwitch) {

					ripperSwitch = true;

					updateBody();

					player->setSize({ resizeFactor[bodyJsonIndex], resizeFactor[bodyJsonIndex], resizeFactor[bodyJsonIndex], 1.0f });
				}
				else {

					if (!player->m_nRipperModeEnabled && ripperSwitch) {

						ripperSwitch = false;

						updateBody();

						player->setSize({ 1.0f, 1.0f, 1.0f, 1.0f });

						hasEnteredQTE = false;
					}
				}

				if (resetSize[bodyJsonIndex]) {
					if (Trigger::StaFlags.STA_QTE || player->m_nBladeModeType == 8) {
						player->setSize({ 1.0f, 1.0f, 1.0f, 1.0f });
						resizeFactorEase = 1.f;
						hasEnteredQTE = true;
					}
					else {
						if (player->m_nRipperModeEnabled && hasEnteredQTE) {
							if (resizeFactor[bodyJsonIndex] > 1.0f) {
								if ((resizeFactorEase + resetSizeRate[bodyJsonIndex]) < resizeFactor[bodyJsonIndex]) {
									resizeFactorEase += resetSizeRate[bodyJsonIndex];
								}
								else {
									resizeFactorEase = resizeFactor[bodyJsonIndex];
								}
							}
							else {
								if ((resizeFactorEase - resetSizeRate[bodyJsonIndex]) > resizeFactor[bodyJsonIndex]) {
									resizeFactorEase -= resetSizeRate[bodyJsonIndex];
								}
								else {
									resizeFactorEase = resizeFactor[bodyJsonIndex];
								}
							}

							player->setSize({ resizeFactorEase, resizeFactorEase, resizeFactorEase, 1.0f });
						}
					}
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

