#pragma once

#include <DXAPI/MatMath.h>
#include <DXAPI/NetObject.h>

namespace DX
{
	class SceneObject : public NetObject
	{
	public:
		SceneObject(unsigned int obj);

		Point3F position;
		Point3F scale;
		float * worldtoobj;
		float * objtoworld;
		Point3F objboxmin;
		Point3F objboxmax;
		Point3F worldboxmin;
		Point3F worldboxmax;
		Point3F worldspherecenter;
		float &worldsphereradius;
		float *renderobjtoworld;
		float *renderworldtoobj;
		void setTransform(float *matrixinput) {
			unsigned int bpv=this->base_pointer_value;
			unsigned int minp = (unsigned int) matrixinput;

			__asm {
				push minp
				mov ecx,bpv
				mov eax,bpv
				mov eax,[eax]
				add eax,0x74
				call [eax]
			}
		}
	};
} // End NameSpace DX