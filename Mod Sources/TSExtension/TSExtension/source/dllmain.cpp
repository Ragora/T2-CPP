// dllmain.cpp : Defines the entry point for the DLL application.
#include <SDKDDKVer.h>
#include <Windows.h>
//#define ENABLE_TCPOBJECT
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
		// Init WSA
		WSADATA wsadata;
		WSAStartup(0x0202, &wsadata);

		Con::addMethodB("Player", "isjumping", &conPlayerGetJumpingState,"Returns whether or not the player is jumping", 2, 2);
		Con::addMethodB("Player", "isjetting", &conPlayerGetJettingState,"Returns whether or not the player is jetting", 2, 2);
		Con::addMethodB("GameConnection", "setheat", &conGameConnectionSetHeatLevel,"Sets the heat level", 3, 3);

		Con::addMethodB("GrenadeProjectile", "explode", &conProjectileExplode,"Explodes the given projectile", 5, 5);
		Con::addMethodB("GameBase","setProcessTicks",&conSetProcessTicks,"Sets the flag for processing ticks or not", 3, 3);
		Con::addMethodB("Projectile", "explode", &conProjectileExplode,"Explodes the given projectile", 5, 5);

		Con::addMethodS("GrenadeProjectile", "getposition", &conGrenadeProjectileGetPosition,"Accurately gets the position of the GrenadeProjectile", 2, 2);
		Con::addMethodS("GrenadeProjectile", "getvelocity", &conGrenadeProjectileGetVelocity,"Gets the velocity of the GrenadeProjectile", 2, 2);
		Con::addMethodB("Projectile", "makeNerf", &conProjectileMakeNerf,"Makes the Projectile deal no damage", 2, 2);
		Con::addMethodS("SceneObject", "getPositionTest", &conGetPosition,"Gets the Rotation by alternate means as a test", 2, 2);
		Con::addMethodB(NULL,"setMPS",&conSetMPS,"sets meters per second for turret", 2, 2);
		// TCPObject
#ifdef ENABLE_TCPOBJECT
		Con::addMethodS("TCPObject", "connect", &conTCPObjectConnect, "Connects to a remote server", 3, 3);
		Con::addMethodB("TCPObject", "send", &conTCPObjectSend, "Sends data to the remote server", 3, 3);
		Con::addMethodB("TCPObject", "disconnect", &conTCPObjectDisconnect, "Disconnects from the remote server", 2, 2);

		// HTTPObject
		Con::addMethodB("HTTPObject", "get", &conHTTPObjectDoNothing, "Disconnects from the remote server", 6, 6);
		Con::addMethodB("HTTPObject", "post", &conHTTPObjectDoNothing, "Disconnects from the remote server", 6, 6);
		Con::addMethodB("HTTPObject", "send", &conHTTPObjectDoNothing, "Disconnects from the remote server", 6, 6);
		Con::addMethodB("HTTPObject", "connect", &conHTTPObjectDoNothing, "Disconnects from the remote server", 6, 6);
		Con::addMethodB("HTTPObject", "listen", &conHTTPObjectDoNothing, "Disconnects from the remote server", 6, 6);
#endif
		// BinaryObject
		Con::addMethodB("BinaryObject", "openforread", &conBinaryObjectOpenForRead, "Opens the input file for reading binary data", 3, 4);
		Con::addMethodB("BinaryObject", "save", &conBinaryObjectSave, "Saves the binary object data to a file", 3, 3);
		Con::addMethodS("BinaryObject", "readu32", &conBinaryObjectReadU32, "Reads an unsigned int from the buffer", 2, 2);
		Con::addMethodS("BinaryObject", "readf32", &conBinaryObjectReadF32, "Reads a float from the buffer", 2, 2);
		Con::addMethodS("BinaryObject", "readu8", &conBinaryObjectReadU8, "Reads a unsigned char from the buffer", 2, 2);
		Con::addMethodB("BinaryObject", "setbufferpointer", &conBinaryObjectSetBufferPointer, "Sets the buffer pointer", 3, 3);
		Con::addMethodS("BinaryObject", "getbufferlength", &conBinaryObjectGetBufferLength, "Returns the length of the buffer", 2, 2);
		Con::addMethodS("BinaryObject", "getbufferpointer", &conBinaryObjectGetBufferPointer, "Returns the buffer pointer", 2, 2);
		Con::addMethodB("BinaryObject", "close", &conBinaryObjectClose, "Closes the binary object", 2, 2);
		Con::addMethodS("NetConnection","getGhostIndex", &conGetGhostIndex, "Gets a ghost index for an object id", 3, 3);
		Con::addMethodB("NetConnection","forceUpdate", &conForceUpdate,"Forces an initial update for an object id", 3, 3);
		Con::addMethodS("NetConnection","resolveGhostParent",&conResolveGhostParent,"Resolves a ghost index parent", 3, 3);
		Con::addMethodS("NetConnection","resolveGhost",&conResolveGhost,"Resolves an object from a ghost ID for ServerConnection", 3, 3);
		Con::addMethodB(NULL,"clientCmdSetGhostTicks",&conclientCmdSetGhostTicks,"Client Command for disabling tick processing on ghost index",2,10);
		Con::addMethodB(NULL,"clientCmdsetProcessTicks",&conclientCmdSetProcessTicks,"Client Command for disabling tick processing on ghost object",2,10);
		Con::addMethodB(NULL,"clientCmdsetPosition",&conclientCmdSetPosition,"Client Command for setting transform on ghost object",2,10);
		// General
		Con::addMethodS(NULL, "sprintf", &conSprintf,"Formats a string. See the C sprintf.", 2, 20);
		Con::addMethodB(NULL, "tsExtensionUpdate", &conTSExtensionUpdate,"Updates the TSExtension.", 1, 1);
		Con::addMethodS(NULL, "getServPAddr",&congetServPAddr,"Gets the memPatch data for ServerProcess",1,1);



		// Add this Gvar to signify that TSExtension is active
		static bool is_active = true;
		Con::addVariable("$TSExtension::isActive", TypeBool, &is_active);
	}
}

