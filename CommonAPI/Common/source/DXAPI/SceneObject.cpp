#include <DXAPI/SceneObject.h>
#include <LinkerAPI.h>

namespace DX
{
	//This is required to make these update properly over the network
	//memPatch("602D19","9090");
	SceneObject::SceneObject(unsigned int obj) : NetObject(obj),
		position(MEMBER_POINT3F(obj, 168, 16)),
		scale(MEMBER_POINT3F(obj, 284, 4)),
		worldtoobj(MEMBER_POINTER(obj, void, 0xdc)),
		objtoworld(MEMBER_POINTER(obj, void, 0x9c)),
		renderobjtoworld(MEMBER_POINTER(obj, void, 360)),
		renderworldtoobj(MEMBER_POINTER(obj, void, 424))
	{
	}

	void SceneObject::getPosition(float *pos){
		if (this->base_pointer_value) {
			const char * results = Con::getMatrixPosition(objtoworld,NULL,0);
			sscanf (results,"%f %f %f", &pos[0], &pos[1], &pos[2]);
		}
	}

	void SceneObject::getRotation(float * rot){
		if (this->base_pointer_value) {
			const char * results = Con::getMatrixRotation(objtoworld,NULL,0);
			sscanf (results,"%f %f %f %f", &rot[0], &rot[1], &rot[2], &rot[3]);
		}
	}	

	void SceneObject::setRotation(float rot []) {
		char arg0[128] = "";
		sprintf (arg0,"%f %f %f %f", rot[0], rot[1], rot[2], rot[3]);
		char * argv[] = { &arg0[0], NULL };
		int argc = 1;
		Con::setMatrixRotation(objtoworld,argc,(const char **)argv,NULL,0);
		this->setMaskBits(0x2000000);
		//Con::setMatrixRotation(renderobjtoworld,argc,(const char **)argv,NULL,0);
	}
	
	void SceneObject::setPosition(float pos []) {
		char arg0[128] = "";
		sprintf (arg0,"%f %f %f %f", pos[0], pos[1], pos[2], 1.0f);
		char * argv[] = { &arg0[0], NULL };
		int argc = 1;
		Con::setMatrixPosition(objtoworld,argc,(const char **)argv,NULL,0);
		this->setMaskBits(0x2000000);
		//Con::setMatrixPosition(renderobjtoworld,argc,(const char **)argv,NULL,0);
	}
}