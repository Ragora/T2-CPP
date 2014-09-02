#include <DXAPI/NetObject.h>

namespace DX
{
	NetObject::NetObject(unsigned int obj) : net_flags(*(unsigned int*)(obj + 64)),
	SimObject(obj)
	{
	}
}