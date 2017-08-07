// dllmain.cpp : Defines the entry point for the DLL application.

#include <SDKDDKVer.h>
#include <Windows.h>
#include <string>

#include <DXAPI/DXAPI.h>
#include <LinkerAPI.h>

#include <Python.h>


static int aienabled=0;
unsigned int oldaiaddr=0x0;
static unsigned int tmpobjptr=0;
static DX::Move curmove;


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

float MECH_TURNING_SPEED=0.4f;
static unsigned int playerptr2;

extern "C"
{
	static DX::AIMove aimoves[1024];
}

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

/* Return the number of arguments of the application command line */
static PyObject*
ts_tseval(PyObject *self, PyObject *args)
{	
	char *input;
    if(!PyArg_ParseTuple(args, "s:tseval",&input))
        return NULL;
	std::string inputstr=std::string(input);
	//Con::printf(inputstr.c_str());
	return Py_BuildValue("s",Con::evaluate(inputstr.c_str(),false,NULL,false));
}


static PyMethodDef TSMethods[] = {
    {"tseval", ts_tseval, METH_VARARGS,
     "Evaluates TorqueScript."},
    {NULL, NULL, 0, NULL}
};

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

static unsigned int updatemovehookptr = (unsigned int)updateMoveHook;

DX::Move tmpmove;

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

newAIMoveListGeneratorPtr gen;


typedef void (__thiscall *newAIMoveListGeneratorPtr)(void*, DX::Move** moves, unsigned int * moveCount);

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


bool conDisableNewAI(Linker::SimObject *obj, S32 argc, const char *argv[])
	{
		if (aienabled==1) {
			Py_Finalize();
			(*((unsigned int *)0x75e360))=(unsigned int)oldaiaddr;
			aienabled=0;
			return true;
		}
		return false;
	}
	bool conEnableNewAI(Linker::SimObject *obj, S32 argc, const char *argv[])
	{
		if (aienabled==0) {
			Py_Initialize();
			Py_InitModule("ts", TSMethods);
			oldaiaddr=(*((unsigned int *)0x75e360));
			gen=(newAIMoveListGeneratorPtr)newAIMoveListGenerator;
			(*((unsigned int *)0x75e360))=(unsigned int)(gen);
			aienabled=1;
			return true;
		}
		return false;
	}

__declspec(dllexport) void __fastcall newAIMoveListGenerator(void* This, void* notUsed, DX::Move** moves, unsigned int* moveCount) {
	tmpobjptr=(unsigned int)This;

	unsigned int * origobjptr;
	origobjptr=(unsigned int *) tmpobjptr;
	DX::AIMove * aimove;
	DX::GameConnection * aiconn;

	//Con::printf ("Possible offsets for ID starting at 0x4\n");
	if (origobjptr !=0 ) 
	{
		//unsigned int * idptr;
		//unsigned int offset=0x4;
		//for (offset=0x4; offset<0x100; offset+=0x4) {

			//idptr=(unsigned int *)((* (origobjptr))+offset);
			//Con::printf ("Offset: %08X Addr: %08X Data: %d",offset, idptr, *idptr);

		//}
		aiconn = &DX::GameConnection((unsigned int)origobjptr+0xA0);
		aimove = getAIMovePtr(aiconn->identifier);
		unsigned int ident=aiconn->identifier;
		printf("%d\n",ident);
		char movecallback[120]="";
		sprintf_s<120>(movecallback,"AIMoveCallback(%d)\n",ident);
		//FILE *f=fopen("AI.py","rb");
		PyRun_SimpleString((std::string("import ts\nfrom ai import AIMoveCallback\nimport ai\n")+std::string(movecallback)).c_str());
		//Con::evaluate(movecallback,false,NULL,NULL);
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

__declspec(dllexport) void ModInitialize(void)
{
	Con::addMethodB(NULL,"enableNewAI",&conEnableNewAI,"Enables the new Move Generation code for the AI", 1,4);
	Con::addMethodB(NULL,"disableNewAI",&conDisableNewAI,"Disables the new Move Generation code for the AI", 1,4);

	Py_SetProgramName("AIInterpreter");
}
	
__declspec(dllexport) void ServerProcess(unsigned int deltaTime)
{
			
}


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
