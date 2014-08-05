#include "stdafx.h"

const char* conGuiTsCtrlProject(SimObject *obj,S32 argc, const char* argv[]);
bool conNetObjectSetGhostable(SimObject *obj,S32 argc, const char* argv[]);
const char* conGetVariable(SimObject *obj,S32 argc, const char* argv[]);

// Merscenne Random
const char *conMRandom(SimObject *obj, S32 argc, const char *argv[]);
const char *conMSeed(SimObject *obj, S32 argc, const char *argv[]);
const char *conMRandomFloat(SimObject *obj, S32 argc, const char *argv[]);

// Clock
const char*conQPCBegin(SimObject *obj, S32 argc, const char *argv[]);
const char*conQPCEnd(SimObject *obj, S32 argc, const char *argv[]);