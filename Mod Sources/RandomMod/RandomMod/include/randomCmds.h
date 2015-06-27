#include <LinkerAPI.h>

const char* conGuiTsCtrlProject(Linker::SimObject *obj,S32 argc, const char* argv[]);
bool conNetObjectSetGhostable(Linker::SimObject *obj,S32 argc, const char* argv[]);
const char* conGetVariable(Linker::SimObject *obj,S32 argc, const char* argv[]);

// Merscenne Random
const char *conMRandom(Linker::SimObject *obj, S32 argc, const char *argv[]);
const char *conMSeed(Linker::SimObject *obj, S32 argc, const char *argv[]);
const char *conMRandomFloat(Linker::SimObject *obj, S32 argc, const char *argv[]);

// Clock
const char*conQPCBegin(Linker::SimObject *obj, S32 argc, const char *argv[]);
const char*conQPCEnd(Linker::SimObject *obj, S32 argc, const char *argv[]);