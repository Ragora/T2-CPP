#pragma once

#include <DXAPI/GameBase.h>

namespace DX
{
	class Projectile : public GameBase
	{
	public:
		Projectile(unsigned int obj);

		//! Velocity. It is constant because modifying it directly breaks the sim.
		const Point3F velocity;

		bool &hidden;
	};
} // End NameSpace DX