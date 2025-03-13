#pragma once
#include <string>

extern void mainInit();
extern void ripperInit();
extern void ripperTick();
extern void ripperReinit();

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
};

extern EventsStruct Events[12];

struct Part {
	bool toggleInRipper = false;
	bool hideInNormal = false;
	bool hideInRipper = false;
};

struct VisorPart : Part {
	bool visorEnabledInRipper = false;
	bool visorEnabledInNormal = false;
};

extern BodyStruct Body[12];

extern Part Hair[12];
extern Part Sheath[12];
extern VisorPart Visor[12];
extern Part Head[12];

extern int MainWeaponIndex[8][12];

extern int UniqueWeaponIndex[3][12];


extern int bodyCount;
extern int bodyJsonIndex;
extern float resizeFactorEase;
extern bool hasEnteredQTE;
extern bool ripperSwitch;
extern bool hasInitialized;
extern int bodyModelIndex;
extern int blademodetype;
extern int remainingUpdates;

extern int** pCurrentCostume;
extern bool debugMode;
extern int currentPhase;
extern bool hairExists;
extern bool sheathExists;
extern bool visorExists;
extern bool headExists;
extern int callCount;