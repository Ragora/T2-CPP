#include <DXAPI/Player.h>

namespace DX
{
	Player::Player(unsigned int obj) : ShapeBase(obj),
	name(0x00), id(*(unsigned int*)(obj + 32)),
	is_jetting(*(bool*)(obj + 735)),
	is_jumping(*(bool*)(obj + 734)),
	is_using_toggledpack(*(bool*)(obj + 1172))
	{

	}
}