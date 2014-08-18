#include <DXAPI/Projectile.h>

namespace DX
{
	Projectile::Projectile(unsigned int obj) : velocity(*(float*)(obj + 892), *(float*)(obj + 896), *(float*)(obj + 900)),
	hidden(*(bool*)(obj + 796)),
	GameBase(obj)
	{
	}
}