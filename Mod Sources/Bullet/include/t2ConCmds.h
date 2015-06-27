/*
 *	t2conCmds.h
 *	Original code by Linker
 *	Modified by Robert MacGregor
*/

#include "stdafx.h"

// Linker's original functions
const char* conGuiTsCtrlProject(Linker::SimObject *obj,S32 argc, const char* argv[]);
bool conNetObjectSetGhostable(Linker::SimObject *obj,S32 argc, const char* argv[]);
const char* conGetVariable(Linker::SimObject *obj,S32 argc, const char* argv[]);

// --- BulletDLL functions ------------------------------------------------
bool conBulletInitialize(Linker::SimObject *obj, S32 argc, const char *argv[]);
bool conBulletDeinitialize(Linker::SimObject *obj, S32 argc, const char *argv[]);
bool conBulletUpdate(Linker::SimObject *obj, S32 argc, const char *argv[]);

// --- Get Commands -------------------------------------------------------
const char *conBulletGetPosition(Linker::SimObject *obj, S32 argc, const char *argv[]);
const char *conBulletGetRotation(Linker::SimObject *obj, S32 argc, const char *argv[]);
bool conBulletInitialized(Linker::SimObject *obj, S32 argc, const char *argv[]);

// --- Set commands -------------------------------------------------------
bool conBulletSetPosition(Linker::SimObject *obj, S32 argc, const char *argv[]);
bool conBulletSetGravity(Linker::SimObject *obj, S32 argc, const char *argv[]);
bool conBulletSetMass(Linker::SimObject *obj, S32 argc, const char *argv[]);
bool conBulletSetRotation(Linker::SimObject *obj, S32 argc, const char *argv[]);

// --- Scene Addition Commands --------------------------------------------
const char *conBulletCreateCube(Linker::SimObject *obj, S32 argc, const char *argv[]);
