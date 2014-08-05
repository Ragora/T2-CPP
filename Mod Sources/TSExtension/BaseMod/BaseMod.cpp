// BaseMod.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "BaseMod.h"

#include "DXAPI.h"
#include "LinkerAPI.h"

#include "DXConCmds.h"

extern "C"
{
	BASEMOD_API void ModInitialize(void)
	{
		Con::addMethodB("Player", "isjumping", &conPlayerGetJumpingState,"Returns whether or not the player is jumping", 2, 2);
		Con::addMethodB("Player", "isjetting", &conPlayerGetJettingState,"Returns whether or not the player is jetting", 2, 2);

		Con::addMethodB("GrenadeProjectile", "explode", &conProjectileExplode,"Explodes the given projectile", 5, 5);
		Con::addMethodB("Projectile", "explode", &conProjectileExplode,"Explodes the given projectile", 5, 5);
	}
}