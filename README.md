
ASI mod for Metal Gear Rising: Revengeance that enables modders to create dynamically appearing parts of their player mods depending on if the player is in Ripper mode or not. Not only that, but it comes with more functionality like player size, bodypart (sheath, visor, hair, etc) settings, and also multiple body support.

> [!IMPORTANT]
> If used in a mod, **be sure to rename the ASI and JSON files to a unique name (predictably your mod's name) and keep the name the same for both files (MyMod.asi, MyMod.json, etc.)**, and be sure to **link this repository in any release and credit me (BlaixenU)** of the mod, public or not. For public mods in particular, please also link my Nexus Mods profile (https://next.nexusmods.com/profile/BlaixenU)

# Table Of Contents
- Installation
- Setting Up Model
- Setting Up JSON
  - General Structure
  - Listing Target Bodies (CostumeIndex)
  - Body Parts
  - RipperSize
  - WwiseEvents
  - Extra Fields

# Usage

## Installation

Head over to the Releases section of this repository and download the latest version. Download both `RipperMeshToggle.asi` and `RipperMeshToggle.json`.

Install these files for your mod the same way you'd install an ASI mod with an ini file, i.e place in the scripts folder of `GameData` or place alongside the contents in a Rising Mod Manager mod directory and mention `RipperMeshToggle.asi` in the `Dll File:` field of the mod's settings.

## Setting Up (Model)

This part is relatively simple. Any meshes inside your model which are of the `normal` meshgroup (`0-normal-X`) will be hidden during Ripper mode and visible outside of Ripper mode, and any meshes inside your model which are of the `ripper` meshgroup (`0-ripper-X`) wil be hidden outside of Ripper mode and visible during Ripper mode, in other words, the `ripper` meshgroup is exclusive to Ripper mode, and the `normal` meshgroupis exclusive to normal mode.

The reason I didn't start with the simpler definition is because it's possible to have extra meshgroups which aren't affected by the logic of the ASI for whatever purpose the modder may want. 

Since this means that both the Ripper and normal model will have to be the same model, give extra care to the texture filenames and to the inclusion of the extra textures inside the WTA/WTP. 

## Setting Up (JSON)

### Essentials
To actually configure the ASI for your mod, head on over to `RipperMeshToggle.json` and give it a look. It's preferred to learn very surface level JSON syntax, since you will have to create as many `Body` objects as there are costumes in your mod that need to be affected. In the JSON file provided alongside the ASI file in the latest release, there will be an object named `"Body0"` after a `"DebugMode"` key. For each costume you want RipperMeshToggle to apply it's logic to, you make a Body object suffixed with a number for it's order (so if there are three costumes in total, then there will be a `"Body0"`, `"Body1"`, and `"Body3"`.  The most notable key is the `"CostumeIndex"` key. This is the key whose value corresponds to the body that will be affected. If you know how to calculate the Decimal ID of a body, just put that in this field. If not, then run the game with the ASI mod with `"DebugMode"` set to true and press L to open the debug menu. The "Current Costume ID" value will be the value of the player's current body model index. Alternatively, take the filename of your body (for example, `pl1030`), replace the '`pl`' with 1 (so now it's `11030`), and then plug this value through a *Hexadecimal to Decimal converter* (which will return a big value like, in the case of `pl1030`, `69648`).

> [!WARNING]
> Using the default Custom Body (`69648`) runs the risk of it not behaving properly, since the Monsoon and Armstrong bossfights dont use PL1010, but rather use PL1090.

Aside from that, you have JSON objects for all extra bodyparts (Hair, Visor, Sheath, and Hair) which contain fields which control their behavior in and out of Ripper mode. In general, they have three fields: `"ToggleInRipper"`, `"HideInNormal"`, and `"HideInRipper"`. `"ToggleInRipper"`, if set to true, sets the bodypart to have its meshes toggled just like the main body if the main body's ``

> [!TIP]
> You can exclude parts of a JSON Body object and the ASI will fallback to default values, this is useful to keep the JSON readable and short in the case that you only use it for simple purposes like to force the visor to render in the Armstrong fight or to perma-hide bodyparts. Do note that this only applies to every key EXCEPT `"CostumeImdex"`, which WILL crash the game if not found in an otherwise valid Body object.

### Ripper Size
There are also keys named `RipperSize` and `ResetSizeInQTE`, as well as `ResetSizeRate`. When in Ripper mode, the ASI will scale the player according to the value in `RipperSize`, which has a base value of `1.000` (100%), and if `ResetSizeInQTE` is true, then the size of the player will be reset inside any QuickTimeEvents. The reason you would want your characters to have their size reset is because alot of QTEs are utterly borked when the player is of a different scale and this is a fine compromise between consistency and functionality. `ResetSizeRate` has a very small value which refers to the increments that the ASI takes per-update while resizing the player back to its `RipperSize` once a QTE ends. A greater `ResetSizeRate` entails less time to reset back to `RipperSize`, and vice versa.

### Wwise Events
In the JSON, there will be an object named `"WwiseEvents"` which contains the two arrays `"OnRipperEnter"` and `"OnRipperExit"`. For as many BGM, SE, or SE ATTR events that you want to be called when *entering* Ripper Mode, add them as string-type elements in the corresponding array, and same thing for *exitting* Ripper Mode.

It's tricky and I personally haven't found success in making a custom BNK for this, but it is a great idea to add *core.bnk* events in these arrays as that soundbank rarely, if not, never remains unloaded and it contains a bunch of general sound effects.
