#include <DXAPI/Projectile.h>

namespace DX
{
	Projectile::Projectile(unsigned int obj) : velocity(*(float*)(obj + 892), *(float*)(obj + 896), *(float*)(obj + 900)),
	hidden(*(bool*)(obj + 796)),
	GameBase(obj)
	{
	}

	void Projectile::explode(const Linker::Point3F &position, const Linker::Point3F &normal, const unsigned int collideType)
	{
		void *pointer = (void*)this->base_pointer_value;

		typedef void (__cdecl *explodeFunc)(const Linker::Point3F &position, const Linker::Point3F &normal, const unsigned int collideType);
		static explodeFunc function_call = (explodeFunc)0x62DC30;

		__asm 
		{
			push collideType;
			push normal;
			push position;
			mov ecx,pointer;
			lea eax, function_call;
			mov eax, [eax];
			call eax;
		}
	}
}