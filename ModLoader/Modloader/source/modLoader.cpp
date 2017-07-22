/*
 *	modLoader.cpp
*/

#include "stdafx.h"
#include <LinkerAPI.h>
#include <DXAPI\ScriptObject.h>
#include <ModLoader\ModLoader.h>

//! A vector of mod callables.
static std::vector<ModLoader::ModLoaderCallables*> sModCallables;

void serverProcessReplacement(unsigned int timeDelta) 
{
	unsigned int servertickaddr=0x602350;
	unsigned int serverthisptr=0x9E5EC0;

	// Call the server process hook for all eligible mods
	for (auto it = sModCallables.begin(); it != sModCallables.end(); it++)
	{
		ModLoader::ModLoaderCallables* currentCallables = *it;
		if (currentCallables->mServerProcessPointer != NULL)
			currentCallables->mServerProcessPointer(timeDelta);
	}

	Con::errorf(0, "Bla -bla blallaaa");

	__asm 
	{
		mov ecx,serverthisptr
		push timeDelta
		call servertickaddr
	}
	
	return;
}

// Mod Loader Implementation
bool conUnloadLoadMod(Linker::SimObject *obj,S32 argc, const char* argv[])
{
	const char* targetName = argv[1];

	for (auto it = sModCallables.begin(); it != sModCallables.end(); it++)
	{
		ModLoader::ModLoaderCallables* currentMod = *it;
		if (strcmp(currentMod->mGetManagementName(), targetName) == 0)
		{
			// Deinitialize the mod and remove it from the list.
			if (currentMod->mDeinitializeModPointer != NULL)
				currentMod->mDeinitializeModPointer();
			sModCallables.erase(it);
			return true;
		}
	}

	// Something weird happened.
	Con::errorf(0, "Failed to unload mod: '%s'.", targetName);
	return false;
}

bool conLoadMod(Linker::SimObject *obj,S32 argc, const char* argv[])
{
	HINSTANCE hDLL = NULL;

	std::string raw = "mods\\";
	raw += argv[1];
	raw += ".dll";

	std::wstring modification(raw.begin(), raw.end());

	hDLL = LoadLibrary(modification.c_str());
	if (hDLL == NULL)
	{
		Con::errorf(0, "loadMod(): Failed to load DLL '%s'. Does it exist in GameData\\mods? (%u)", raw.c_str(), GetLastError());
		return false; // The DLL doesn't exist
	}

	// Attempt to load supported loader information first
	ModLoader::GetModLoaderVersionPointer getModLoaderVersion = (ModLoader::GetModLoaderVersionPointer)GetProcAddress(hDLL, "getModLoaderVersion");
	if (getModLoaderVersion == NULL)
	{
		Con::errorf(0, "loadMod(): Failed to locate entry point 'getModLoaderVersion' in mod DLL '%s'. Is it a good mod DLL? (%u)", raw.c_str(), GetLastError());
		return false; // Unable to load entry point
	}

	// Check if there's a server process responder in this DLL
	ModLoader::GetModCallablesPointer getModCallables = (ModLoader::GetModCallablesPointer)GetProcAddress(hDLL, "getModCallables");
	if (getModCallables == NULL)
	{
		Con::errorf(0, "loadMod(): Failed to locate entry point 'getModCallables' in mod DLL '%s'. Is it a good mod DLL? (%u)", raw.c_str(), GetLastError());
		return false; // Unable to load entry point
	}

	unsigned int modLoaderVersion = getModLoaderVersion();
	ModLoader::ModLoaderCallables* callables = getModCallables();

	// Management name function must be provided
	if (callables->mGetManagementName == NULL)
	{
		Con::errorf(0, "loadMod(): Loaded mod did not provide a management name. This is required.", raw.c_str(), GetLastError());
		return false; // Unable to load entry point
	}
	const char* managementName = callables->mGetManagementName();

	// Is the mod already loaded?
	for (auto it = sModCallables.begin(); it != sModCallables.end(); it++)
	{
		ModLoader::ModLoaderCallables* currentMod = *it;
		if (strcmp(currentMod->mGetManagementName(), managementName) == 0)
		{
			Con::errorf(0, "loadMod(): The mod is already loaded.", raw.c_str(), GetLastError());
			return false;
		}
	}

	// FIXME: We should probably only run this once at init
	Con::addMethodB(NULL, "unloadMod", &conUnloadLoadMod, "Unloads a C++ modification by name.", 2, 2);

	if (callables->mInitializeModPointer != NULL)
		callables->mInitializeModPointer();

	sModCallables.push_back(callables);
	return true;
}