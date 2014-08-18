#include <DXAPI/SimObject.h>

namespace DX
{
	SimObject::SimObject(unsigned int obj) : base_pointer_value(obj)
	{
	}

	void SimObject::deleteObject(void)
	{
		void *pointer = (void*)this->base_pointer_value;

		typedef void (__cdecl *deleteObjectFunc)(void);
		static deleteObjectFunc function_call = (deleteObjectFunc)0x439DE0;

		__asm 
		{
			mov ecx, pointer;
			lea eax, function_call;
			mov eax, [eax];
			call eax;
		}
	}
}