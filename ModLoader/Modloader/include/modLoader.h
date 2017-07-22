/*
 *	modLoader.h
*/

#include <LinkerAPI.h>

// Mod Loader Implementation
void serverProcessReplacement(unsigned int timeDelta);
bool conLoadMod(Linker::SimObject *obj,S32 argc, const char* argv[]);