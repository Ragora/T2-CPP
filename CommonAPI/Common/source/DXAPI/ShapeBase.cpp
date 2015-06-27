#include <DXAPI/ShapeBase.h>
#define CloakMask 0x40
namespace DX
{
	ShapeBase::ShapeBase(unsigned int obj) : GameBase(obj),
	heat_level(*(float*)(obj + 1972)), cloak_level(*(float*)(obj+0x810)), cloaked(*(bool*)(obj+0x80D))
	{
	}
	void ShapeBase::setMaskBits(int value){
		unsigned int addr=this->base_pointer_value;
		unsigned int bits=value;
		unsigned int funcaddr=0x585BE0;
		__asm {
			mov eax,bits
			push eax
			mov ecx,addr
			mov eax,funcaddr
			call eax
		};
	}
}