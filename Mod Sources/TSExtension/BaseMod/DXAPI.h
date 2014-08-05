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
 
#pragma once

#include "LinkerAPI.h"

namespace DX
{
	//! A typedef referring to some type of unresolved object in the game.
	typedef void* UnresolvedObject;

	class Projectile {};

	//! Structure representing a player object in the game.
	typedef struct
	{
		//! Object Name
		const char *name;
		//! Object ID
		const unsigned int &id;

		//! X Coordinate of the position.
		float &position_x;
		//! Y Coordinate of the position.
		float &position_y;
		//! Z Coordinate of the position.
		float &position_z;

		//! Player Object Jetting State (readonly, writing it doesn't do anything)
		const bool &is_jetting;
		//! Player Object Jumping State (readonly, writing it doesn't do anything)
		const bool &is_jumping;
	} Player;

	/**
	 *	@brief Returns a usable Player structure from a void
	 *	pointer.
	 *	@param obj A void pointer to attempt to resolve from.
	 *	@return A usable Player structure to manipulate.
	 */
	Player GetPlayerPointer(UnresolvedObject obj);

	//! Structure representing a static shape in the game.
	typedef struct
	{
		//! Object Name
		const char *name;
		//! X Coordinate of the position.
		float &position_x;
		//! Y Coordinate of the position.
		float &position_y;
		//! Z Coordinate of the position.
		float &position_z;
	} StaticShape;

	/**
	 *	@brief Returns a usable StaticShape structure from a void
	 *	pointer.
	 *	@param obj A void pointer to attempt to resolve from.
	 *	@return A usable StaticShape structure to manipulate.
	 */
	StaticShape GetStaticShapePointer(UnresolvedObject obj);

	//! Structure representing a SimGroup in the game.
	typedef struct
	{

	} SimGroup;

	/**
	 *	@brief Returns a usable SimGroup structure from a void
	 *	pointer.
	 *	@param obj A void pointer to attempt to resolve from.
	 *	@return A usable SimGroup structure to manipulate.
	 */
	SimGroup GetSimGroupPointer(UnresolvedObject obj);


	//! Structure representing a FlyingVehicle in the game.
	typedef struct
	{
		//! Strafing State. 0=Not Strafing. -1=Strafing Left. 1=Strafing Right (readonly, writing it doesn't do anything)
		const float &strafing_state; 
	} FlyingVehicle;

	/**
	 *	@brief Returns a usable FlyingVehicle structure from a void
	 *	pointer.
	 *	@param obj A void pointer to attempt to resolve from.
	 *	@return A usable FlyingVehicle structure to manipulate.
	 */
	FlyingVehicle GetFlyingVehiclePointer(UnresolvedObject obj);

	void Projectile_explode(Projectile *obj, const Point3F &position, const Point3F &normal, const unsigned int collideType);
}