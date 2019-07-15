#include <DXAPI/TCPObject.h>

namespace DX
{
	TCPObject::TCPObject(unsigned int obj) : SimObject(obj),
		state(MEMBER_FIELD(obj, unsigned int, 56))
	{
	}
}