#pragma once

#include <DXAPI/SimObject.h>

namespace DX
{
	class TCPObject : public SimObject
	{
	public:
		TCPObject(unsigned int obj);

		unsigned int &state;
	};
} // End NameSpace DX
