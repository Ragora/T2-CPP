// dllmain.cpp : Defines the entry point for the DLL application.
#include <SDKDDKVer.h>
#include <Windows.h>
#define _USE_MATH_DEFINES
#include <math.h>
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
#include <DXAPI/Move.h>
#include <DXConCmds.h>
static DX::Move curmove;
static unsigned int tmpobjptr=0;
unsigned char movemem[500];
static DX::Move mechchangedmove;
static void * moveptrmech;
unsigned int updatemoveretptr=0x5d2d7c;
float maxrot=2.9;
float minrot=-2.9;
static unsigned int playerptr=0x0;
static float newTurn = 0.1;
static float turnStrength = 1.0;
extern "C"
{
	static DX::AIMove aimoves[1024];
	
#define MECH_TURNING_SPEED 0.4
	// Maximum radians per 32ms tick
__declspec(naked) void updateMoveHook()
{
    // this gets run from 0x5D2D6E

    __asm {
        mov playerptr,ebx
        mov eax,[ebp+0x8]
        mov moveptrmech,eax
    };

    unsigned playerptr2;
    DX::Player *playervar;
    playerptr2=playerptr;
    playervar=&(DX::Player(playerptr2));
    memcpy((void *)&mechchangedmove,(void *)(moveptrmech),sizeof(DX::Move));

    if (dAtob((playervar->CallMethod("isMechControlEnabled",0))) && mechchangedmove.freelook && (mechchangedmove.y>0.0))
    {
        mechchangedmove.pitch = 0;
        mechchangedmove.yaw = 0;
        mechchangedmove.roll = 0;
        mechchangedmove.freelook = true;

        // FIXME: The 3 here should reference the datablock's maximum turning angle -- we're essentially normalizing our rotation here.
        float turnStrength = playervar->headRotationZ / 3;
        // Use whatever is leftover in our forward movement
        float forwardStrength = 1 - fabs(turnStrength);
        // Calculate a new turn value that we use for both the main body and the head.
        float newTurn = turnStrength * MECH_TURNING_SPEED;
		float newHeadTurn = turnStrength * (MECH_TURNING_SPEED/20);

        mechchangedmove.y = forwardStrength;
        mechchangedmove.x = turnStrength;
        // FIXME: Is the yaw value definitely in radians?
		playervar->mRotZ += newTurn;

        // Now, we must translate the turning strength into an appropriate subtraction for our
        // head rotation.
        playervar->headRotationZ += -newTurn;

	}
    __asm {
        mov eax,offset mechchangedmove
        mov [ebp+8],eax
        mov ebx,playerptr
        mov eax,[ebp+8]
        mov edx,[eax]
        mov [ebx+0x894],edx
        mov eax,[eax+4]
        jmp [updatemoveretptr]
    };
}
	static unsigned int updatemovehookptr = (unsigned int)updateMoveHook;
	DX::AIMove * getAIMovePtr(unsigned int id) {
		int moveindex=0;
		bool foundindex=false;
		for (int x=0; x<1024; x++) {
			if (aimoves[x].id==id && aimoves[x].used==true) {
				moveindex=x;
				foundindex=true;
				break;
			}
			if (aimoves[x].used==false) {
				moveindex=x;
				break;
			}
		}
		if (foundindex==true) {
			return &aimoves[moveindex];
		} else {
			aimoves[moveindex].id=id;
			aimoves[moveindex].used=true;
			DX::generateNullMove(&(aimoves[moveindex].move));
			return &aimoves[moveindex];
		}
	}
	DX::Move tmpmove;
    __declspec(dllexport) void __cdecl newAIMoveListGenerator(DX::Move** moves, unsigned int * moveCount) {
		__asm {
			mov tmpobjptr,ecx
		}

		unsigned int * origobjptr;
		origobjptr=(unsigned int *) tmpobjptr;
		DX::AIMove * aimove;
		DX::GameConnection * aiconn;
		//Con::printf ("Possible offsets for ID starting at 0x4\n");
		if (origobjptr !=0 ) {
		//unsigned int * idptr;
		//unsigned int offset=0x4;
		//for (offset=0x4; offset<0x100; offset+=0x4) {
		
			//idptr=(unsigned int *)((* (origobjptr))+offset);
			//Con::printf ("Offset: %08X Addr: %08X Data: %d",offset, idptr, *idptr);
		
		//}
		aiconn = &DX::GameConnection((unsigned int)origobjptr+0xA0);
		aimove = getAIMovePtr(aiconn->identifier);
		char movecallback[120]="";
		sprintf (movecallback,"AIMoveCallback(%d);",aiconn->identifier);
		Con::evaluate(movecallback,false,NULL,NULL);
		//Con::printf ("BasePointer: %08X", aiconn.base_pointer_value);
		//Con::printf ("Ecx Value: %08X", origobjptr);
		//Con::printf("ID: %d\n",aiconn.identifier);
		//Con::evaluate ("listPlayers();",true,NULL,NULL);

		//Con::printf("orig: %08X   obj: %08X\n",origobjptr,0xBADABEEB);
		
			
			
			//Con::printf("Move processed for %08X\n",(aicon.identifier));
		}
		//memcpy (&tmpmove,&(aimove->move),sizeof(DX::Move));
		//DX::generateNullMove(&(aimove->move));
		*moves = &(aimove->move);
		*moveCount=1;
		return;


	}
	bool consetTrigger(Linker::SimObject *obj, S32 argc, const char *argv[]) {
		unsigned int aiconid = atoi(argv[1]);
		unsigned int index = atoi(argv[2]);
		if (index < 6) {
			DX::AIMove * aimove = getAIMovePtr(aiconid);
			bool value = dAtob(argv[3]);
			aimove->move.triggers[index]=value;
			return true;
		}
		return false;
	}
	bool consetMove(Linker::SimObject *obj, S32 argc, const char *argv[]) {
	// setMove(%aicon, x, y, z, yaw, pitch, roll);
		unsigned int aiconid = atoi(argv[1]);
		DX::AIMove * aimove = getAIMovePtr(aiconid);
		aimove->move.x=DX::clampFloat(atof(argv[2]));
		aimove->move.y=DX::clampFloat(atof(argv[3]));
		aimove->move.z=DX::clampFloat(atof(argv[4]));
		aimove->move.yaw=DX::clampMove(atof(argv[5]));
		aimove->move.pitch=DX::clampMove(atof(argv[6]));
		aimove->move.roll=DX::clampMove(atof(argv[7]));
		//Con::printf ("Set move variables for %d to x:%f y:%f z:%f yaw:%f pitch:%f roll:%f\n",aimove->id,aimove->move.x,aimove->move.y,aimove->move.z,aimove->move.yaw,aimove->move.pitch,aimove->move.roll);
		return true;
	}


	bool conEnableNewAI(Linker::SimObject *obj, S32 argc, const char *argv[]) 
	{

		(*((unsigned int *)0x75e360))=(unsigned int)newAIMoveListGenerator;
		return true;
	}

	static S32 gravid=0;
	static float movespeed=0.0;
	__declspec(dllexport) void ServerProcess(unsigned int deltaTime)
	{
		//memPatch("602D1E","9090");
		float pos[3];
		float rot[4];
		if (gravid!=0) {
			if (movespeed != 0.0) {
				float timeinseconds=(deltaTime/1000.0f);
				void * objptr = Sim::findObject(gravid);
				if ((unsigned int)(objptr)) {
					DX::SceneObject newobj=DX::SceneObject((unsigned int)objptr);
					newobj.getPosition(pos);
					newobj.getRotation(rot);
					pos[2]+=(movespeed*timeinseconds);
					newobj.setPosition(pos);
				}
			}

		}
	}
	__declspec(dllexport) void ModInitialize(void)
	{
		// Init WSA
		WSADATA wsadata;
		WSAStartup(0x0202, &wsadata);
		Con::addMethodS(NULL,"dumpHex",&conDumpHex,"dumpHex(addr,size,spaces)",4,5);
		Con::addMethodS(NULL,"dumpDec",&conDumpUInt,"dumpDec(addr)",2,3);
		Con::addMethodS(NULL,"dumpFloat",&conDumpFloat,"dumpFloat(addr)",2,3);
		Con::addMethodB("Player", "isjumping", &conPlayerGetJumpingState,"Returns whether or not the player is jumping", 2, 2);
		Con::addMethodB("Player", "isjetting", &conPlayerGetJettingState,"Returns whether or not the player is jetting", 2, 2);
		Con::addMethodB("GameConnection", "setheat", &conGameConnectionSetHeatLevel,"Sets the heat level", 3, 3);
		Con::addMethodB("ShapeBase","setcloakvalue",&conShapeBaseSetCloakValue,"Sets the cloak value ex: setcloakvalue(1,0.5)",3,4);
		Con::addMethodB("GrenadeProjectile", "explode", &conProjectileExplode,"Explodes the given projectile", 5, 5);
		Con::addMethodB("GameBase","setProcessTicks",&conSetProcessTicks,"Sets the flag for processing ticks or not", 3, 3);
		Con::addMethodB("Projectile", "explode", &conProjectileExplode,"Explodes the given projectile", 5, 5);
		Con::addMethodB(NULL,"enableNewAI",&conEnableNewAI,"Enables the new Move Generation code for the AI", 1,4);
		Con::addMethodB(NULL,"setAIMove",&consetMove,"setAIMove(%aicon, x, y, z, yaw, pitch, roll)", 2,10);
		Con::addMethodB(NULL,"setAITrigger", &consetTrigger, "setAITrigger(%aicon,triggerid,value);",2,5);
		Con::addMethodS("GrenadeProjectile", "getposition", &conGrenadeProjectileGetPosition,"Accurately gets the position of the GrenadeProjectile", 2, 2);
		Con::addMethodS("GrenadeProjectile", "getvelocity", &conGrenadeProjectileGetVelocity,"Gets the velocity of the GrenadeProjectile", 2, 2);
		Con::addMethodB("Projectile", "makeNerf", &conProjectileMakeNerf,"Makes the Projectile deal no damage", 2, 2);

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
		// General
		Con::addMethodS(NULL, "sprintf", &conSprintf,"Formats a string. See the C sprintf.", 2, 20);
		Con::addMethodB(NULL, "tsExtensionUpdate", &conTSExtensionUpdate,"Updates the TSExtension.", 1, 1);


		// Add this Gvar to signify that TSExtension is active
		static bool is_active = true;


		Con::addVariable("$TSExtension::UberGravity", TypeF32, &movespeed);
		Con::addVariable("$TSExtension::UberId",TypeS32, &gravid);
		Con::addVariable("$TSExtension::isActive", TypeBool, &is_active);
		char mechcode[8]="\xA1\xAA\xAA\xAA\xAA\xFF\xE0";

		*((unsigned int*)(mechcode+1))=(unsigned int)&updatemovehookptr;
		DX::memPatch(0x5D2D6E,(unsigned char *)mechcode,7);
	}
}

