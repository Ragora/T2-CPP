#pragma once

#include <DXAPI/Point3F.h>
#include <DXAPI/ShapeBase.h>

namespace DX
{
	//! Class representing a StaticShape in the game.
	class StaticShape : public ShapeBase
	{
	public:
		StaticShape(unsigned int obj);

		//! Object Name
		//const char *name;
		//! Position
		//const Point3F position;
	};
} // End NameSpace DX