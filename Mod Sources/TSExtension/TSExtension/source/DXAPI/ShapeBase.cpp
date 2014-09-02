#include <DXAPI/ShapeBase.h>

namespace DX
{
	ShapeBase::ShapeBase(unsigned int obj) : GameBase(obj),
	heat_level(*(float*)(obj + 1972))
	{
	}
}