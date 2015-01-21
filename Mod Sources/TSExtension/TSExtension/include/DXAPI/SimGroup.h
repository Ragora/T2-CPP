#pragma once
#include <DXAPI/SimObject.h>
namespace DX
{
	class SimGroup : public SimObject
	{
	public:
		SimGroup(unsigned int obj);
		unsigned int &count;
		unsigned int *simobjptr;
		unsigned int SimGroup::getObject(unsigned int idx);
		unsigned int SimGroup::getCount(void);
	};
};