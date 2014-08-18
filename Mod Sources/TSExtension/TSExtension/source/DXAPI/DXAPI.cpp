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

#include <DXAPI/Point3F.h>
#include <DXAPI/DXAPI.h>

#include <LinkerAPI.h>

namespace DX 
{
	void Projectile_explode(Projectile *obj, const Point3F &position, const Point3F &normal, const unsigned int collideType) 
	{
		typedef void (__cdecl *explodeFunc)(const Point3F &position, const Point3F &normal, const unsigned int collideType);
		static explodeFunc function_call = (explodeFunc)0x62DC30;

		__asm 
		{
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