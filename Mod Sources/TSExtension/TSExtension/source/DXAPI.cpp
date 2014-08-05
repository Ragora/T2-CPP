/**
 *	@file DXAPI.cpp
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

#include "DXAPI.h"
#include "LinkerAPI.h"

namespace DX 
{
	Player GetPlayerPointer(UnresolvedObject obj)
	{
		// Probably some way to do all this via assembly but screw me if I know
		unsigned int base_tribes_pointer = (unsigned int)obj;

		Player result = 
		{ 
			0x00,								         // Object Name
			*(unsigned int*)(base_tribes_pointer + 32),  // Object ID
			*(float*)(base_tribes_pointer + 168),        // Position X
			*(float*)(base_tribes_pointer + 184),        // Position Y
			*(float*)(base_tribes_pointer + 200),        // Position Z
			*(bool*)(base_tribes_pointer + 735),         // Jetting State
			*(bool*)(base_tribes_pointer + 734)	         // Jumping State
		};
		return result;
	}

	StaticShape GetStaticShapePointer(UnresolvedObject obj)
	{
		unsigned int base_tribes_pointer = (unsigned int)obj;

		StaticShape result = 
		{ 
			0x00, 								   // Object Name
			*(float*)(base_tribes_pointer + 200),  // Position X
			*(float*)(base_tribes_pointer + 200),  // Position Y
			*(float*)(base_tribes_pointer + 200)   // Position Z
		};
		return result;
	}

	SimGroup GetSimGroupPointer(UnresolvedObject obj)
	{
		unsigned int base_tribes_pointer = (unsigned int)obj;

		SimGroup result = 
		{ 

		};
		return result;
	}

	FlyingVehicle GetFlyingVehiclePointer(UnresolvedObject obj)
	{
		unsigned int base_tribes_pointer = (unsigned int)obj;

		FlyingVehicle result = 
		{ 
			*(float*)(base_tribes_pointer + 2200),  // Strafing Status
		};
		return result;
	}

	void Projectile_explode(Projectile *obj, const Point3F &position, const Point3F &normal, const unsigned int collideType) 
	{
		typedef void (__cdecl *explodeFunc)(const Point3F &position, const Point3F &normal, const unsigned int collideType);
		static explodeFunc function_call = (explodeFunc)0x62DC30;

		__asm {
			push collideType;
			push normal;
			push position;
			mov ecx,obj;
			lea eax, function_call;
			mov eax, [eax];
			call eax;
		}
}

}