#include <DXAPI/SceneObject.h>

namespace DX
{
	SceneObject::SceneObject(unsigned int obj) : position(*(float*)(obj + 168), *(float*)(obj + 184), *(float*)(obj + 200)),
	scale(*(float*)(obj + 284), *(float*)(obj + 288), *(float*)(obj + 292)),
	worldtoobj((float*)(obj + 0xDC)),
	objtoworld((float*)(obj+0x9C)),
	objboxmin(*(float*)(obj + 296), *(float*)(obj + 300), *(float*)(obj + 304)),
	objboxmax(*(float*)(obj + 308), *(float*)(obj + 312), *(float*)(obj + 316)),
	worldboxmin(*(float*)(obj + 320), *(float*)(obj + 324), *(float*)(obj + 328)),
	worldboxmax(*(float*)(obj + 332), *(float*)(obj + 336), *(float*)(obj + 340)),
	worldspherecenter(*(float*)(obj + 344), *(float*)(obj + 348), *(float*)(obj + 352)),
	worldsphereradius(*(float*)(obj+356)),
	renderobjtoworld((float*)(obj+360)),
	renderworldtoobj((float*)(obj+424)),
	NetObject(obj)
	{
	}
}