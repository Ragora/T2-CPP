#include <DXAPI/Player.h>

namespace DX
{
	Player::Player(unsigned int obj) : ShapeBase(obj),
		is_jetting(MEMBER_FIELD(obj, bool, 735)),
		is_jumping(MEMBER_FIELD(obj, bool, 734)),

		headRotationZ(MEMBER_FIELD(obj, float, 2376)),
		mRotZ(MEMBER_FIELD(obj, float, 2388)),

		is_using_toggledpack(MEMBER_FIELD(obj, bool, 1172)),
		velocity(MEMBER_POINT3F(obj, 2392, 4))
	{

	}
}