// dllmain.cpp : Defines the entry point for the DLL application.
#include <SDKDDKVer.h>
#include <Windows.h>
#define _USE_MATH_DEFINES
#include <math.h>
//#define TORNADO_ENABLE
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
static char test[100];
static DX::Move *mechchangedmove;
static char test2[100];
static void * moveptrmech;
unsigned int updatemoveretptr=0x5d2d7c;
float maxrot=2.9f;
float minrot=-2.9f;
static unsigned int playerptr=0x0;
static float newTurn = 0.1f;
static float turnStrength = 1.0f;

extern "C"
{
	static DX::AIMove aimoves[1024];
	static char command[256]="";
	static const char * dataBlock;
	static char buf[256]="";
//#define MECH_TURNING_SPEED 0.4
	
	// Maximum radians per 32ms tick
void *readIntptr=(void *)0x43BF10;
void *writeIntptr=(void *)0x43BF60;
void *writeStringptr=(void *)0x43C6D0;
void *readStringptr=(void *)0x43C630;

int streamReadInt(void * stream,int bitcount) 
{
	int retvalue=0x0;
	__asm {
		mov ecx,stream
		push bitcount
		call readIntptr
		mov retvalue,eax
	}
	return retvalue;
}

void streamWriteInt(void * stream,int value,int bitcount) {
	__asm {
		mov ecx,stream
		push bitcount
		push value
		call writeIntptr
	}
	return;
}

void streamWriteString(void * stream,const char * stringvar,int maxlen) {
	__asm {
		mov ecx,stream
		push maxlen
		push stringvar
		call writeStringptr
	}
	return;
}

void streamReadString(void * stream,char *stringvar) {
	__asm {
		mov ecx,stream
		push stringvar
		call readStringptr
	}
	return;
}

static char fieldnames[127][256];
static char fieldvalues[127][256];
static DX::Player *playervar;
static DX::SimObject *playerdatavar;
unsigned int gamebaseretptr=0x5E2A13;
unsigned int gboaparentptr=0x58c1e0;
static DX::GameBase *gb;
static DX::SimObject * gbdb;
static const char * gbfieldvalue;
void __declspec (naked)  GameBaseOnAddHook() {
	void * gamebaseptr;
	 __asm {
		push ebp
		mov ebp,esp
		push ebx
		push esi
		mov esi,ecx
		mov gamebaseptr,esi
		call [gboaparentptr]
		test al,al
		jz gboaparentfailed
		mov edx,[esi+0x248]
		test edx,edx
		jnz gboaworked
gboaparentfailed:
		lea esp,[ebp-0x8]
		xor al,al
		pop esi
		pop ebx
		pop ebp
		retn
gboaworked:
		pushad
	};
	gb = &DX::GameBase((unsigned int)gamebaseptr);
	gbdb = &DX::SimObject(gb->dataBlock);
	gbfieldvalue=gbdb->getFieldValue(DX::StringTableInsert("subClass",false));
	if (gbfieldvalue!=NULL) {
#ifdef TORNADO_ENABLE
		if (stricmp(gbfieldvalue,"tornado")==0) {
			
		}
#endif
	}
	__asm {
		popad
		jmp [gamebaseretptr]
	}

 }
void DBpackData(void *stream) {
	void* thisptr;
	__asm {
		mov thisptr,ecx
	};
	DX::SimObject dbobj = DX::SimObject((unsigned int)thisptr);
	char readcommand[256]="";
	sprintf_s<256>(readcommand,"return getWordCount(%d.extraNetFields);", dbobj.identifier);

	char str[256]="";
	strncpy_s<256>(str,Con::evaluate(readcommand,false,NULL,true), 256);

	Con::printf ("GWC returned %s\n",str);
	int realcount=atoi(str);
	int i=0;
	int counter=0;
	for (i; (i<126 && i<realcount); i++) {
		sprintf_s<256>(readcommand,"return getWord(%d.extraNetFields,%d);",dbobj.identifier,i);
		strncpy_s<256>(fieldnames[i],Con::evaluate(readcommand,false,NULL,true), 256);

		Con::printf (fieldnames[i]);
		_snprintf_s<256>(readcommand,256,"return(%d.%s);",dbobj.identifier,fieldnames[i]);
		strncpy_s<256>(fieldvalues[i],Con::evaluate(readcommand,false,NULL,true), 256);
		strcpy_s<256>(buf,dbobj.getFieldValue(DX::StringTableInsert(fieldnames[i],false)));
		Con::printf ("fieldvalues[i]:%s GDF: %s\n",fieldvalues[i],buf);

	}
	counter=i;
	Con::printf("Sending %d extra fields",counter);
	streamWriteInt(stream,counter,7);
	for (int x=0; x < counter; x++ ) {
		streamWriteString(stream,fieldnames[x],254);
		streamWriteString(stream,fieldvalues[x],254);
	}

}
void DBunpackData(void *stream) {
	void* thisptr;
	__asm {
		mov thisptr,ecx
	};

	char setcommand[256]="";
	DX::SimObject dbobj = DX::SimObject((unsigned int)thisptr);
	int counter=streamReadInt(stream,7);
	char buf[256]="";
	Con::printf ("Receiving %d extra fields",counter);
	for (int i=0; i < counter; i++) {
		streamReadString(stream,buf);
		strcpy_s<256>(fieldnames[i],buf);
		streamReadString(stream,buf);
		strcpy_s<256>(fieldvalues[i],buf);
	}
	for (int i=0; i < counter; i++ ){
		Con::printf (fieldnames[i]);
		Con::printf (fieldvalues[i]);
		dbobj.setDataField(DX::StringTableInsert(fieldnames[i],false),NULL,fieldvalues[i]);
		//_snprintf (setcommand,255,"%d.%s=\"%s\";",dbobj.identifier,fieldnames[i],fieldvalues[i]);
		//Con::evaluate(setcommand,false,NULL,true);
	}
}


float MECH_TURNING_SPEED=0.4f;
static unsigned int playerptr2;

__declspec(naked) void updateMoveHook()
{
    // this gets run from 0x5D2D6E

    __asm {
        mov playerptr,ebx
        mov eax,[ebp+0x8]
        mov moveptrmech,eax
		pusha
    };

	MECH_TURNING_SPEED=0.4f;

	if (playerptr!=0) {
		playervar=&DX::Player(playerptr);
		mechchangedmove=(DX::Move*) moveptrmech;
		//_snprintf (command,255,"return(%d.getDataBlock());",playervar->identifier);
		//strncpy(buf,Con::evaluate(command,false,NULL,true),255);
		if (playervar->dataBlock!=NULL) {

			playerdatavar = &DX::SimObject(playervar->dataBlock);
			//Con::printf("Datablock is %s\n",buf);
			if (playerdatavar->base_pointer_value!=0) 
			{
				strcpy_s<256>(buf,playerdatavar->getFieldValue(DX::StringTableInsert("mechControlEnabled",false)));
				Con::printf("mechControlEnabled: %s",buf);

				if ((mechchangedmove)->freelook && ((mechchangedmove)->y>0.0) && dAtob(buf))
				{
					//sprintf (command,"return (%d.getDataBlock().mechTurnSpeed);",playervar->identifier);
					strcpy_s<256>(buf,playerdatavar->getFieldValue(DX::StringTableInsert("mechTurnSpeed",false)));
					Con::printf("mechTurnSpeed: %s",buf);
					MECH_TURNING_SPEED=atof(buf);

					// FIXME: The 3 here should reference the datablock's maximum turning angle -- we're essentially normalizing our rotation here.
					float turnStrength = playervar->headRotationZ / 3;
					// Use whatever is leftover in our forward movement
					float forwardStrength = 1 - fabs(turnStrength);
					// Calculate a new turn value that we use for both the main body and the head.
					float newTurn = turnStrength * MECH_TURNING_SPEED;
					float newHeadTurn = turnStrength * (MECH_TURNING_SPEED/20);

					(mechchangedmove)->y = forwardStrength;
					(mechchangedmove)->x += turnStrength;

					// FIXME: Is the yaw value definitely in radians?
					playervar->mRotZ += newTurn + (mechchangedmove)->yaw;

					// Now, we must translate the turning strength into an appropriate subtraction for our
					// head rotation.
					playervar->headRotationZ += -newTurn;

					(mechchangedmove)->pitch = 0;
					(mechchangedmove)->yaw = 0;
					(mechchangedmove)->roll = 0;
					(mechchangedmove)->freelook = true;
				}
			}			
		}
	}
    __asm 
	{
		popa
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
		sprintf_s<120>(movecallback,"AIMoveCallback(%d);",aiconn->identifier);
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
		aimove->move.x=DX::clampFloat(std::stof(argv[2]));
		aimove->move.y=DX::clampFloat(std::stof(argv[3]));
		aimove->move.z=DX::clampFloat(std::stof(argv[4]));
		aimove->move.yaw=DX::clampMove(std::stof(argv[5]));
		aimove->move.pitch=DX::clampMove(std::stof(argv[6]));
		aimove->move.roll=DX::clampMove(std::stof(argv[7]));
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
	static unsigned int gboaptr =(unsigned int ) &GameBaseOnAddHook;
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
		Con::addMethodI("NetConnection","getGhostIndex", &conGetGhostIndex, "Gets a ghost index for an object id", 3, 3);
		Con::addMethodB("NetConnection","forceUpdate", &conForceUpdate,"Forces an initial update for an object id", 3, 3);
		Con::addMethodI("NetConnection","resolveGhostParent",&conResolveGhostParent,"Resolves a ghost index parent", 3, 3);
		Con::addMethodS(NULL,"floatToHex",&conFloatToHex,"converts float to hex",2,3);
		Con::addMethodI("NetConnection","resolveGhost",&conResolveGhost,"Resolves an object from a ghost ID for ServerConnection", 3, 3);
		Con::addMethodB(NULL,"clientCmdSetGhostTicks",&conclientCmdSetGhostTicks,"Client Command for disabling tick processing on ghost index",2,10);
		Con::addMethodB(NULL,"clientCmdsetProcessTicks",&conclientCmdSetProcessTicks,"Client Command for disabling tick processing on ghost object",2,10);
		// General
		Con::addMethodS(NULL, "sprintf", &conSprintf,"Formats a string. See the C sprintf.", 2, 20);
		Con::addMethodB(NULL, "tsExtensionUpdate", &conTSExtensionUpdate,"Updates the TSExtension.", 1, 1);
		// Regex
		Con::addMethodB(NULL, "reSearch", &reSearch,"reSearch(pattern, target): Searches for a pattern within the target string.", 3, 3);
		Con::addMethodB(NULL, "reMatch", &reMatch,"reMatch(pattern, pattern): Attempts to match the entire target string to a pattern.", 3, 3);
		Con::addMethodB(NULL, "reIterBegin", &reIterBegin,"reIterBegin(pattern, target): Begins an iterator search for patterns in the target string.", 3, 3);
		Con::addMethodB(NULL, "reIterEnd", &reIterEnd,"reIterEnd(): Returns true when the iterator search ends.", 1, 1);
		Con::addMethodS(NULL,"reIterNext",&reIterNext,"reIterNext(): Returns the next matched pattern in the string.", 1, 1);
		Con::addMethodS(NULL,"reReplace",&reReplace,"reReplace(pattern, target, replace): Replaces the pattern within the target string with another string.", 4, 4);

		// Add this Gvar to signify that TSExtension is active
		static bool is_active = true;


		Con::addVariable("$TSExtension::UberGravity", TypeF32, &movespeed);
		Con::addVariable("$TSExtension::UberId",TypeS32, &gravid);
		Con::addVariable("$TSExtension::isActive", TypeBool, &is_active);
		char mechcode[8]="\xA1\xAA\xAA\xAA\xAA\xFF\xE0";
		char dbrwcode[8]="\xC7\x42\x18\x80\x01\x00\x00";
		char dbpack[8]=  "\xB8\xAA\xAA\xAA\xAA\xFF\xE0";
		char dbunpack[8]="\xB8\xAA\xAA\xAA\xAA\xFF\xE0";
		char dbpatch3[8]="\xC7\x42\x18\x80\x01\x00\x00";
		char dbclient2[2]="\xEB";
		char dbclient[3]="\x90\x90";
		char gboaonadd[8]="FF\x25\xAA\xAA\xAA\xAA";
			//memPatch("42e05f",
		*((unsigned int*)(mechcode+1))=(unsigned int)&updatemovehookptr;
		*((unsigned int*)(dbpack+1))=(unsigned int)&DBpackData;
		*((unsigned int*)(dbunpack+1))=(unsigned int)&DBunpackData;
		*((unsigned int*)(gboaonadd+2))=(unsigned int)&gboaptr;
#ifdef NEW_DB_CODE
		DX::memPatch(0x5D2D6E,(unsigned char *)mechcode,7);
		DX::memPatch(0x438415,(unsigned char *)dbrwcode,7);
		DX::memPatch(0x436DF0,(unsigned char *)dbpack,7);
		DX::memPatch(0x436E00,(unsigned char *)dbunpack,7);
		DX::memPatch(0x42e05f,(unsigned char *)dbclient,2);
		DX::memPatch(0x66E1ED,(unsigned char *)dbclient2,1);
		DX::memPatch(0x438415,(unsigned char *)dbpatch3,7);
		DX::memPatch(0x5E29F0,(unsigned char *)gboaonadd,7);
#endif
	}
}
