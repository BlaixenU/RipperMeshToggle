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

// ini allocations

int targetBody;
float resizeFactor;

bool resetSize;
float resetSizeRate = 0.01f;

bool IncludeHair;
bool IncludeSheath;
bool IncludeVisor;
bool IncludeHead;
bool IncludeMainWeapon;
bool IncludeUniqueWeapon;

bool HideHair;
bool HideSheath;
bool HideVisor;
bool HideHead;

int MainWeaponCount;
int MainWeaponIndex[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

int UniqueWeaponCount;
int UniqueWeaponIndex[3] = { 0, 0, 0 };

// not ini allocations

float resizeFactorEase = 1.f;
bool hasEnteredQTE = false;
bool ripperSwitch = false;
bool hasInitialized = false;
int i;
int num;
bool check = false;
int bodyModelIndex;
bool resizeUp;


static void updateBodyPart(Behavior* Part, bool IncludePart, bool HidePart, bool inRipperMode) {

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

static void updateBody() {

	Pl0000* player = cGameUIManager::Instance.m_pPlayer;

	if (player)
	{
		bool inRipperMode = player->m_nRipperModeEnabled;

		Behavior* Hair = (Behavior*)player->m_HairHandle.getEntity()->m_pInstance;
		Behavior* Sheath = (Behavior*)player->m_SheathHandle.getEntity()->m_pInstance;
		Behavior* Visor = (Behavior*)player->m_HelmetHandle.getEntity()->m_pInstance;
		Behavior* Head = (Behavior*)player->m_FaceHandle.getEntity()->m_pInstance;

		// make this toggleAnyMesh code more clamplicated, gotta make dt for weapons 

		player->toggleAnyMesh("normal", !inRipperMode);
		player->toggleAnyMesh("ripper", inRipperMode);


		updateBodyPart(Hair, IncludeHair, HideHair, inRipperMode);
		updateBodyPart(Sheath, IncludeSheath, HideSheath, inRipperMode);
		updateBodyPart(Visor, IncludeVisor, HideVisor, inRipperMode);
		updateBodyPart(Head, IncludeHead, HideHead, inRipperMode);
	}
}


void mainInit() {


	CIniReader ini("RipperMeshToggle.ini");
	targetBody = ini.ReadInteger("Main", "ModelIndex", 65552);
	resizeFactor = ini.ReadFloat("Main", "RipperSize", 1.0f);
	resetSize = ini.ReadBoolean("Main", "ResetSizeInQTE", true);

	IncludeHair = ini.ReadBoolean("Include", "Hair", true);
	IncludeSheath = ini.ReadBoolean("Include", "Sheath", false);
	IncludeVisor = ini.ReadBoolean("Include", "Visor", true);
	IncludeHead = ini.ReadBoolean("Include", "Head", true);
	IncludeMainWeapon = ini.ReadBoolean("Include", "MainWeapon", true);
	IncludeUniqueWeapon = ini.ReadBoolean("Include", "UniqueWeapon", true);

	HideHair = ini.ReadBoolean("Hide", "Hair", true);
	HideSheath = ini.ReadBoolean("Hide", "Sheath", false);
	HideVisor = ini.ReadBoolean("Hide", "Visor", true);
	HideHead = ini.ReadBoolean("Hide", "Head", true);

	if (resizeFactor >= 1.f) {
		resizeUp = true;
	}
	else {
		resizeUp = false;
	}

	if (IncludeMainWeapon)
	{
		MainWeaponCount = ini.ReadInteger("MainWeapon", "Count", 0);

		if (MainWeaponCount == 7) {

			MainWeaponIndex[0] = 0x11012; // hf blade
			MainWeaponIndex[1] = 0x13000; // armourbreaker
			MainWeaponIndex[2] = 0x13001; // stun blade
			MainWeaponIndex[3] = 0x13002; // hf longsword
			MainWeaponIndex[4] = 0x13003; // hf wooden sword
			MainWeaponIndex[5] = 0x13004; // hf machete
			MainWeaponIndex[6] = 0x13005; // hf murasama
			MainWeaponIndex[7] = 0x11301; // fox blade
		}
		else {
			for (i = 0; i < MainWeaponCount; i++) {
				MainWeaponIndex[i] = ini.ReadInteger("MainWeapon", ("Index" + std::to_string(i)), 0);
			}

		}
	}

	if (IncludeUniqueWeapon)
	{
		UniqueWeaponCount = ini.ReadInteger("UniqueWeapon", "Count", 0);

		if (UniqueWeaponCount == 3) {
			UniqueWeaponIndex[0] = 0x32000;
			UniqueWeaponIndex[1] = 0x32020;
			UniqueWeaponIndex[2] = 0x32030;
		}
		else {
			for (i = 0; i < UniqueWeaponCount; i++) {
				UniqueWeaponIndex[i] = ini.ReadInteger("UniqueWeapon", ("Index" + std::to_string(i)), 0);
			}
		}
	}
}


void ripperInit() {

	Pl0000* player = cGameUIManager::Instance.m_pPlayer;

	if (player && (player->m_nModelIndex == targetBody)) {
		
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

		Behavior* Hair = (Behavior*)player->m_HairHandle.getEntity()->m_pInstance;
		Behavior* Sheath = (Behavior*)player->m_SheathHandle.getEntity()->m_pInstance;
		Behavior* Visor = (Behavior*)player->m_HelmetHandle.getEntity()->m_pInstance;
		Behavior* Head = (Behavior*)player->m_FaceHandle.getEntity()->m_pInstance;

		Behavior* MainWeapon = (Behavior*)player->m_SwordHandle.getEntity()->m_pInstance;
		Behavior* UniqueWeapon = (Behavior*)player->field_FF8.getEntity()->m_pInstance;
		// replace field_FF8 with m_CustomWeaponHandle once SDK updates
	}


	if (hasInitialized) {

		if (player && (player->m_nModelIndex == targetBody)) {

			if (player->m_nRipperModeEnabled && !ripperSwitch) {

				ripperSwitch = true;

				updateBody();

				player->setSize({ resizeFactor, resizeFactor, resizeFactor, 1.0f });
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

							if ((resizeFactorEase + 0.01f) < resizeFactor) {
								resizeFactorEase += 0.01f;
							}
							else {
								resizeFactorEase = resizeFactor;
							}

							player->setSize({ resizeFactorEase, resizeFactorEase, resizeFactorEase, 1.0f });
						}
					}
					else {
						if (player->m_nRipperModeEnabled && hasEnteredQTE) {

							if ((resizeFactorEase - 0.01f) > resizeFactor) {
								resizeFactorEase -= 0.01f;
							}
							else {
								resizeFactorEase = resizeFactor;
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

