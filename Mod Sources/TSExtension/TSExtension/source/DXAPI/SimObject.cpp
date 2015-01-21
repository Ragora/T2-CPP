#include <cstdarg>

#include <DXAPI/SimObject.h>

#include <LinkerAPI.h>

namespace DX
{
	SimObject::SimObject(unsigned int obj) : identifier(*(unsigned int*)(obj + 32)), type(*(unsigned int*)(obj+0x28)),
	base_pointer_value(obj)
	{
	}	
	const char *SimObject::getClassName(){
		unsigned int bpv=this->base_pointer_value;
		unsigned int cnptr=0;
		unsigned int bpv2=(*(unsigned int*)bpv);
		__asm {
		mov ecx,bpv2
		mov eax,[ecx]
		call eax
		mov eax,[eax+0x1C]
		mov cnptr,eax
		};
		if (cnptr) {
			return (char *) cnptr;
		} else {
			return "";
		}
	}
	void SimObject::deleteObject(void)
	{
		void *pointer = (void*)this->base_pointer_value;

		typedef void (__cdecl *deleteObjectFunc)(void);
		static deleteObjectFunc function_call = (deleteObjectFunc)0x4268D0;

		__asm 
		{
			mov ecx, pointer;
			lea eax, function_call;
			mov eax, [eax];
			call eax;
		}
	}

	const char *SimObject::CallMethod(const char *name, unsigned int argc, ...)
	{
		char **argv = (char**)malloc(sizeof(char*) * (2 + argc));
		argv[0]= (char*)name;
		argv[1] = "";

		va_list vargs;
		va_start(vargs, argc);

		for (unsigned int iteration = 0; iteration < argc; iteration++)
			argv[2 + iteration] = va_arg(vargs, char*);
		va_end(vargs);

		const char *result = Con::executem((Linker::SimObject*)this->base_pointer_value, 2 + argc, (const char**)argv);
		free(argv);

		return result;
	}
}