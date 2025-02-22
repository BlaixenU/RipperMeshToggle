#pragma once
#include <string>

extern void mainInit();
extern void ripperInit();
extern void ripperTick();
extern void ripperReinit();

extern struct Part {
	bool toggleInRipper = false;
	bool hideInNormal = false;
	bool hideInRipper = false;
};
extern Part Hair[12], Sheath[12], Visor[12], Head[12];

extern int targetBody[12];
extern float resizeFactor[12];
extern bool visorBypass[12];

extern bool resetSize[12];
extern float resetSizeRate[12];

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