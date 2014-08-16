/**
 *	@brief The DXAPI is an API that allows you to manipulate various game objects 
 *	in Tribes 2 from raw C++ code. It dynamically resolves the addresses of member
 *	variables as you can't really trust the compiler to produce binary compatible classes,
 *	especially with all the inheritance involved in the original Tribes 2 codebase.
 *
 *	This code wouldn't be possible without Linker's original gift on the-construct.net forums,
 *	whose files are appropriately labeled in this codebase.
 *
 *	@copyright (c) 2014 Robert MacGregor
 */

#pragma once

#include <LinkerAPI.h>

// Returns the address of an object in memory
const char* conGetAddress(SimObject *obj, S32 argc, const char *argv[]);

// Player Commands -----------------------------------
bool conPlayerGetJumpingState(SimObject *obj, S32 argc, const char* argv[]);
bool conPlayerGetJettingState(SimObject *obj, S32 argc, const char* argv[]);

// GrenadeProjectile Commands ------------------------
const char* conGrenadeProjectileGetPosition(SimObject *obj, S32 argc, const char* argv[]);
const char* conGrenadeProjectileGetVelocity(SimObject *obj, S32 argc, const char* argv[]);

// Projectile explode -------------------------------
bool conProjectileExplode(SimObject *obj, S32 argc, const char* argv[]);

// General Commands ---------------------------------
const char* conSprintf(SimObject *obj, S32 argc, const char* argv[]);