#include <DXAPI/SceneObject.h>
#include <LinkerAPI.h>
namespace DX
{
	//This is required to make these update properly over the network
	//memPatch("602D1E","9090");
	SceneObject::SceneObject(unsigned int obj) : position(*(float*)(obj + 168), *(float*)(obj + 184), *(float*)(obj + 200)),
	scale(*(float*)(obj + 284), *(float*)(obj + 288), *(float*)(obj + 292)), worldtoobj((void*)(obj+0xdc)),objtoworld((void*)(obj+0x9c)),renderobjtoworld((void*)(obj+360)),renderworldtoobj((void*)(obj+424)),
	NetObject(obj)
	{
	}
	float * SceneObject::getPosition(){
		if (this->base_pointer_value) {
			const char * results = Con::getMatrixPosition(objtoworld,NULL,0);
			float pos[3];
			sscanf (results,"%g %g %g", &pos[0], &pos[1], &pos[2]);
			return pos;
		}
	}
	float * SceneObject::getRotation(){
		if (this->base_pointer_value) {
			const char * results = Con::getMatrixRotation(objtoworld,NULL,0);
			float pos[4];
			sscanf (results,"%g %g %g %g", &pos[0], &pos[1], &pos[2], &pos[3]);
			return pos;
		}
	}	
	void SceneObject::setRotation(float rot []) {
		char arg0[128] = "";
		sprintf (arg0,"%g %g %g %g", rot[0], rot[1], rot[2], rot[3]);
		char * argv[] = { &arg0[0], NULL };
		int argc = 1;
		Con::setMatrixRotation(objtoworld,argc,(const char **)argv,NULL,0);
	}
	
	void SceneObject::setPosition(float pos []) {
		char arg0[128] = "";
		sprintf (arg0,"%g %g %g", pos[0], pos[1], pos[2]);
		char * argv[] = { &arg0[0], NULL };
		int argc = 1;
		Con::setMatrixPosition(objtoworld,argc,(const char **)argv,NULL,0);
	}
}