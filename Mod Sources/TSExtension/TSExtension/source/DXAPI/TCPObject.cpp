#include <DXAPI/TCPObject.h>

namespace DX
{
	TCPObject::TCPObject(unsigned int obj): state(*(unsigned int*)(obj + 56)),
	SimObject(obj)
	{
	}
}