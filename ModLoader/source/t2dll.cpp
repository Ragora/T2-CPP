/*
 *	t2dll.cpp
 *	Original code by Linker
 *	Modified by Robert MacGregor
*/

#include "stdafx.h"
#include "t2ConCmds.h"
#include "t2api.h"
#include <Windows.h>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    return true;
}

__declspec(dllexport) void initT2Dll(void)
{
		// Run Linker's old implementations ...
		Con::addMethodS("GuiTSCtrl","project",&conGuiTsCtrlProject,"projects a world space vector to on-screen position",3,3);
		Con::addMethodB("NetObject","SetGhostable",&conNetObjectSetGhostable,"set or unset object to be ghostable",3,3);
		Con::addMethodS(NULL,"getVariable",&conGetVariable,"Get a variable without restrictions",2,2);

		Con::addVariable("$cpuspeed",TypeS32,reinterpret_cast<void*>(0x8477F8)); //1 - S32, this is so i can set my cpu speed to 31337 or osmething =P
		Con::addVariable("$GameBase::showBoundingBox",TypeBool,reinterpret_cast<void*>(0x9ECF24));

		// Mod Loader Function
		Con::addMethodB(NULL, "loadMod", &conLoadMod, "Loads a C++ modification.",2,2);

		// Load the original TribesNext DLL if available
		typedef void (*LPINITT2DLL)(void);
		HINSTANCE hDLL = NULL;
		LPINITT2DLL lpinitT2DLL = NULL;
		hDLL = LoadLibrary(L"tribesnext.dll"); // AfxLoadLibrary is probably better.

		if (hDLL == NULL)
			return; // The DLL doesn't exist
		else
			lpinitT2DLL = (LPINITT2DLL)GetProcAddress(hDLL, "_Z9initT2Dllv"); // Attempt to load our entry point

		if (lpinitT2DLL == NULL)
			return; // Unable to load entry point
		else
			lpinitT2DLL(); // The function was loaded, call TribesNext and move on to postTN Startup
}
	
#ifdef _MANAGED
#pragma managed(pop)
#endif

