#pragma once

#include <DXAPI/Point3F.h>
#include <DXAPI/NetObject.h>

namespace DX
{
	class SceneObject : public NetObject
	{
	public:
		SceneObject(unsigned int obj);
		void SceneObject::getPosition(float * pos);
		void SceneObject::getRotation(float * rot);
		void SceneObject::setRotation(float []);
		void SceneObject::setPosition(float []);
		void * worldtoobj;
		void * objtoworld;
		void * renderobjtoworld;
		void * renderworldtoobj;

		Point3F position;
		Point3F scale;
	};
	
} // End NameSpace DX
