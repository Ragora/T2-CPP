#include <DXAPI/GameConnection.h>

namespace DX
{
	GameConnection::GameConnection(unsigned int obj) : NetConnection(obj)
	{
	}

	ShapeBase GameConnection::getControlObject(void)
	{
		unsigned int result_ptr = 0;
		unsigned int my_ptr = this->base_pointer_value;
		__asm
		{
			mov ecx, my_ptr;
			add ecx, 0x33f480;

			test ecx, ecx;
			mov edx, ecx;
			jz loc_5FDA60_sim;
			add edx, 4294967136;

			loc_5FDA60_sim:
			mov eax, [edx + 0x825c];
			test eax, eax;
			jnz got_valid_ptr;

			got_valid_ptr:
			mov result_ptr, eax;
		}

		return ShapeBase(result_ptr);
	}
} // End NameSpace DX