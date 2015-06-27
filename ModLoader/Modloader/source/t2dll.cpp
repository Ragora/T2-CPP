/*
 *	t2dll.cpp
 *	Original code by Linker
 *	Modified by Robert MacGregor
*/

#include <stdafx.h>

#include <t2api.h>
#include <modLoader.h>

#include <unordered_set>

#define endian(hex) (((hex & 0x000000FF) << 24)+((hex & 0x0000FF00) << 8)+((hex & 0x00FF0000)>>8)+((hex & 0xFF000000) >> 24))

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    return true;
}

const char* congetServPAddr(SimObject *obj, S32 argc, const char *argv[]) 
{
		char test[256] = "";
		char test2[256]="";
		int spr=(signed int)*serverProcessReplacement;
		sprintf(test2,"B8%08XFFD089EC5DC3",endian(spr));
		/*test2[0]=test[6];
		test2[1]=test[7];
		test2[2]=test[4];
		test2[3]=test[5];
		test2[4]=test[2];
		test2[5]=test[3];
		test2[6]=test[0];
		test2[7]=test[1];
		test2[8]=0;*/
		return test2;
}

__declspec(dllexport) void initT2Dll(void)
{
		Con::addVariable("$cpuspeed",TypeS32,reinterpret_cast<void*>(0x8477F8)); //1 - S32, this is so i can set my cpu speed to 31337 or osmething =P
		Con::addVariable("$GameBase::showBoundingBox",TypeBool,reinterpret_cast<void*>(0x9ECF24));

		// Mod Loader Function
		Con::addMethodB(NULL, "loadMod", &conLoadMod, "Loads a C++ modification.",2,2);
		Con::addMethodS(NULL, "getServPAddr",&congetServPAddr,"Gets the memPatch data for ServerProcess",1,1);

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

