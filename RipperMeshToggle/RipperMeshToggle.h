#pragma once
#include <string>
#include <vector>

extern void mainInit();
extern void ripperInit();
extern void ripperTick();
extern void ripperReinit();

struct EventsStruct { // Events Structure. Mirrors the "Events" object in the JSON file.
	std::vector<std::string> exitEvents;
	std::vector<std::string> enterEvents;
};

struct PartStruct { // Generic Part Structure. Mirrors the "Hair", "Sheath", and "Head" objects in the JSON file.
	bool toggleInRipper;
	bool hideInNormal;
	bool hideInRipper;
};

struct VisorStruct : PartStruct { // Visor Structure. Mirrors the "Visor" object in the JSON file.
	bool visorEnabledInRipper;
	bool visorEnabledInNormal;
};

struct BodyStruct { // Body Structure. Mirrors the "Body" object in the JSON file.
	std::vector<int> targetBody;

	bool toggleInRipper;

	float resizeFactor;
	bool resetSize;
	float resetSizeRate;

	bool showVisorAtArmstrong;

	PartStruct Hair;
	PartStruct Sheath;
	VisorStruct Visor;
	PartStruct Head;

	EventsStruct Events;
};

extern std::vector<BodyStruct> Body;



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