#include <DXAPI/SceneObject.h>

namespace DX
{
	SceneObject::SceneObject(unsigned int obj) : position(*(float*)(obj + 168), *(float*)(obj + 184), *(float*)(obj + 200)),
	scale(*(float*)(obj + 284), *(float*)(obj + 288), *(float*)(obj + 292)),
	NetObject(obj)
	{
	}
}