#pragma once

#include <DXAPI/Point3F.h>
#include <DXAPI/NetConnection.h>
#include <LinkerAPI.h>
#include <DXAPI/ShapeBase.h>

namespace DX
{
	/**
	 *	@brief A class representing an incoming connection to the Tribes 2 server.
	 */
	class GameConnection : public NetConnection
	{
		public:
			GameConnection(unsigned int obj);

			/**
			 *	@brief Gets the control object this game connection is controlling.
			 *	@return The shapebase instance being controlled.
			 */
			ShapeBase getControlObject(void);
	};
} // End NameSpace DX
