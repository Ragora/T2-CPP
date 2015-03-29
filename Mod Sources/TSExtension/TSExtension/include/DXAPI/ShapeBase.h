#pragma once

#include <DXAPI/GameBase.h>

namespace DX
{
	class ShapeBase : public GameBase
	{
	public:
		ShapeBase(unsigned int obj);
		float &cloak_level;
		bool &cloaked;
		//! Heat Level
		float &heat_level;
		void setMaskBits(int value);
	};
} // End NameSpace DX