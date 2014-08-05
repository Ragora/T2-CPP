/*
 *	modLoader.cpp
*/

#include "stdafx.h"
#include <t2api.h>

// Mod Loader Implementation
bool conLoadMod(SimObject *obj,S32 argc, const char* argv[])
{
		typedef void (*LPMODINIT)(void);
		HINSTANCE hDLL = NULL;
		LPMODINIT lpInitMod = NULL;

		std::string raw = "mods\\";
		raw += argv[1];
		raw += ".dll";

		std::wstring modification(raw.begin(), raw.end());

		hDLL = LoadLibrary(modification.c_str());
		if (hDLL == NULL)
		{
			Con::errorf(0, "loadMod(): Failed to load DLL '%s'. Does it exist in GameData\mods? (%u)", raw.c_str(), GetLastError());
			return false; // The DLL doesn't exist
		}
		
		lpInitMod = (LPMODINIT)GetProcAddress(hDLL, "ModInitialize"); // Attempt to load our entry point

		if (lpInitMod == NULL)
		{
			Con::errorf(0, "loadMod(): Failed to locate entry point 'ModInitialize' in mod DLL '%s'. Is it a good mod DLL? (%u)", raw.c_str(), GetLastError());
			return false; // Unable to load entry point
		}

		lpInitMod();
		Con::errorf(0, "loadMod(): Loaded and executed entry point code for mod DLL '%s'", raw.c_str());
		return true;
}