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
#include <Windows.h>
#include <WinUser.h>
#include <shared.h>
#include <Hw.h>
#include "d3dx9.h"
#include "cWeaponSelectMenu.h"
#include "cObj.h"
#include <BehaviorEmBase.h>
#include <EmBaseDLC.h>
#include <iostream>
#include <sstream>
#include <XInput.h>
#include <format>
#include <Entity.h>
#include "IniReader.h"
#include "json.hpp"
#include <algorithm>
#include <vector>
using json = nlohmann::json;

// json allocations

int targetBody[12];
float resizeFactor[12];

bool resetSize[12];
float resetSizeRate[12];

bool IncludeHair[12];
bool IncludeSheath[12];
bool IncludeVisor[12];
bool IncludeHead[12];
bool IncludeMainWeapon[12];
bool IncludeUniqueWeapon[12];

bool HideHair[12];
bool HideSheath[12];
bool HideVisor[12];
bool HideHead[12];

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

template <typename type> bool inArray(type array[], int size, type target) {

	for (auto index = 0; index < size; index++) {
		if (array[index] == target) {
			return true;
		}
	}
	return false;
}

inline void updateBodyPart(Behavior* Part, bool IncludePart, bool HidePart, bool inRipperMode) {

	if (IncludePart) {
		if (HidePart) {
			if (inRipperMode) {
				Part->disableRender();
			}
			else {
				Part->enableRender();
			}
		}
		else {
			Part->toggleAnyMesh("normal", !inRipperMode);
			Part->toggleAnyMesh("ripper", inRipperMode);
		}
	}
}

inline void updateBody() {

	Pl0000* player = cGameUIManager::Instance.m_pPlayer;

	if (player)
	{
		bool inRipperMode = player->m_nRipperModeEnabled;

		Behavior* Hair = nullptr; 
		Behavior* Sheath = nullptr; 
		Behavior* Visor = nullptr; 
		Behavior* Head = nullptr;

		if (Entity* entity = player->m_HairHandle.getEntity(); entity)
			Hair = (Behavior*)entity->m_pInstance;

		if (Entity* entity = player->m_SheathHandle.getEntity(); entity)
			Sheath = (Behavior*)entity->m_pInstance;

		if (Entity* entity = player->m_HelmetHandle.getEntity(); entity)
			Visor = (Behavior*)entity->m_pInstance;

		if (Entity* entity = player->m_FaceHandle.getEntity(); entity)
			Head = (Behavior*)entity->m_pInstance;

		// make this toggleAnyMesh code more clamplicated, gotta make dt for weapons 

		player->toggleAnyMesh("normal", !inRipperMode);
		player->toggleAnyMesh("ripper", inRipperMode);


		if (Hair) {
			updateBodyPart(Hair, IncludeHair, HideHair, inRipperMode);
		}

		if (Sheath) {
			updateBodyPart(Sheath, IncludeSheath, HideSheath, inRipperMode);
		}

		if (Visor) {
			updateBodyPart(Visor, IncludeVisor, HideVisor, inRipperMode);
		}

		if (Head) {
			updateBodyPart(Head, IncludeHead, HideHead, inRipperMode);
		}
	}
}


void mainInit() {

	std::ifstream jsonStream("RipperMeshToggle.json");
	json jsonFile = json::parse(jsonStream);

	for (int indexA = 0; indexA < jsonFile.size(); ++indexA) {
		bodyCount++;

		targetBody[indexA] = jsonFile["Body" + indexA]["ModelIndex"];
		resizeFactor[indexA] = jsonFile["Body" + indexA]["RipperSize"];

		resetSize[indexA] = jsonFile["Body" + indexA]["ResetSizeInQTE"];

		IncludeHair[indexA] = jsonFile["Body" + indexA]["Include"]["Hair"];
		IncludeSheath[indexA] = jsonFile["Body" + indexA]["Include"]["Sheath"];
		IncludeVisor[indexA] = jsonFile["Body" + indexA]["Include"]["Visor"];
		IncludeHead[indexA] = jsonFile["Body" + indexA]["Include"]["Head"];
		IncludeMainWeapon[indexA] = jsonFile["Body" + indexA]["Include"]["MainWeapon"];
		IncludeUniqueWeapon[indexA] = jsonFile["Body" + indexA]["Include"]["UniqueWeapon"];

		HideHair[indexA] = jsonFile["Body" + indexA]["Hide"]["Hair"];
		HideSheath[indexA] = jsonFile["Body" + indexA]["Hide"]["Sheath"];
		HideVisor[indexA] = jsonFile["Body" + indexA]["Hide"]["Visor"];
		HideHead[indexA] = jsonFile["Body" + indexA]["Hide"]["Head"];

		for (auto indexB = 0; indexB != jsonFile["Body" + indexA]["MainWeaponIndex"].size(); ++indexB) {
			MainWeaponIndex[indexA][indexB] = jsonFile["Body" + indexA]["MainWeaponIndex"][indexB];
		}

		for (auto indexB = 0; indexB != jsonFile["Body" + indexA]["UniqueWeaponIndex"].size(); ++indexB) {
			UniqueWeaponIndex[indexA][indexB] = jsonFile["Body" + indexA]["UniqueWeaponIndex"][indexB];
		}

	}
}


void ripperInit() {

	Pl0000* player = cGameUIManager::Instance.m_pPlayer;

	if (player && inArray<int>(targetBody, 12, player->m_nModelIndex)) {
		
		for (auto index = 0; index < 12; index++) {
			if (player->m_nModelIndex == targetBody[index]) {
				bodyJsonIndex = index;
				break;
			}
		}

		bodyModelIndex = player->m_nModelIndex;

		updateBody();

		player->setSize({ 1.0f, 1.0f, 1.0f, 1.0f });

		ripperSwitch = false;

		hasInitialized = true;
	}
}


void ripperTick() {

	Pl0000* player = cGameUIManager::Instance.m_pPlayer;

	if (player) {

		Behavior* Hair = nullptr;
		Behavior* Sheath = nullptr;
		Behavior* Visor = nullptr;
		Behavior* Head = nullptr;
		Behavior* MainWeapon = nullptr;
		Behavior* UniqueWeapon = nullptr;

		if (Entity* entity = player->m_HairHandle.getEntity(); entity)
			Hair = (Behavior*)entity->m_pInstance;

		if (Entity* entity = player->m_SheathHandle.getEntity(); entity)
			Sheath = (Behavior*)entity->m_pInstance;

		if (Entity* entity = player->m_HelmetHandle.getEntity(); entity)
			Visor = (Behavior*)entity->m_pInstance;

		if (Entity* entity = player->m_FaceHandle.getEntity(); entity)
			Head = (Behavior*)entity->m_pInstance;

		if (Entity* entity = player->m_SwordHandle.getEntity(); entity)
			MainWeapon = (Behavior*)entity->m_pInstance;

		if (Entity* entity = player->field_FF8.getEntity(); entity)
			UniqueWeapon = (Behavior*)entity->m_pInstance;

		// replace field_FF8 with m_CustomWeaponHandle once SDK updates
	}


	if (hasInitialized) {

		if (player && (player->m_nModelIndex == targetBody[bodyJsonIndex])) {

			if (player->m_nRipperModeEnabled && !ripperSwitch) {

				ripperSwitch = true;

				updateBody();

				player->setSize({ resizeFactor[bodyJsonIndex], resizeFactor[bodyJsonIndex], resizeFactor[bodyJsonIndex], 1.0f});
			}
			else {

				if (!player->m_nRipperModeEnabled && ripperSwitch) {

					ripperSwitch = false;

					updateBody();

					player->setSize({ 1.0f, 1.0f, 1.0f, 1.0f });

					hasEnteredQTE = false;
				}
			}


			if (resetSize) {
				if (Trigger::StaFlags.STA_QTE) {
					player->setSize({ 1.0f, 1.0f, 1.0f, 1.0f });
					resizeFactorEase = 1.f;
					hasEnteredQTE = true;
				}
				else {

					if (resizeUp) {
						if (player->m_nRipperModeEnabled && hasEnteredQTE) {

							if ((resizeFactorEase + 0.01f) < resizeFactor[bodyJsonIndex]) {
								resizeFactorEase += 0.01f;
							}
							else {
								resizeFactorEase = resizeFactor[bodyJsonIndex];
							}

							player->setSize({ resizeFactorEase, resizeFactorEase, resizeFactorEase, 1.0f });
						}
					}
					else {
						if (player->m_nRipperModeEnabled && hasEnteredQTE) {

							if ((resizeFactorEase - 0.01f) > resizeFactor[bodyJsonIndex]) {
								resizeFactorEase -= 0.01f;
							}
							else {
								resizeFactorEase = resizeFactor[bodyJsonIndex];
							}

							player->setSize({ resizeFactorEase, resizeFactorEase, resizeFactorEase, 1.0f });
						}

					}
				}
			}
		}
	}
	else {
		ripperInit();
	}
}



void ripperReinit() {
	hasInitialized = false;
}

