#pragma once
#include <string>

extern void mainInit();
extern void ripperInit();
extern void ripperTick();
extern void ripperReinit();

extern int targetBody[12];
extern float resizeFactor[12];

extern bool resetSize[12];
extern float resetSizeRate[12];

extern bool IncludeHair[12];
extern bool IncludeSheath[12];
extern bool IncludeVisor[12];
extern bool IncludeHead[12];
extern bool IncludeMainWeapon[12];
extern bool IncludeUniqueWeapon[12];

extern bool HideHair[12];
extern bool HideSheath[12];
extern bool HideVisor[12];
extern bool HideHead[12];

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