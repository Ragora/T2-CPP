/*
 *	t2conCmds.h
 *	Original code by Linker
 *	Modified by Robert MacGregor
*/

#include "stdafx.h"

// Linker's original functions
const char* conGuiTsCtrlProject(SimObject *obj,S32 argc, const char* argv[]);
bool conNetObjectSetGhostable(SimObject *obj,S32 argc, const char* argv[]);
const char* conGetVariable(SimObject *obj,S32 argc, const char* argv[]);

// Mod Loader Implementation
bool conLoadMod(SimObject *obj,S32 argc, const char* argv[]);