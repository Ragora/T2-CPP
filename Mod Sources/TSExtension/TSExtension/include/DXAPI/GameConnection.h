#pragma once

#include <DXAPI/Point3F.h>
#include <DXAPI/NetConnection.h>

#include <DXAPI/ShapeBase.h>

namespace DX
{
	class GameConnection : public NetConnection
	{
	public:
		GameConnection(unsigned int obj);

		ShapeBase getControlObject(void);
	};
} // End NameSpace DX
