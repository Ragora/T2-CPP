/**
 *	@file DXAPI.h
 *	@brief The DXAPI is an API that allows you to manipulate various game objects 
 *	in Tribes 2 from raw C++ code. It dynamically resolves the addresses of member
 *	variables as you can't really trust the compiler to produce binary compatible classes,
 *	especially with all the inheritance involved in the original Tribes 2 codebase.
 *
 *	This code wouldn't be possible without Linker's original gift on the-construct.net forums,
 *	whose files are appropriately labelled in this codebase. These files have been edited where
 *	appropriate in order to make that code play better with the DXAPI.
 *
 *	@copyright (c) 2014 Robert MacGregor
 */
 
//#pragma once

#include "LinkerAPI.h"

#include <DXAPI/Point3F.h>
#include <DXAPI/GameBase.h>
#include <DXAPI/NetObject.h>
#include <DXAPI/Player.h>
#include <DXAPI/Projectile.h>
#include <DXAPI/SceneObject.h>
#include <DXAPI/ShapeBase.h>
#include <DXAPI/SimObject.h>
#include <DXAPI/StaticShape.h>
#include <DXAPI/GrenadeProjectile.h>
#include <DXAPI/FlyingVehicle.h>
#include <DXAPI/GameConnection.h>
#include <DXAPI/NetConnection.h>
#include <DXAPI/TCPObject.h>
#include <DXAPI/ScriptObject.h>

namespace DX
{
	//! A typedef referring to some type of unresolved object in the game.
	typedef void* UnresolvedObject;

	const char *GetModPaths(void);
	bool IsFile(const char *filename);

	bool GetRelativePath(const char *filename, char *ret, int buffer_length);
	bool GetRunningMod(char *ret, int buffer_length);

	bool SanitizeFileName(char *ret, int buffer_length);
} // End NameSpace DX