#pragma once

#include <DXAPI/MatMath.h>
#include <DXAPI/NetConnection.h>
#include <LinkerAPI.h>
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
