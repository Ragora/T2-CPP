#include <cstdarg>
#include <DXAPI/SimObject.h>
#include <DXAPI/SimGroup.h>

#include <LinkerAPI.h>

namespace DX
{

	SimGroup::SimGroup(unsigned int obj) : count(*(unsigned int*)(obj+0x30)), simobjptr((unsigned int*)*(unsigned int*)(obj+0x38)),
	SimObject(obj)
	{
		
	
	}
	unsigned int SimGroup::getObject(unsigned int idx) {
		if (idx<this->count) {
			return simobjptr[idx];
		} else {
			return 0;
		}
	}
	unsigned int SimGroup::getCount(void) {
			return this->count;
	}

};