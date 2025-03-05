#pragma once
#include <string>

extern void mainInit();
extern void ripperInit();
extern void ripperTick();
extern void ripperReinit();

struct EventsStruct {
	std::vector<std::string> exitEvents;
	std::vector<std::string> enterEvents = { "a", "b", "c" };
};

extern EventsStruct Events[12];

struct BodyStruct {
	int targetBody = 0;

	bool toggleInRipper = true;

	float resizeFactor = 1.f;
	bool resetSize = false;
	float resetSizeRate = 0.01f;

	bool showVisorAtArmstrong = true;
};

extern BodyStruct Body[12];

struct Part {
	bool toggleInRipper = false;
	bool hideInNormal = false;
	bool hideInRipper = false;
};

struct VisorPart : Part {
	bool visorEnabledInRipper = false;
	bool visorEnabledInNormal = false;
};

extern Part Hair[12], Sheath[12], Head[12];
extern VisorPart Visor[12];

extern int MainWeaponIndex[8][12];

extern int UniqueWeaponIndex[3][12];


extern bool ripperSwitch;
extern bool hasInitialized;
extern int i;
extern int num;
extern bool check;
extern int bodyModelIndex;
extern float resizeFactorEase;
extern int blademodetype;
extern int bodyJsonIndex;
extern int** pCurrentCostume;
extern bool debugMode;
extern int currentPhase;

extern bool hairExists;
extern bool sheathExists;
extern bool visorExists;
extern bool headExists;

extern const char* visorRenderLog;

extern int callCount;