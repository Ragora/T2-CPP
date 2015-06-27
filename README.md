T2CPP
=====

Mod Loading system and mods for Tribes 2 written in C++.


Credit Attribution
====

| Name                      | Reasons                                                                               |
| ------------------------- | ------------------------------------------------------------------------------------- |
| Robert MacGregor          | Main Coding                                                                           |
| Calvin Balke              | Found Projectile::explode                                                             |
| Linker                    | [Original T2DLL Code](http://www.the-construct.net/forums/showthread.php?t=500)       |
| LouCypher                 | Reverse engineered a IFC22.dll replacement he made to build some [template code](https://github.com/Ragora/T2-IFC22Template)        |
| Jason R. Blevins          | [Mersenne Twister Implementation](Mod Sources/RandomMod/RandomMod/source/mt.cc)       |

What is it?
====

Tribes 2 C++ (T2CPP) provides a C++ API on top of the existing API left by [Linker](http://www.the-construct.net/forums/showthread.php?t=500&highlight=Linker).
This is all provided from a custom built replacement IFC22.dll replacement for the game which was reverse engineered from the [IFC22.dll implementation](http://rdb.internectual.net/t2) of [TribesNext](http://www.tribesnext.com) by LouCypher.

The template coding for my IFC22.dll can be found [here](https://github.com/Ragora/T2-IFC22Template|here) and the project page for T2CPP itself is [here](https://github.com/Ragora/T2-CPP|here).

In this Repository
====

The T2CPP project also contains sources for three different mods, which are listed below.

|                                       Mod Name                                                     |
|----------------------------------------------------------------------------------------------------|
|       [Template Mod](https://github.com/Ragora/T2-CPP/tree/master/Mod%20Sources/BaseMod)           |
|       [TSExtension](https://github.com/Ragora/T2-CPP/tree/master/Mod%20Sources/TSExtension)        |
|       [Random Mod](https://github.com/Ragora/T2-CPP/tree/master/Mod%20Sources/RandomMod)           |
|         [Watchdog](https://github.com/Ragora/T2-CPP/tree/master/Mod%20Sources/Watchdog)            |

Usage
====

Essentially, you just need to compile a Release version of the [ModLoader DLL](ModLoader) and replace Tribes 2's IFC22.dll with the new DLL you built. To use the DLL, you must make sure you have Force Feedback enabled (it works for dedicated servers too) otherwise the IFC22.dll replacement code is never executed. You can test if you did it correctly by checking for the existence of the Torque Script function "loadMod". Refer to the [Base Mod](Mod Sources/BaseMod) for C++ mod template and refer to both the [Linker and DX API's](Mod Sources/TSExtension/TSExtension/source) for how to manipulate Tribes 2.

Once everything is running, you must create a "mods" folder in the GameData folder of your Tribes 2 installation, this is where the compiled DLL mods you wish to run will go. If you install a mod called "RandomMod.dll" to the mods folder, you must stick a line of Torque Script somewhere (probably autoexec) that's always executed that reads:

```javascript
loadMod("RandomMod"); // Note the absense of the mods/ directory and the .dll on the end, the code handles that
```

#### loadMod(): Failed to load DLL '%s'. Does it exist in GameData\mods? (%u)


This simply means that loadMod() was unable to locate the desired DLL in the mods folder. Make sure it exists there.


#### loadMod(): Failed to locate entry point 'ModInitialize' in mod DLL '%s'. Is it a good mod DLL? (%u)


This error means that the DLL you specified probably isn't a proper mod DLL for this system. Either that or the DLL wasn't built correctly. The "void ModInitialize(void)" function **must** be built as extern C and should be defined with "__declspec(dllexport)" to work.

#### loadMod() is not found and I have the IFC22.dll replacement installed.

You probably don't have Force Feedback enabled. It can be enabled in Tribes 2's Settings under the "Game" tab or with the following preference:
$Pref::useImmersion = "1";
