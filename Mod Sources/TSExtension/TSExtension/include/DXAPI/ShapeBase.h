#pragma once

#include <DXAPI/GameBase.h>

namespace DX
{
	class ShapeBase : public GameBase
	{
	public:
		ShapeBase(unsigned int obj);

		//! Heat Level
		float &heat_level;
	};
} // End NameSpace DX