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
float resetSizeRate = 1.f / 60.f;

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
int UniqueWeaponIndex[3] = { 0, 0, 0 }; // we have any non-default weapons???

// not ini allocations

float resizeFactorEase = 1.f;
bool hasEnteredQTE = false;
bool ripperSwitch = false;
bool hasInitialized = false;
// int i; ///< Uhm, this is a little bit awkward
int num;
bool check = false;
int bodyModelIndex;
// bool resizeUp;

void updateBodyPart(Behavior* Part, bool IncludePart, bool HidePart, bool inRipperMode) 
{
	if (IncludePart) 
	{
		if (HidePart) 
		{
			if (inRipperMode)
				Part->disableRender();
			else
				Part->enableRender();
		}
		else 
		{
			Part->toggleAnyMesh("normal", !inRipperMode);
			Part->toggleAnyMesh("ripper", inRipperMode);
		}
	}
}

void updateBody() 
{
	Pl0000* player = cGameUIManager::Instance.m_pPlayer;

	if (player)
	{
		bool inRipperMode = player->m_nRipperModeEnabled;

		// Behavior* Hair = (Behavior*)player->m_HairHandle.getEntity()->m_pInstance;
		// Behavior* Sheath = (Behavior*)player->m_SheathHandle.getEntity()->m_pInstance;
		// Behavior* Visor = (Behavior*)player->m_HelmetHandle.getEntity()->m_pInstance;
		// Behavior* Head = (Behavior*)player->m_FaceHandle.getEntity()->m_pInstance;

		// Rather than using function to get the instance, we'll make it faster by casting the field into the appropriate typename

		Behavior *Hair = nullptr, Sheath = nullptr, Visor = nullptr, Head = nullptr;

		if (Entity *entity = player->m_HairHandle.getEntity(); entity)
			Hair = (Behavior*)entity->m_pInstance;

		if (Entity *entity = player->m_SheathHandle.getEntity(); entity)
			Sheath = (Behavior*)entity->m_pInstance;

		if (Entity *entity = player->m_HelmetHandle.getEntity(); entity)
			Visor = (Behavior*)entity->m_pInstance;

		if (Entity *entity = player->m_FaceHandle.getEntity(); entity)
			Head = (Behavior*)entity->m_pInstance;

		// make this toggleAnyMesh code more clamplicated, gotta make dt for weapons 

		player->toggleAnyMesh("normal", !inRipperMode);
		player->toggleAnyMesh("ripper", inRipperMode);

		// We're not sure if we have any of these
		if (Hair)
			updateBodyPart(Hair, IncludeHair, HideHair, inRipperMode);

		if (Sheath)
			updateBodyPart(Sheath, IncludeSheath, HideSheath, inRipperMode);

		if (Visor)
			updateBodyPart(Visor, IncludeVisor, HideVisor, inRipperMode);

		if (Head)
			updateBodyPart(Head, IncludeHead, HideHead, inRipperMode);
	}
}


void mainInit()
{
	CIniReader ini("RipperMeshToggle.ini");
	targetBody = ini.ReadInteger("Main", "ModelIndex", 0x10010); // you're allowed to insert hex, they'll still convert to int whatever you do
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

	/*
	if (resizeFactor >= 1.f) 
	{
		resizeUp = true;
	}
	else 
	{
		resizeUp = false;
	}
	*/

	if (IncludeMainWeapon)
	{
		MainWeaponCount = ini.ReadInteger("MainWeapon", "Count", 0);

		if (MainWeaponCount == 7) 
		{

			MainWeaponIndex[0] = 0x11012; // hf blade
			MainWeaponIndex[1] = 0x13000; // armourbreaker
			MainWeaponIndex[2] = 0x13001; // stun blade
			MainWeaponIndex[3] = 0x13002; // hf longsword
			MainWeaponIndex[4] = 0x13003; // hf wooden sword
			MainWeaponIndex[5] = 0x13004; // hf machete
			MainWeaponIndex[6] = 0x13005; // hf murasama
			MainWeaponIndex[7] = 0x11301; // fox blade
		}
		else 
		{
			for (int i = 0; i < MainWeaponCount; i++) 
			{
				MainWeaponIndex[i] = ini.ReadInteger("MainWeapon", ("Index" + std::to_string(i)), 0);
			}

		}
	}

	if (IncludeUniqueWeapon)
	{
		UniqueWeaponCount = ini.ReadInteger("UniqueWeapon", "Count", 0);

		if (UniqueWeaponCount == 3) 
		{
			UniqueWeaponIndex[0] = 0x32000;
			UniqueWeaponIndex[1] = 0x32020;
			UniqueWeaponIndex[2] = 0x32030;
		}
		else 
		{
			for (int i = 0; i < UniqueWeaponCount; i++) 
			{
				UniqueWeaponIndex[i] = ini.ReadInteger("UniqueWeapon", ("Index" + std::to_string(i)), 0);
			}
		}
	}
}


void ripperInit() 
{
	Pl0000* player = cGameUIManager::Instance.m_pPlayer;

	if (player && player->m_nModelIndex == targetBody) 
	{
		bodyModelIndex = player->m_nModelIndex;

		updateBody();

		player->setSize({ 1.0f, 1.0f, 1.0f, 1.0f });

		ripperSwitch = false;

		hasInitialized = true;
	}
}

/// Should've used hooking to change meshes

constexpr float sizeDelta = 1.0f / 60.0f;

void ripperTick() 
{
	if (!hasInitialized)
		ripperInit(); // Although why initialize in the update function

	Pl0000* player = cGameUIManager::Instance.m_pPlayer;

	if (!player)
		return;

	Behavior* Hair = (Behavior*)player->m_HairHandle.getEntity()->m_pInstance;
	Behavior* Sheath = (Behavior*)player->m_SheathHandle.getEntity()->m_pInstance;
	Behavior* Visor = (Behavior*)player->m_HelmetHandle.getEntity()->m_pInstance;
	Behavior* Head = (Behavior*)player->m_FaceHandle.getEntity()->m_pInstance;

	Behavior* MainWeapon = (Behavior*)player->m_SwordHandle.getEntity()->m_pInstance;
	Behavior* UniqueWeapon = (Behavior*)player->field_FF8.getEntity()->m_pInstance;
	// replace field_FF8 with m_CustomWeaponHandle once SDK updates


	if (hasInitialized) 
	{
		if (player && player->m_nModelIndex == targetBody) 
		{
			if (player->m_nRipperModeEnabled && !ripperSwitch) 
			{
				ripperSwitch = true;

				updateBody();

				player->setSize({ resizeFactor, resizeFactor, resizeFactor, 1.0f });
			}
			else 
			{
				if (!player->m_nRipperModeEnabled && ripperSwitch) 
				{

					ripperSwitch = false;

					updateBody();

					player->setSize({ 1.0f, 1.0f, 1.0f, 1.0f });

					hasEnteredQTE = false;
				}
			}


			if (resetSize) 
			{
				if (Trigger::StaFlags.STA_QTE) 
				{
					player->setSize({ 1.0f, 1.0f, 1.0f, 1.0f });
					resizeFactorEase = 1.f;
					hasEnteredQTE = true;
				}
				else 
				{
					/*
					// Why would you check if it sizes up or not??
					if (resizeUp) 
					{
						if (player->m_nRipperModeEnabled && hasEnteredQTE) 
						{
							/// What is this??
							if ((resizeFactorEase + 0.01f) < resizeFactor) 
							{
								resizeFactorEase += 0.01f;
							}
							else 
							{
								resizeFactorEase = resizeFactor;
							} 

							player->setSize({ resizeFactorEase, resizeFactorEase, resizeFactorEase, 1.0f });
						}
					}
					else 
					{
						if (player->m_nRipperModeEnabled && hasEnteredQTE) 
						{
							if ((resizeFactorEase - 0.01f) > resizeFactor) 
							{
								resizeFactorEase -= 0.01f;
							}
							else 
							{
								resizeFactorEase = resizeFactor;
							}

							player->setSize({ resizeFactorEase, resizeFactorEase, resizeFactorEase, 1.0f });
						}

					}
					*/
					if (player->m_nRipperModeEnabled && hasEnteredQTE)
					{
						if (resizeFactor >= 1.f)
							resizeFactorEase = min(resizeFactorEase + sizeDelta, resizeFactor)
						else
							resizeFactorEase = max(resizeFactorEase - sizeDelta, resizeFactor);

						player->m_vecSize = {resizeFactorEase, resizeFactorEase, resizeFactorEase, 1.0f};
						player->m_pEntity->m_pAnimation->m_fAnimationSize = resizeFactorEase;
					}
				}
			}
		}
	}
	else 
	{
		ripperInit();
	}
}

void ripperReinit() 
{
	hasInitialized = false;
}