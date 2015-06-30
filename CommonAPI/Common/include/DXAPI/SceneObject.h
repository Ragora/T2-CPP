#pragma once

#include <DXAPI/Point3F.h>
#include <DXAPI/NetObject.h>

namespace DX
{
	class SceneObject : public NetObject
	{
	public:
		SceneObject(unsigned int obj);

		Point3F position;
		Point3F scale;
		float &rx;
		float &ry;
		float &rz;
		float &rw;
	};
} // End NameSpace DX
