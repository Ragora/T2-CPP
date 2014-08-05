/*
 *	t2ConCmds.cpp
 *	Original code by Linker
 *	Modified by Robert MacGregor
*/

#include <string>

#include "stdafx.h"
#include "t2ConCmds.h"

// Linker's implementations
const char* conGuiTsCtrlProject(SimObject *obj,S32 argc, const char* argv[]) {
	Point3F pt;
	Point3F rt;
	dSscanf(argv[2],"%g %g %g",&pt.x,&pt.y,&pt.z);
	GuiTSCtrl_project(reinterpret_cast<GuiTSCtrl *>(obj),pt,&rt);

	char* buffer = Con::getReturnBuffer(255);
	dSprintf(buffer,255,"%g %g %g",rt.x,rt.y,rt.z);

	return buffer;
}

bool conNetObjectSetGhostable(SimObject *obj,S32 argc, const char* argv[]) {
	if (dAtob(argv[2])) {
		__asm {
			push ecx
			mov ecx, [obj]
			mov eax,[ecx+40h]
			or eax, 100h
			mov [ecx+40h], eax
			pop ecx
		}
	} else {
		__asm {
			xor eax, eax
			or eax, 100h
			not eax
			push ecx
			mov ecx,[obj]
			and eax, [ecx+40h]
			mov [ecx+40h],eax
			push    40h
			mov eax, 0x585BE0
			call eax
			pop ecx
		}
	}
	return 1;
}

const char* conGetVariable(SimObject *obj,S32 argc, const char* argv[]) {
	return Con::getVariable(argv[1]);
}

// Mod Loader Implementation
bool conLoadMod(SimObject *obj,S32 argc, const char* argv[])
{
		typedef void (*LPMODINIT)(void);
		HINSTANCE hDLL = NULL;
		LPMODINIT lpInitMod = NULL;

		std::string raw = argv[1];

		std::wstring input(raw.begin(), raw.end());
		std::wstring modification = L"mods/";
		modification += input;
		modification += L".dll";

		hDLL = LoadLibrary(modification.c_str()); // AfxLoadLibrary is probably better.

		if (hDLL == NULL)
			return false; // The DLL doesn't exist
		else
			lpInitMod = (LPMODINIT)GetProcAddress(hDLL, "?ModInitialize@@YAXXZ"); // Attempt to load our entry point

		if (lpInitMod == NULL)
			return false; // Unable to load entry point
		else
			lpInitMod(); // The function was loaded, call TribesNext and move on to postTN Startup

		return true;
}