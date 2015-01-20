#pragma once

#include <DXAPI/MatMath.h>
#include <DXAPI/GameBase.h>

#include <LinkerAPI.h>

namespace DX
{
	class Projectile : public GameBase
	{
	public:
		Projectile(unsigned int obj);

		void explode(const Linker::Point3F &position, const Linker::Point3F &normal, const unsigned int collideType);

		//! Velocity. It is constant because modifying it directly breaks the sim.
		const Point3F velocity;

		bool &hidden;
	};
} // End NameSpace DX