#include <DXAPI/NetConnection.h>
#include <DXAPI/NetObject.h>
#include <LinkerAPI.h>
namespace DX
{
		S32 NetConnection::getGhostIndex(NetObject *obj) {
		unsigned int object_ptr = (unsigned int)obj->base_pointer_value;
		unsigned int my_ptr = this->base_pointer_value-0xA0;
		unsigned int ghostid=0;
		unsigned int function=0x584FB0;
		__asm
		{
			mov ecx,my_ptr
			mov edx,object_ptr
			push edx
			call function
			mov ghostid, eax
		}
		return ghostid;
	}

	NetConnection::NetConnection(unsigned int obj) :	SimObject(obj)

	{
			this->mGhostRefs = (GhostInfo *)this->base_pointer_value-0xA0+0x8210;
	}

}

