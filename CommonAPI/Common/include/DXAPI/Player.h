#pragma once

#include <DXAPI/Point3F.h>
#include <DXAPI/ShapeBase.h>

namespace DX
{
	//! Class representing a player object in the game.
	class Player : public ShapeBase
	{
	public:
		Player(unsigned int obj);

		//! Player Object Jetting State (readonly, writing it doesn't do anything)
		const bool &is_jetting;
		//! Player Object Jumping State (readonly, writing it doesn't do anything)
		const bool &is_jumping;
		float &headRotationZ;
		float &mRotZ;
		//! Player Object Using Toggable Pack
		bool &is_using_toggledpack;

		//! Player Velocity.
		Point3F velocity;
	};
} // End NameSpace DX
