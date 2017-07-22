/**
 *	@file DXAPI.cpp
 *	@brief The DXAPI is an API that allows you to manipulate various game objects 
 *	in Tribes 2 from raw C++ code. It dynamically resolves the addresses of member
 *	variables as you can't really trust the compiler to produce binary compatible classes,
 *	especially with all the inheritance involved in the original Tribes 2 codebase.
 *
 *	This code wouldn't be possible without Linker's original gift on the-construct.net forums,
 *	whose files are appropriately labelled in this codebase. These files have been edited where
 *	appropriate in order to make that code play better with the DXAPI.
 *
 *	@copyright (c) 2014 Robert MacGregor
 */

#include <DXAPI/Point3F.h>
#include <DXAPI/DXAPI.h>
#include <Windows.h>
#include <LinkerAPI.h>

namespace DX 
{
	const char * StringTableInsert(const char * str,bool casesensitive) {
		const char* retval;
		unsigned int * StringTablePtr=(unsigned int *)0x9e618c;
		unsigned int StrTableAddr=*StringTablePtr;
		__asm {
			mov ecx,StrTableAddr
			push casesensitive
			push str
			mov eax,0x441A00
			call eax
			mov retval,eax
		}
		return retval;
	}

	bool memPatch(unsigned int addr, unsigned char * data, unsigned int size){
		DWORD oldprotect=0;
		DWORD oldnewprotect=0;
		VirtualProtect((void *)addr,size,PAGE_EXECUTE_READWRITE,&oldprotect);
		memcpy((void *)addr,(void*) data,size);
		VirtualProtect((void *)addr,size,oldprotect,&oldnewprotect);
		return true;
	}

	float memToFloat(unsigned int addr){
		DWORD oldprotect=0;
		DWORD oldnewprotect=0;
		VirtualProtect((void *)addr,4,PAGE_EXECUTE_READWRITE,&oldprotect);
		float * floatout=(float *)addr;
		float retfloat=0.0;
		retfloat = *floatout;
		VirtualProtect((void *)addr,4,oldprotect,&oldnewprotect);
		return true;
	}

	unsigned int memToUInt(unsigned int addr){
		DWORD oldprotect=0;
		DWORD oldnewprotect=0;
		VirtualProtect((void *)addr,4,PAGE_EXECUTE_READWRITE,&oldprotect);
		unsigned int * intout=(unsigned int *)addr;
		int retint=0;
		retint = *intout;
		VirtualProtect((void *)addr,4,oldprotect,&oldnewprotect);
		return true;
	}

	bool memToHex(unsigned int addr, char * dst, int size, bool spaces=false){
		DWORD oldprotect=0;
		DWORD oldnewprotect=0;
		char hexdigits[20]="";
		char outstr[256];
		VirtualProtect((void *)addr,size,PAGE_EXECUTE_READWRITE,&oldprotect);
		for (int i=0; i<size; i++) {
			if (spaces==true) {
				sprintf(hexdigits,"%02X ",*((unsigned char*)addr+i));
			} else {
				sprintf(hexdigits,"%02X",*((unsigned char*)addr+i));
			}
			strncat(outstr,hexdigits,254-strlen(hexdigits));

		}
		VirtualProtect((void *)addr,size,oldprotect,&oldnewprotect);
		strncpy(dst,outstr,255);
		return true;
	}

	const char *GetModPaths(void)
	{
		int pointer = *(int*)0x9E8690;
		pointer = *(int*)(pointer + 2048);
		return (const char*)pointer;
	}

	bool IsFile(const char *filename)
	{
		typedef bool (*IsFileFuncPtr)(void*, const char *filename, int);
		static IsFileFuncPtr IsFileFunc = (IsFileFuncPtr)0x440DF0;

		// File Manager Object or something?
		void *unknown = *(void**)0x9E8690;
		int unknown_int = 0; // Not sure what this is

		int result = 0;
		__asm
		{
			push unknown_int;
			push filename;
			mov ecx, unknown;
			lea eax, IsFileFunc;
			mov eax, [eax];
			call eax;
			mov result, eax;
		}

		return result != 0;
	}

	bool GetRelativePath(const char *filename, char *ret, int buffer_length)
	{
		// Make sure T2 can see it first off, we don't want to be loading
		// arbitrary files on disk
		if (!IsFile(filename))
			return false;

		const char *modpaths = GetModPaths();

		int modpaths_len = strlen(modpaths);
		char *modpaths_temp = new char[modpaths_len + 1];
		memcpy(modpaths_temp, modpaths, modpaths_len + 1);

		// Now we process all the modpaths
		int last_start = 0;
		for (int iteration = 0; iteration < modpaths_len + 1; iteration++)
			if (modpaths_temp[iteration] == ';' || iteration == modpaths_len)
			{
				memset(ret, 0x00, buffer_length);
				modpaths_temp[iteration] = 0x00;
				char *modname = &modpaths_temp[last_start];

				sprintf_s(ret, buffer_length, "%s/%s", modname, filename);
				
				// Check if it exists
				FILE *handle = fopen(ret, "r");
				if (handle)
				{
					fclose(handle);
					delete[] modpaths_temp;
					return true;
				}

				last_start = iteration + 1;
			}

		delete[] modpaths_temp;
		return false;
	}

	bool GetRunningMod(char *ret, int buffer_length)
	{
		const char *modpaths = GetModPaths();
		unsigned int start_point = (unsigned int)modpaths;
		unsigned int end_point = (unsigned int)strstr(modpaths, ";");

		// FIXME: Potential Buffer overflow
		if (end_point == 0)
			memcpy(ret, modpaths, strlen(modpaths));
		else
			memcpy(ret, modpaths, end_point - start_point);

		// FIXME: Attackers can use setModPath() to attempt to trick my code into loading files
		// outside of Tribes 2's reach
		return SanitizeFileName(ret, buffer_length);
	}

	bool SanitizeFileName(char *ret, int buffer_length)
	{
		bool was_dirty = false;
		for (unsigned int iteration = 0; iteration < strlen(ret); iteration++)
			if (ret[iteration] == '.' || ret[iteration] == '\\' || ret[iteration] == '/' || ret[iteration] == '~')
			{
				was_dirty = true;
				ret[iteration] == 0x20; // In the event the occurence is at the very end

				for (unsigned int replace_iteration = iteration; replace_iteration < strlen(ret); replace_iteration++)
					ret[replace_iteration] = ret[replace_iteration + 1];
			}

		return was_dirty;
	}

	bool memPatch(void* address, void* payload, unsigned int payloadSize)
	{
		DWORD oldProtect;
		bool success = VirtualProtect(address, payloadSize, PAGE_EXECUTE_READWRITE, &oldProtect);

		if (!success)
			return false;

		// Come on Microsoft... why can't anything you make actually be good.
		// NOTE: This memcpy invocation was crashing the linker...
		//memcpy(address, payload, payloadSize);

		unsigned char* destination = reinterpret_cast<unsigned char*>(address);
		unsigned char* sourceData = reinterpret_cast<unsigned char*>(payload);
		for (unsigned int iteration = 0; iteration < payloadSize; ++iteration)
			destination[iteration] = sourceData[iteration];

		return true;
	}

	bool memPatch(unsigned int address, void* payload, unsigned int payloadSize)
	{
		return memPatch(reinterpret_cast<void*>(address), payload, payloadSize);
	}
}