// dllmain.cpp : Defines the entry point for the DLL application.
#include <SDKDDKVer.h>
#include <Windows.h>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

#include <DXAPI/DXAPI.h>
#include <LinkerAPI.h>

#include <DXConCmds.h>

extern "C"
{
	__declspec(dllexport) void ModInitialize(void)
	{
		Con::addMethodB("Player", "isjumping", &conPlayerGetJumpingState,"Returns whether or not the player is jumping", 2, 2);
		Con::addMethodB("Player", "isjetting", &conPlayerGetJettingState,"Returns whether or not the player is jetting", 2, 2);
		Con::addMethodB("GameConnection", "setheat", &conGameConnectionSetHeatLevel,"Sets the heat level", 3, 3);

		Con::addMethodB("GrenadeProjectile", "explode", &conProjectileExplode,"Explodes the given projectile", 5, 5);
		Con::addMethodB("Projectile", "explode", &conProjectileExplode,"Explodes the given projectile", 5, 5);

		Con::addMethodS("GrenadeProjectile", "getposition", &conGrenadeProjectileGetPosition,"Accurately gets the position of the GrenadeProjectile", 2, 2);
		Con::addMethodS("GrenadeProjectile", "getvelocity", &conGrenadeProjectileGetVelocity,"Gets the velocity of the GrenadeProjectile", 2, 2);
		Con::addMethodB("Projectile", "makeNerf", &conProjectileMakeNerf,"Makes the Projectile deal no damage", 2, 2);

		// TCPObject
		Con::addMethodS("TCPObject", "altConnect", &conTCPObjectConnect, "Connects to a remote server", 3, 3);
		Con::addMethodB("TCPObject", "altSend", &conTCPObjectSend, "Sends data to the remote server", 3, 3);
		Con::addMethodB("TCPObject", "altDisconnect", &conTCPObjectDisconnect, "Disconnects from the remote server", 2, 2);

		// General
		Con::addMethodS(NULL, "sprintf", &conSprintf,"Formats a string. See the C sprintf.", 2, 20);
		Con::addMethodB(NULL, "tsExtensionUpdate", &conTSExtensionUpdate,"Updates the TSExtension.", 1, 1);

		// Add this Gvar to signify that TSExtension is active
		static bool is_active = true;
		Con::addVariable("$TSExtension::isActive", TypeBool, &is_active);
	}
}

