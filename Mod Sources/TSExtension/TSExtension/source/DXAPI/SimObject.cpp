#include <cstdarg>

#include <DXAPI/SimObject.h>

#include <LinkerAPI.h>

namespace DX
{
	SimObject::SimObject(unsigned int obj) : identifier(*(unsigned int*)(obj + 32)),
	base_pointer_value(obj)
	{
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

	const char *SimObject::TSCall(const char *name, unsigned int argc, ...)
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