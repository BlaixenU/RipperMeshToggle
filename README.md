ASI mod for Metal Gear Rising: Revengeance that enables modders to create dynamically appearing parts of their player mods depending on if the player is in Ripper mode or not. Not only that, but it comes with more functionality like player size, bodypart (sheath, visor, hair, etc) settings, and also multiple body support.

> [!IMPORTANT]
> If used in a mod, **do NOT rename the ASI file**, and be sure to **link this repository in any release** of the mod, public or not.

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
To actually configure the ASI for your mod, head on over to `RipperMeshToggle.json` and give it a look. It's preferred to learn very surface level JSON syntax, since you will have to create as many `Body` objects as there are costumes in your mod that need to be affected. In the sample JSON file, there are two objects, `"Body0"` and `"Body1"`. Both of these have the same keys with varying values. You can delete the second one if you only want to mod one, either for the size of your mod or for the sake of experimentation (recommended).  The most notable key is the `"ModelIndex"` key. This is the key whose value corresponds to the body that will be affected. If you know how to calculate the Decimal ID of a body, just put that in this field. If not, then run the game with the ASI mod with `"DebugMode"` set to true and press L to open the debug menu. The "Current Costume ID" value will be the value of the player's current body model index. Alternatively, take the filename of your body (for example, `pl1030`), replace the '`pl`' with 1 (so now it's `11030`), and then plug this value through a *Hexadecimal to Decimal converter* (which will return a big value like, in the case of `pl1030`, `69648`).

> [!WARNING]
> Using the default Custom Body (`69648`) runs the risk of it not behaving properly, since the Monsoon and Armstrong bossfights dont use PL1010, but rather use PL1090.

Aside from that, there are the `Include` and `Hide` arrays. The simplest way to understand these is to think of which parts of the player need to be affected in any way at all. If you were to keep the sheath the same regardless of if the player is in Ripper mode or not, then keep the value of `Sheath` inside `Include` false. However, if you want a part to be *hidden* during Ripper mode, then set it as true inside `Include` and true inside `Hide`. Finally, if you want to make the part also have it's own ripper mesh, then you can include it but not hide it.

> [!TIP]
> If you want to make it so a part is hidden when not in Ripper mode but appears when in Ripper mode, you can have it included but not hidden, and then inside the model, only contain meshes of the `ripper` meshgroup.

There are also Includes for `MainWeapon` and `UniqueWeapon`, as well as the arrays for them. As of yet, this functionality has not been added, so it's advised to keep both as false.

### Extras
There are also keys named `RipperSize` and `ResetSizeInQTE`, as well as `ResetSizeRate`. When in Ripper mode, the ASI will scale the player according to the value in `RipperSize`, which has a base value of `1.000` (100%), and if `ResetSizeInQTE` is true, then the size of the player will be reset inside any QuickTimeEvents. The reason you would want your characters to have their size reset is because alot of QTEs are utterly borked when the player is of a different scale and this is a fine compromise between consistency and functionality. `ResetSizeRate` has a very small value which refers to the increments that the ASI takes per-update while resizing the player back to its `RipperSize` once a QTE ends. A greater `ResetSizeRate` entails less time to reset back to `RipperSize`, and vice versa.

