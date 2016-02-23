#include <cstdarg>

#include <DXAPI/SimObject.h>

#include <LinkerAPI.h>

namespace DX
{
	SimObject::SimObject(unsigned int obj) : identifier(*(unsigned int*)(obj + 32)), fieldDictionary(*(unsigned int*)(obj + 0x2C)), dataBlock(*(unsigned int*)(obj + 0x248)),
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
	const char *SimObject::getFieldValue(const char *slotname)
	{
		void * getfieldvalueptr=(void *)0x435210;
		const char* retptr;
		void * thisptr=(void *)this->base_pointer_value;
		void * fieldDictPtr=(void*)this->fieldDictionary;
		if (this->base_pointer_value!=0 && this->fieldDictionary!=0) {
			__asm {
				push slotname
				mov ecx,fieldDictPtr
				call getfieldvalueptr
				mov retptr,eax
			};
			if (retptr != NULL) {
				return retptr;
			} else {
				return "";
			}
		}
		return "";
	}
	void SimObject::setDataField(const char *slotname, const char *array, const char *value)
	{
		void * setfieldptr=(void *)0x4364E0;
		void * retptr;
		void * thisptr=(void *)this->base_pointer_value;
		__asm {
			push value
			push array
			push slotname
			mov ecx,thisptr
			call setfieldptr
		};
		return;
	}
}