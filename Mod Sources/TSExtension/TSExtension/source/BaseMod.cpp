// BaseMod.cpp : Defines the exported functions for the DLL application.
//

#include <DXAPI.h>
#include <LinkerAPI.h>

#include <DXConCmds.h>

extern "C"
{
	__declspec(dllexport) void ModInitialize(void)
	{
		Con::addMethodB("Player", "isjumping", &conPlayerGetJumpingState,"Returns whether or not the player is jumping", 2, 2);
		Con::addMethodB("Player", "isjetting", &conPlayerGetJettingState,"Returns whether or not the player is jetting", 2, 2);

		Con::addMethodB("GrenadeProjectile", "explode", &conProjectileExplode,"Explodes the given projectile", 5, 5);
		Con::addMethodB("Projectile", "explode", &conProjectileExplode,"Explodes the given projectile", 5, 5);

		Con::addMethodS("GrenadeProjectile", "getposition", &conGrenadeProjectileGetPosition,"Accurately gets the position of the GrenadeProjectile", 2, 2);
		Con::addMethodS("GrenadeProjectile", "getvelocity", &conGrenadeProjectileGetVelocity,"Gets the velocity of the GrenadeProjectile", 2, 2);
	}
}