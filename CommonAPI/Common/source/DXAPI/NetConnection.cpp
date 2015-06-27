#include <DXAPI/NetConnection.h>
#include <DXAPI/NetObject.h>
#include <LinkerAPI.h>
namespace DX
{
		unsigned char NetConnection::getGhostFrom() {
			return *(unsigned char *)((this->actualbaseptr)+0x8204);
		}
		unsigned char NetConnection::getGhostTo() {
			return *(unsigned char *)((this->actualbaseptr)+0x8205);
		}
		S32 NetConnection::getGhostIndex(NetObject obj) {
		unsigned int object_ptr = (unsigned int)obj.base_pointer_value;
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
		NetObject NetConnection::resolveGhostParent(S32 id) {
			if (this->getGhostFrom()) {
			if (this->mGhostRefs[id].obj)
			{
			return NetObject((unsigned int)(this->mGhostRefs[id].obj));
			}
			}
			return NULL;
		}
		NetObject NetConnection::resolveGhost(S32 id) {
			if (this->getGhostTo()) {
				return NetObject((unsigned int)this->mLocalGhosts[id]);
			}
			return NULL;
		}
	NetConnection::NetConnection(unsigned int obj) :	SimObject(obj)

	{
			unsigned int ptr=((this->base_pointer_value)-(0xA0));
			this->actualbaseptr=ptr;
			unsigned int * ginfoptrptr=0;
			ginfoptrptr=(unsigned int *) (this->actualbaseptr+(0x8210));
			this->mGhostRefs = (GhostInfo *) *ginfoptrptr;
			this->mLocalGhosts = (NetObject **) *(unsigned int *) ((this->actualbaseptr+(0x820C)));
			
	}

}

