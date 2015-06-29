/*
 *	t2dll.cpp
 *	Original code by Linker
 *	Modified by Robert MacGregor
*/

#include "stdafx.h"
#include "t2ConCmds.h"
#include <LinkerAPI.h>
#include <Windows.h>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    return true;
}


extern "C"
{
	__declspec(dllexport) void ModInitialize(void)
	{
		Con::addMethodB(NULL, "bulletInitialize", &conBulletInitialize, "Initializes the Bullet physics system.", 0, 0);
		Con::addMethodB(NULL, "bulletDeinitialize", &conBulletDeinitialize, "Deinitializes the Bullet physics system.", 0, 0);

		Con::addMethodS(NULL, "bulletGetPosition", &conBulletGetPosition, "Queries Bullet for the position of the object identified by the input id.", 2, 2);
		Con::addMethodS(NULL, "bulletGetRotation", &conBulletGetRotation, "Queries Bullet for the rotation of the object identified by the input id.", 2, 2);

		Con::addMethodB(NULL, "bulletSetPosition", &conBulletSetPosition, "bulletSetPosition(id, position_x, position_y, position_z)", 5, 5);
		Con::addMethodB(NULL, "bulletSetGravity", &conBulletSetGravity, "bulletSetGravity(x,y,z)", 4, 4);
		Con::addMethodB(NULL, "bulletInitialized", &conBulletInitialized, "Returns whether or not if Bullet has been initialized.", 0,0);

		Con::addMethodS(NULL, "bulletCreateCube", &conBulletCreateCube, "bulletMakeCube(mass, scaleX, scaleY, scaleZ, boundID)", 6, 6);

	}

	__declspec(dllexport) void ServerProcess(unsigned int deltaTime)
	{
		updateBullet(deltaTime);
	}
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

