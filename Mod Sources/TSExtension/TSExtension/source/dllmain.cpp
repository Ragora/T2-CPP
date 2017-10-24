// dllmain.cpp : Defines the entry point for the DLL application.
#include <SDKDDKVer.h>
#include <Windows.h>
#define _USE_MATH_DEFINES
#include <math.h>

//#define TORNADO_ENABLE
//#define MECH_MOVE_CODE
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
}




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

#define TSEXTENSION_API __declspec( dllexport )
#include <ModLoader\ModLoader.h>




const char* conGuiTsCtrlProject(Linker::SimObject *obj,S32 argc, const char* argv[]) {
	Linker::Point3F pt;
	Linker::Point3F rt;
	dSscanf(argv[2],"%g %g %g",&pt.x,&pt.y,&pt.z);
	GuiTSCtrl_project(reinterpret_cast<GuiTSCtrl *>(obj),pt,&rt);
	
	char* buffer = Con::getReturnBuffer(255);
	dSprintf(buffer,255,"%g %g %g",rt.x,rt.y,rt.z);

	return buffer;
}

static bool dbpatched=0;
bool enableDBPatch(Linker::SimObject* obj, S32 argc, const char* argv[])
{
	if (dbpatched==1) {
		Con::errorf(0, "Danger, DB Patched already");
		return 0;
	}
	dbpatched=1;
	Con::errorf(0, "Danger, DB Patch Enabling");
	unsigned int addrbase=(unsigned int)VirtualAlloc(NULL,0x20000,MEM_COMMIT, PAGE_READWRITE);
	printf("%08X\n",addrbase);
	DWORD oldprotect=0;
	VirtualProtect((void *)addrbase,0x20000,PAGE_EXECUTE_READWRITE,&oldprotect);
	unsigned int addr=addrbase+0x10100;
	unsigned char addr0=addr&0xFF;
	unsigned char addr1=(addr>>8)&0xFF;
	unsigned char addr2=(addr>>16)&0xFF;
	unsigned char addr3=(addr>>24)&0xFF;
	unsigned int addrbase2=addrbase+0x0;
	unsigned char addr4=addrbase2&0xFF;
	unsigned char addr5=(addrbase2>>8)&0xFF;
	unsigned char addr6=(addrbase2>>16)&0xFF;
	unsigned char addr7=(addrbase2>>24)&0xFF;
	printf("%d\n",GetLastError());
unsigned char patch0040C627=addr0;
DX::memPatch(0x0040C627, &patch0040C627,1);
unsigned char patch0040C628=addr1;
DX::memPatch(0x0040C628, &patch0040C628,1);
unsigned char patch0040C629=addr2;
DX::memPatch(0x0040C629, &patch0040C629,1);
unsigned char patch0040C62A=addr3;
DX::memPatch(0x0040C62A, &patch0040C62A,1);
unsigned char patch0040C65A=addr0;
DX::memPatch(0x0040C65A, &patch0040C65A,1);
unsigned char patch0040C65B=addr1;
DX::memPatch(0x0040C65B, &patch0040C65B,1);
unsigned char patch0040C65C=addr2;
DX::memPatch(0x0040C65C, &patch0040C65C,1);
unsigned char patch0040C65D=addr3;
DX::memPatch(0x0040C65D, &patch0040C65D,1);
unsigned char patch0040C68D=addr0;
DX::memPatch(0x0040C68D, &patch0040C68D,1);
unsigned char patch0040C68E=addr1;
DX::memPatch(0x0040C68E, &patch0040C68E,1);
unsigned char patch0040C68F=addr2;
DX::memPatch(0x0040C68F, &patch0040C68F,1);
unsigned char patch0040C690=addr3;
DX::memPatch(0x0040C690, &patch0040C690,1);
unsigned char patch0042D157=addr4;
DX::memPatch(0x0042D157, &patch0042D157,1);
unsigned char patch0042D158=addr5;
DX::memPatch(0x0042D158, &patch0042D158,1);
unsigned char patch0042D159=addr6;
DX::memPatch(0x0042D159, &patch0042D159,1);
unsigned char patch0042D15A=addr7;
DX::memPatch(0x0042D15A, &patch0042D15A,1);
unsigned char patch00436CEC=0x28;
DX::memPatch(0x00436CEC, &patch00436CEC,1);
unsigned char patch00436D15=0x28;
DX::memPatch(0x00436D15, &patch00436D15,1);
unsigned char patch00436E22=addr0;
DX::memPatch(0x00436E22, &patch00436E22,1);
unsigned char patch00436E23=addr1;
DX::memPatch(0x00436E23, &patch00436E23,1);
unsigned char patch00436E24=addr2;
DX::memPatch(0x00436E24, &patch00436E24,1);
unsigned char patch00436E25=addr3;
DX::memPatch(0x00436E25, &patch00436E25,1);
unsigned char patch00436E30=addr4;
DX::memPatch(0x00436E30, &patch00436E30,1);
unsigned char patch00436E31=addr5;
DX::memPatch(0x00436E31, &patch00436E31,1);
unsigned char patch00436E32=addr6;
DX::memPatch(0x00436E32, &patch00436E32,1);
unsigned char patch00436E33=addr7;
DX::memPatch(0x00436E33, &patch00436E33,1);
unsigned char patch00436E78=0xa0;
DX::memPatch(0x00436E78, &patch00436E78,1);
unsigned char patch00436E86=0xa0;
DX::memPatch(0x00436E86, &patch00436E86,1);
unsigned char patch00436E96=0xa0;
DX::memPatch(0x00436E96, &patch00436E96,1);
unsigned char patch00436EB8=0xa0;
DX::memPatch(0x00436EB8, &patch00436EB8,1);
unsigned char patch00436EC9=0xa0;
DX::memPatch(0x00436EC9, &patch00436EC9,1);
unsigned char patch00436EE3=0xa0;
DX::memPatch(0x00436EE3, &patch00436EE3,1);
unsigned char patch00436EF1=0xa0;
DX::memPatch(0x00436EF1, &patch00436EF1,1);
unsigned char patch00436EFD=0xa0;
DX::memPatch(0x00436EFD, &patch00436EFD,1);
unsigned char patch00436F53=0x27;
DX::memPatch(0x00436F53, &patch00436F53,1);
unsigned char patch00436F5B=0xa0;
DX::memPatch(0x00436F5B, &patch00436F5B,1);
unsigned char patch0043843D=addr0;
DX::memPatch(0x0043843D, &patch0043843D,1);
unsigned char patch0043843E=addr1;
DX::memPatch(0x0043843E, &patch0043843E,1);
unsigned char patch0043843F=addr2;
DX::memPatch(0x0043843F, &patch0043843F,1);
unsigned char patch00438440=addr3;
DX::memPatch(0x00438440, &patch00438440,1);
unsigned char patch0043845C=addr4;
DX::memPatch(0x0043845C, &patch0043845C,1);
unsigned char patch0043845D=addr5;
DX::memPatch(0x0043845D, &patch0043845D,1);
unsigned char patch0043845E=addr6;
DX::memPatch(0x0043845E, &patch0043845E,1);
unsigned char patch0043845F=addr7;
DX::memPatch(0x0043845F, &patch0043845F,1);
unsigned char patch004405A1=addr0;
DX::memPatch(0x004405A1, &patch004405A1,1);
unsigned char patch004405A2=addr1;
DX::memPatch(0x004405A2, &patch004405A2,1);
unsigned char patch004405A3=addr2;
DX::memPatch(0x004405A3, &patch004405A3,1);
unsigned char patch004405A4=addr3;
DX::memPatch(0x004405A4, &patch004405A4,1);
unsigned char patch00524703=addr0;
DX::memPatch(0x00524703, &patch00524703,1);
unsigned char patch00524704=addr1;
DX::memPatch(0x00524704, &patch00524704,1);
unsigned char patch00524705=addr2;
DX::memPatch(0x00524705, &patch00524705,1);
unsigned char patch00524706=addr3;
DX::memPatch(0x00524706, &patch00524706,1);
unsigned char patch00524781=addr0;
DX::memPatch(0x00524781, &patch00524781,1);
unsigned char patch00524782=addr1;
DX::memPatch(0x00524782, &patch00524782,1);
unsigned char patch00524783=addr2;
DX::memPatch(0x00524783, &patch00524783,1);
unsigned char patch00524784=addr3;
DX::memPatch(0x00524784, &patch00524784,1);
unsigned char patch00524B4F=addr0;
DX::memPatch(0x00524B4F, &patch00524B4F,1);
unsigned char patch00524B50=addr1;
DX::memPatch(0x00524B50, &patch00524B50,1);
unsigned char patch00524B51=addr2;
DX::memPatch(0x00524B51, &patch00524B51,1);
unsigned char patch00524B52=addr3;
DX::memPatch(0x00524B52, &patch00524B52,1);
unsigned char patch00524BCF=addr0;
DX::memPatch(0x00524BCF, &patch00524BCF,1);
unsigned char patch00524BD0=addr1;
DX::memPatch(0x00524BD0, &patch00524BD0,1);
unsigned char patch00524BD1=addr2;
DX::memPatch(0x00524BD1, &patch00524BD1,1);
unsigned char patch00524BD2=addr3;
DX::memPatch(0x00524BD2, &patch00524BD2,1);
unsigned char patch00583FB4=0x20;
DX::memPatch(0x00583FB4, &patch00583FB4,1);
unsigned char patch00584218=0x20;
DX::memPatch(0x00584218, &patch00584218,1);
unsigned char patch00584511=0xff;
DX::memPatch(0x00584511, &patch00584511,1);
unsigned char patch00584512=0x3f;
DX::memPatch(0x00584512, &patch00584512,1);
unsigned char patch0058463C=0xff;
DX::memPatch(0x0058463C, &patch0058463C,1);
unsigned char patch0058463D=0x3f;
DX::memPatch(0x0058463D, &patch0058463D,1);
unsigned char patch00584681=0xff;
DX::memPatch(0x00584681, &patch00584681,1);
unsigned char patch00584682=0x3f;
DX::memPatch(0x00584682, &patch00584682,1);
unsigned char patch005846EF=0xff;
DX::memPatch(0x005846EF, &patch005846EF,1);
unsigned char patch005846F0=0x3f;
DX::memPatch(0x005846F0, &patch005846F0,1);
unsigned char patch00584720=0x0;
DX::memPatch(0x00584720, &patch00584720,1);
unsigned char patch00584721=0x40;
DX::memPatch(0x00584721, &patch00584721,1);
unsigned char patch00584943=0x0;
DX::memPatch(0x00584943, &patch00584943,1);
unsigned char patch00584944=0x40;
DX::memPatch(0x00584944, &patch00584944,1);
unsigned char patch00584A23=0x0;
DX::memPatch(0x00584A23, &patch00584A23,1);
unsigned char patch00584A24=0x40;
DX::memPatch(0x00584A24, &patch00584A24,1);
unsigned char patch00584B6B=0x0;
DX::memPatch(0x00584B6B, &patch00584B6B,1);
unsigned char patch00584B6C=0x40;
DX::memPatch(0x00584B6C, &patch00584B6C,1);
unsigned char patch00584BA2=0x0;
DX::memPatch(0x00584BA2, &patch00584BA2,1);
unsigned char patch00584BA3=0x40;
DX::memPatch(0x00584BA3, &patch00584BA3,1);
unsigned char patch00584BCD=0x0;
DX::memPatch(0x00584BCD, &patch00584BCD,1);
unsigned char patch00584BCE=0x40;
DX::memPatch(0x00584BCE, &patch00584BCE,1);
unsigned char patch00584E45=0x0;
DX::memPatch(0x00584E45, &patch00584E45,1);
unsigned char patch00584E46=0x40;
DX::memPatch(0x00584E46, &patch00584E46,1);
unsigned char patch00584FD1=0xff;
DX::memPatch(0x00584FD1, &patch00584FD1,1);
unsigned char patch00584FD2=0x3f;
DX::memPatch(0x00584FD2, &patch00584FD2,1);
unsigned char patch00585431=0x20;
DX::memPatch(0x00585431, &patch00585431,1);
unsigned char patch00585491=0x20;
DX::memPatch(0x00585491, &patch00585491,1);
unsigned char patch005854ED=0x20;
DX::memPatch(0x005854ED, &patch005854ED,1);
unsigned char patch00585694=0x20;
DX::memPatch(0x00585694, &patch00585694,1);
unsigned char patch005856E4=0x20;
DX::memPatch(0x005856E4, &patch005856E4,1);
unsigned char patch00585730=0x20;
DX::memPatch(0x00585730, &patch00585730,1);
unsigned char patch00586662=0x0;
DX::memPatch(0x00586662, &patch00586662,1);
unsigned char patch00586665=0x40;
DX::memPatch(0x00586665, &patch00586665,1);
unsigned char patch00586675=0x0;
DX::memPatch(0x00586675, &patch00586675,1);
unsigned char patch00586678=0x40;
DX::memPatch(0x00586678, &patch00586678,1);
unsigned char patch00586688=0x0;
DX::memPatch(0x00586688, &patch00586688,1);
unsigned char patch0058668B=0x40;
DX::memPatch(0x0058668B, &patch0058668B,1);
unsigned char patch0058669B=0x0;
DX::memPatch(0x0058669B, &patch0058669B,1);
unsigned char patch0058669E=0x40;
DX::memPatch(0x0058669E, &patch0058669E,1);
unsigned char patch00586A30=0x0;
DX::memPatch(0x00586A30, &patch00586A30,1);
unsigned char patch00586A32=0x1;
DX::memPatch(0x00586A32, &patch00586A32,1);
unsigned char patch00586A50=0x0;
DX::memPatch(0x00586A50, &patch00586A50,1);
unsigned char patch00586A52=0xc;
DX::memPatch(0x00586A52, &patch00586A52,1);
unsigned char patch00586C1C=0x0;
DX::memPatch(0x00586C1C, &patch00586C1C,1);
unsigned char patch00586C1D=0x40;
DX::memPatch(0x00586C1D, &patch00586C1D,1);
unsigned char patch00586C3F=0x0;
DX::memPatch(0x00586C3F, &patch00586C3F,1);
unsigned char patch00586C41=0x1;
DX::memPatch(0x00586C41, &patch00586C41,1);
unsigned char patch00586CE1=0x0;
DX::memPatch(0x00586CE1, &patch00586CE1,1);
unsigned char patch00586CE2=0x40;
DX::memPatch(0x00586CE2, &patch00586CE2,1);
unsigned char patch00586D33=0x0;
DX::memPatch(0x00586D33, &patch00586D33,1);
unsigned char patch00586D35=0x1;
DX::memPatch(0x00586D35, &patch00586D35,1);
unsigned char patch00586DDC=0x0;
DX::memPatch(0x00586DDC, &patch00586DDC,1);
unsigned char patch00586DDD=0x40;
DX::memPatch(0x00586DDD, &patch00586DDD,1);
unsigned char patch00587699=0x16;
DX::memPatch(0x00587699, &patch00587699,1);
unsigned char patch005876A8=0x16;
DX::memPatch(0x005876A8, &patch005876A8,1);
unsigned char patch005876ED=0x16;
DX::memPatch(0x005876ED, &patch005876ED,1);
unsigned char patch005876F8=0x16;
DX::memPatch(0x005876F8, &patch005876F8,1);
unsigned char patch005878CF=0x16;
DX::memPatch(0x005878CF, &patch005878CF,1);
unsigned char patch005878DF=0x16;
DX::memPatch(0x005878DF, &patch005878DF,1);
unsigned char patch00587908=0x16;
DX::memPatch(0x00587908, &patch00587908,1);
unsigned char patch00587918=0x16;
DX::memPatch(0x00587918, &patch00587918,1);
unsigned char patch005B542E=addr0;
DX::memPatch(0x005B542E, &patch005B542E,1);
unsigned char patch005B542F=addr1;
DX::memPatch(0x005B542F, &patch005B542F,1);
unsigned char patch005B5430=addr2;
DX::memPatch(0x005B5430, &patch005B5430,1);
unsigned char patch005B5431=addr3;
DX::memPatch(0x005B5431, &patch005B5431,1);
unsigned char patch005C2F6D=0x20;
DX::memPatch(0x005C2F6D, &patch005C2F6D,1);
unsigned char patch005C2F6E=0x4e;
DX::memPatch(0x005C2F6E, &patch005C2F6E,1);
unsigned char patch005CC4FE=0x20;
DX::memPatch(0x005CC4FE, &patch005CC4FE,1);
unsigned char patch005CC67B=0x20;
DX::memPatch(0x005CC67B, &patch005CC67B,1);
unsigned char patch005CDE35=addr0;
DX::memPatch(0x005CDE35, &patch005CDE35,1);
unsigned char patch005CDE36=addr1;
DX::memPatch(0x005CDE36, &patch005CDE36,1);
unsigned char patch005CDE37=addr2;
DX::memPatch(0x005CDE37, &patch005CDE37,1);
unsigned char patch005CDE38=addr3;
DX::memPatch(0x005CDE38, &patch005CDE38,1);
unsigned char patch005CDE77=addr0;
DX::memPatch(0x005CDE77, &patch005CDE77,1);
unsigned char patch005CDE78=addr1;
DX::memPatch(0x005CDE78, &patch005CDE78,1);
unsigned char patch005CDE79=addr2;
DX::memPatch(0x005CDE79, &patch005CDE79,1);
unsigned char patch005CDE7A=addr3;
DX::memPatch(0x005CDE7A, &patch005CDE7A,1);
unsigned char patch005CDEBA=addr0;
DX::memPatch(0x005CDEBA, &patch005CDEBA,1);
unsigned char patch005CDEBB=addr1;
DX::memPatch(0x005CDEBB, &patch005CDEBB,1);
unsigned char patch005CDEBC=addr2;
DX::memPatch(0x005CDEBC, &patch005CDEBC,1);
unsigned char patch005CDEBD=addr3;
DX::memPatch(0x005CDEBD, &patch005CDEBD,1);
unsigned char patch005CE464=addr0;
DX::memPatch(0x005CE464, &patch005CE464,1);
unsigned char patch005CE465=addr1;
DX::memPatch(0x005CE465, &patch005CE465,1);
unsigned char patch005CE466=addr2;
DX::memPatch(0x005CE466, &patch005CE466,1);
unsigned char patch005CE467=addr3;
DX::memPatch(0x005CE467, &patch005CE467,1);
unsigned char patch005CE4BE=addr0;
DX::memPatch(0x005CE4BE, &patch005CE4BE,1);
unsigned char patch005CE4BF=addr1;
DX::memPatch(0x005CE4BF, &patch005CE4BF,1);
unsigned char patch005CE4C0=addr2;
DX::memPatch(0x005CE4C0, &patch005CE4C0,1);
unsigned char patch005CE4C1=addr3;
DX::memPatch(0x005CE4C1, &patch005CE4C1,1);
unsigned char patch005CE516=addr0;
DX::memPatch(0x005CE516, &patch005CE516,1);
unsigned char patch005CE517=addr1;
DX::memPatch(0x005CE517, &patch005CE517,1);
unsigned char patch005CE518=addr2;
DX::memPatch(0x005CE518, &patch005CE518,1);
unsigned char patch005CE519=addr3;
DX::memPatch(0x005CE519, &patch005CE519,1);
unsigned char patch005CE56E=addr0;
DX::memPatch(0x005CE56E, &patch005CE56E,1);
unsigned char patch005CE56F=addr1;
DX::memPatch(0x005CE56F, &patch005CE56F,1);
unsigned char patch005CE570=addr2;
DX::memPatch(0x005CE570, &patch005CE570,1);
unsigned char patch005CE571=addr3;
DX::memPatch(0x005CE571, &patch005CE571,1);
unsigned char patch005CE5CA=addr0;
DX::memPatch(0x005CE5CA, &patch005CE5CA,1);
unsigned char patch005CE5CB=addr1;
DX::memPatch(0x005CE5CB, &patch005CE5CB,1);
unsigned char patch005CE5CC=addr2;
DX::memPatch(0x005CE5CC, &patch005CE5CC,1);
unsigned char patch005CE5CD=addr3;
DX::memPatch(0x005CE5CD, &patch005CE5CD,1);
unsigned char patch005DAAC2=0x20;
DX::memPatch(0x005DAAC2, &patch005DAAC2,1);
unsigned char patch005DADBE=0x20;
DX::memPatch(0x005DADBE, &patch005DADBE,1);
unsigned char patch005E33AE=addr0;
DX::memPatch(0x005E33AE, &patch005E33AE,1);
unsigned char patch005E33AF=addr1;
DX::memPatch(0x005E33AF, &patch005E33AF,1);
unsigned char patch005E33B0=addr2;
DX::memPatch(0x005E33B0, &patch005E33B0,1);
unsigned char patch005E33B1=addr3;
DX::memPatch(0x005E33B1, &patch005E33B1,1);
unsigned char patch005E42EF=addr0;
DX::memPatch(0x005E42EF, &patch005E42EF,1);
unsigned char patch005E42F0=addr1;
DX::memPatch(0x005E42F0, &patch005E42F0,1);
unsigned char patch005E42F1=addr2;
DX::memPatch(0x005E42F1, &patch005E42F1,1);
unsigned char patch005E42F2=addr3;
DX::memPatch(0x005E42F2, &patch005E42F2,1);
unsigned char patch005E4338=addr0;
DX::memPatch(0x005E4338, &patch005E4338,1);
unsigned char patch005E4339=addr1;
DX::memPatch(0x005E4339, &patch005E4339,1);
unsigned char patch005E433A=addr2;
DX::memPatch(0x005E433A, &patch005E433A,1);
unsigned char patch005E433B=addr3;
DX::memPatch(0x005E433B, &patch005E433B,1);
unsigned char patch005E4381=addr0;
DX::memPatch(0x005E4381, &patch005E4381,1);
unsigned char patch005E4382=addr1;
DX::memPatch(0x005E4382, &patch005E4382,1);
unsigned char patch005E4383=addr2;
DX::memPatch(0x005E4383, &patch005E4383,1);
unsigned char patch005E4384=addr3;
DX::memPatch(0x005E4384, &patch005E4384,1);
unsigned char patch005E43CA=addr0;
DX::memPatch(0x005E43CA, &patch005E43CA,1);
unsigned char patch005E43CB=addr1;
DX::memPatch(0x005E43CB, &patch005E43CB,1);
unsigned char patch005E43CC=addr2;
DX::memPatch(0x005E43CC, &patch005E43CC,1);
unsigned char patch005E43CD=addr3;
DX::memPatch(0x005E43CD, &patch005E43CD,1);
unsigned char patch005EBD75=addr0;
DX::memPatch(0x005EBD75, &patch005EBD75,1);
unsigned char patch005EBD76=addr1;
DX::memPatch(0x005EBD76, &patch005EBD76,1);
unsigned char patch005EBD77=addr2;
DX::memPatch(0x005EBD77, &patch005EBD77,1);
unsigned char patch005EBD78=addr3;
DX::memPatch(0x005EBD78, &patch005EBD78,1);
unsigned char patch005EF082=0x20;
DX::memPatch(0x005EF082, &patch005EF082,1);
unsigned char patch005EF5C1=addr0;
DX::memPatch(0x005EF5C1, &patch005EF5C1,1);
unsigned char patch005EF5C2=addr1;
DX::memPatch(0x005EF5C2, &patch005EF5C2,1);
unsigned char patch005EF5C3=addr2;
DX::memPatch(0x005EF5C3, &patch005EF5C3,1);
unsigned char patch005EF5C4=addr3;
DX::memPatch(0x005EF5C4, &patch005EF5C4,1);
unsigned char patch005EFC9B=0x20;
DX::memPatch(0x005EFC9B, &patch005EFC9B,1);
unsigned char patch005F4605=addr0;
DX::memPatch(0x005F4605, &patch005F4605,1);
unsigned char patch005F4606=addr1;
DX::memPatch(0x005F4606, &patch005F4606,1);
unsigned char patch005F4607=addr2;
DX::memPatch(0x005F4607, &patch005F4607,1);
unsigned char patch005F4608=addr3;
DX::memPatch(0x005F4608, &patch005F4608,1);
unsigned char patch005F4655=addr0;
DX::memPatch(0x005F4655, &patch005F4655,1);
unsigned char patch005F4656=addr1;
DX::memPatch(0x005F4656, &patch005F4656,1);
unsigned char patch005F4657=addr2;
DX::memPatch(0x005F4657, &patch005F4657,1);
unsigned char patch005F4658=addr3;
DX::memPatch(0x005F4658, &patch005F4658,1);
unsigned char patch005F46B7=addr0;
DX::memPatch(0x005F46B7, &patch005F46B7,1);
unsigned char patch005F46B8=addr1;
DX::memPatch(0x005F46B8, &patch005F46B8,1);
unsigned char patch005F46B9=addr2;
DX::memPatch(0x005F46B9, &patch005F46B9,1);
unsigned char patch005F46BA=addr3;
DX::memPatch(0x005F46BA, &patch005F46BA,1);
unsigned char patch005F470B=addr0;
DX::memPatch(0x005F470B, &patch005F470B,1);
unsigned char patch005F470C=addr1;
DX::memPatch(0x005F470C, &patch005F470C,1);
unsigned char patch005F470D=addr2;
DX::memPatch(0x005F470D, &patch005F470D,1);
unsigned char patch005F470E=addr3;
DX::memPatch(0x005F470E, &patch005F470E,1);
unsigned char patch005F4C75=addr0;
DX::memPatch(0x005F4C75, &patch005F4C75,1);
unsigned char patch005F4C76=addr1;
DX::memPatch(0x005F4C76, &patch005F4C76,1);
unsigned char patch005F4C77=addr2;
DX::memPatch(0x005F4C77, &patch005F4C77,1);
unsigned char patch005F4C78=addr3;
DX::memPatch(0x005F4C78, &patch005F4C78,1);
unsigned char patch005FB0A1=addr0;
DX::memPatch(0x005FB0A1, &patch005FB0A1,1);
unsigned char patch005FB0A2=addr1;
DX::memPatch(0x005FB0A2, &patch005FB0A2,1);
unsigned char patch005FB0A3=addr2;
DX::memPatch(0x005FB0A3, &patch005FB0A3,1);
unsigned char patch005FB0A4=addr3;
DX::memPatch(0x005FB0A4, &patch005FB0A4,1);
unsigned char patch005FB2C3=addr0;
DX::memPatch(0x005FB2C3, &patch005FB2C3,1);
unsigned char patch005FB2C4=addr1;
DX::memPatch(0x005FB2C4, &patch005FB2C4,1);
unsigned char patch005FB2C5=addr2;
DX::memPatch(0x005FB2C5, &patch005FB2C5,1);
unsigned char patch005FB2C6=addr3;
DX::memPatch(0x005FB2C6, &patch005FB2C6,1);
unsigned char patch005FB2F1=addr0;
DX::memPatch(0x005FB2F1, &patch005FB2F1,1);
unsigned char patch005FB2F2=addr1;
DX::memPatch(0x005FB2F2, &patch005FB2F2,1);
unsigned char patch005FB2F3=addr2;
DX::memPatch(0x005FB2F3, &patch005FB2F3,1);
unsigned char patch005FB2F4=addr3;
DX::memPatch(0x005FB2F4, &patch005FB2F4,1);
unsigned char patch005FBE44=0x0;
DX::memPatch(0x005FBE44, &patch005FBE44,1);
unsigned char patch005FBE45=0x40;
DX::memPatch(0x005FBE45, &patch005FBE45,1);
unsigned char patch005FC174=0x20;
DX::memPatch(0x005FC174, &patch005FC174,1);
unsigned char patch005FC982=0x0;
DX::memPatch(0x005FC982, &patch005FC982,1);
unsigned char patch005FC983=0x40;
DX::memPatch(0x005FC983, &patch005FC983,1);
unsigned char patch005FCAFC=0x20;
DX::memPatch(0x005FCAFC, &patch005FCAFC,1);
unsigned char patch005FED3A=0x0;
DX::memPatch(0x005FED3A, &patch005FED3A,1);
unsigned char patch005FED3B=0x40;
DX::memPatch(0x005FED3B, &patch005FED3B,1);
unsigned char patch005FEDA7=0x0;
DX::memPatch(0x005FEDA7, &patch005FEDA7,1);
unsigned char patch005FEDA8=0x40;
DX::memPatch(0x005FEDA8, &patch005FEDA8,1);
unsigned char patch005FFBD7=addr0;
DX::memPatch(0x005FFBD7, &patch005FFBD7,1);
unsigned char patch005FFBD8=addr1;
DX::memPatch(0x005FFBD8, &patch005FFBD8,1);
unsigned char patch005FFBD9=addr2;
DX::memPatch(0x005FFBD9, &patch005FFBD9,1);
unsigned char patch005FFBDA=addr3;
DX::memPatch(0x005FFBDA, &patch005FFBDA,1);
unsigned char patch005FFBE1=addr4;
DX::memPatch(0x005FFBE1, &patch005FFBE1,1);
unsigned char patch005FFBE2=addr5;
DX::memPatch(0x005FFBE2, &patch005FFBE2,1);
unsigned char patch005FFBE3=addr6;
DX::memPatch(0x005FFBE3, &patch005FFBE3,1);
unsigned char patch005FFBE4=addr7;
DX::memPatch(0x005FFBE4, &patch005FFBE4,1);
unsigned char patch005FFD4C=addr0;
DX::memPatch(0x005FFD4C, &patch005FFD4C,1);
unsigned char patch005FFD4D=addr1;
DX::memPatch(0x005FFD4D, &patch005FFD4D,1);
unsigned char patch005FFD4E=addr2;
DX::memPatch(0x005FFD4E, &patch005FFD4E,1);
unsigned char patch005FFD4F=addr3;
DX::memPatch(0x005FFD4F, &patch005FFD4F,1);
unsigned char patch005FFE5B=addr0;
DX::memPatch(0x005FFE5B, &patch005FFE5B,1);
unsigned char patch005FFE5C=addr1;
DX::memPatch(0x005FFE5C, &patch005FFE5C,1);
unsigned char patch005FFE5D=addr2;
DX::memPatch(0x005FFE5D, &patch005FFE5D,1);
unsigned char patch005FFE5E=addr3;
DX::memPatch(0x005FFE5E, &patch005FFE5E,1);
unsigned char patch005FFE8C=addr0;
DX::memPatch(0x005FFE8C, &patch005FFE8C,1);
unsigned char patch005FFE8D=addr1;
DX::memPatch(0x005FFE8D, &patch005FFE8D,1);
unsigned char patch005FFE8E=addr2;
DX::memPatch(0x005FFE8E, &patch005FFE8E,1);
unsigned char patch005FFE8F=addr3;
DX::memPatch(0x005FFE8F, &patch005FFE8F,1);
unsigned char patch005FFF51=addr0;
DX::memPatch(0x005FFF51, &patch005FFF51,1);
unsigned char patch005FFF52=addr1;
DX::memPatch(0x005FFF52, &patch005FFF52,1);
unsigned char patch005FFF53=addr2;
DX::memPatch(0x005FFF53, &patch005FFF53,1);
unsigned char patch005FFF54=addr3;
DX::memPatch(0x005FFF54, &patch005FFF54,1);
unsigned char patch00600105=addr0;
DX::memPatch(0x00600105, &patch00600105,1);
unsigned char patch00600106=addr1;
DX::memPatch(0x00600106, &patch00600106,1);
unsigned char patch00600107=addr2;
DX::memPatch(0x00600107, &patch00600107,1);
unsigned char patch00600108=addr3;
DX::memPatch(0x00600108, &patch00600108,1);
unsigned char patch00606483=0x20;
DX::memPatch(0x00606483, &patch00606483,1);
unsigned char patch00606816=0x20;
DX::memPatch(0x00606816, &patch00606816,1);
unsigned char patch00608D99=addr0;
DX::memPatch(0x00608D99, &patch00608D99,1);
unsigned char patch00608D9A=addr1;
DX::memPatch(0x00608D9A, &patch00608D9A,1);
unsigned char patch00608D9B=addr2;
DX::memPatch(0x00608D9B, &patch00608D9B,1);
unsigned char patch00608D9C=addr3;
DX::memPatch(0x00608D9C, &patch00608D9C,1);
unsigned char patch00608DDC=addr0;
DX::memPatch(0x00608DDC, &patch00608DDC,1);
unsigned char patch00608DDD=addr1;
DX::memPatch(0x00608DDD, &patch00608DDD,1);
unsigned char patch00608DDE=addr2;
DX::memPatch(0x00608DDE, &patch00608DDE,1);
unsigned char patch00608DDF=addr3;
DX::memPatch(0x00608DDF, &patch00608DDF,1);
unsigned char patch00608E38=addr0;
DX::memPatch(0x00608E38, &patch00608E38,1);
unsigned char patch00608E39=addr1;
DX::memPatch(0x00608E39, &patch00608E39,1);
unsigned char patch00608E3A=addr2;
DX::memPatch(0x00608E3A, &patch00608E3A,1);
unsigned char patch00608E3B=addr3;
DX::memPatch(0x00608E3B, &patch00608E3B,1);
unsigned char patch00608E9D=addr0;
DX::memPatch(0x00608E9D, &patch00608E9D,1);
unsigned char patch00608E9E=addr1;
DX::memPatch(0x00608E9E, &patch00608E9E,1);
unsigned char patch00608E9F=addr2;
DX::memPatch(0x00608E9F, &patch00608E9F,1);
unsigned char patch00608EA0=addr3;
DX::memPatch(0x00608EA0, &patch00608EA0,1);
unsigned char patch00608F07=addr0;
DX::memPatch(0x00608F07, &patch00608F07,1);
unsigned char patch00608F08=addr1;
DX::memPatch(0x00608F08, &patch00608F08,1);
unsigned char patch00608F09=addr2;
DX::memPatch(0x00608F09, &patch00608F09,1);
unsigned char patch00608F0A=addr3;
DX::memPatch(0x00608F0A, &patch00608F0A,1);
unsigned char patch0060F5D7=addr0;
DX::memPatch(0x0060F5D7, &patch0060F5D7,1);
unsigned char patch0060F5D8=addr1;
DX::memPatch(0x0060F5D8, &patch0060F5D8,1);
unsigned char patch0060F5D9=addr2;
DX::memPatch(0x0060F5D9, &patch0060F5D9,1);
unsigned char patch0060F5DA=addr3;
DX::memPatch(0x0060F5DA, &patch0060F5DA,1);
unsigned char patch0060F61E=addr0;
DX::memPatch(0x0060F61E, &patch0060F61E,1);
unsigned char patch0060F61F=addr1;
DX::memPatch(0x0060F61F, &patch0060F61F,1);
unsigned char patch0060F620=addr2;
DX::memPatch(0x0060F620, &patch0060F620,1);
unsigned char patch0060F621=addr3;
DX::memPatch(0x0060F621, &patch0060F621,1);
unsigned char patch00611EB7=addr0;
DX::memPatch(0x00611EB7, &patch00611EB7,1);
unsigned char patch00611EB8=addr1;
DX::memPatch(0x00611EB8, &patch00611EB8,1);
unsigned char patch00611EB9=addr2;
DX::memPatch(0x00611EB9, &patch00611EB9,1);
unsigned char patch00611EBA=addr3;
DX::memPatch(0x00611EBA, &patch00611EBA,1);
unsigned char patch00611EFA=addr0;
DX::memPatch(0x00611EFA, &patch00611EFA,1);
unsigned char patch00611EFB=addr1;
DX::memPatch(0x00611EFB, &patch00611EFB,1);
unsigned char patch00611EFC=addr2;
DX::memPatch(0x00611EFC, &patch00611EFC,1);
unsigned char patch00611EFD=addr3;
DX::memPatch(0x00611EFD, &patch00611EFD,1);
unsigned char patch0061661D=addr0;
DX::memPatch(0x0061661D, &patch0061661D,1);
unsigned char patch0061661E=addr1;
DX::memPatch(0x0061661E, &patch0061661E,1);
unsigned char patch0061661F=addr2;
DX::memPatch(0x0061661F, &patch0061661F,1);
unsigned char patch00616620=addr3;
DX::memPatch(0x00616620, &patch00616620,1);
unsigned char patch0061665D=addr0;
DX::memPatch(0x0061665D, &patch0061665D,1);
unsigned char patch0061665E=addr1;
DX::memPatch(0x0061665E, &patch0061665E,1);
unsigned char patch0061665F=addr2;
DX::memPatch(0x0061665F, &patch0061665F,1);
unsigned char patch00616660=addr3;
DX::memPatch(0x00616660, &patch00616660,1);
unsigned char patch00616697=addr0;
DX::memPatch(0x00616697, &patch00616697,1);
unsigned char patch00616698=addr1;
DX::memPatch(0x00616698, &patch00616698,1);
unsigned char patch00616699=addr2;
DX::memPatch(0x00616699, &patch00616699,1);
unsigned char patch0061669A=addr3;
DX::memPatch(0x0061669A, &patch0061669A,1);
unsigned char patch0061EB0F=addr0;
DX::memPatch(0x0061EB0F, &patch0061EB0F,1);
unsigned char patch0061EB10=addr1;
DX::memPatch(0x0061EB10, &patch0061EB10,1);
unsigned char patch0061EB11=addr2;
DX::memPatch(0x0061EB11, &patch0061EB11,1);
unsigned char patch0061EB12=addr3;
DX::memPatch(0x0061EB12, &patch0061EB12,1);
unsigned char patch0061EB53=addr0;
DX::memPatch(0x0061EB53, &patch0061EB53,1);
unsigned char patch0061EB54=addr1;
DX::memPatch(0x0061EB54, &patch0061EB54,1);
unsigned char patch0061EB55=addr2;
DX::memPatch(0x0061EB55, &patch0061EB55,1);
unsigned char patch0061EB56=addr3;
DX::memPatch(0x0061EB56, &patch0061EB56,1);
unsigned char patch0061EF48=addr0;
DX::memPatch(0x0061EF48, &patch0061EF48,1);
unsigned char patch0061EF49=addr1;
DX::memPatch(0x0061EF49, &patch0061EF49,1);
unsigned char patch0061EF4A=addr2;
DX::memPatch(0x0061EF4A, &patch0061EF4A,1);
unsigned char patch0061EF4B=addr3;
DX::memPatch(0x0061EF4B, &patch0061EF4B,1);
unsigned char patch0061EF9C=addr0;
DX::memPatch(0x0061EF9C, &patch0061EF9C,1);
unsigned char patch0061EF9D=addr1;
DX::memPatch(0x0061EF9D, &patch0061EF9D,1);
unsigned char patch0061EF9E=addr2;
DX::memPatch(0x0061EF9E, &patch0061EF9E,1);
unsigned char patch0061EF9F=addr3;
DX::memPatch(0x0061EF9F, &patch0061EF9F,1);
unsigned char patch0061F004=addr0;
DX::memPatch(0x0061F004, &patch0061F004,1);
unsigned char patch0061F005=addr1;
DX::memPatch(0x0061F005, &patch0061F005,1);
unsigned char patch0061F006=addr2;
DX::memPatch(0x0061F006, &patch0061F006,1);
unsigned char patch0061F007=addr3;
DX::memPatch(0x0061F007, &patch0061F007,1);
unsigned char patch0061F067=addr0;
DX::memPatch(0x0061F067, &patch0061F067,1);
unsigned char patch0061F068=addr1;
DX::memPatch(0x0061F068, &patch0061F068,1);
unsigned char patch0061F069=addr2;
DX::memPatch(0x0061F069, &patch0061F069,1);
unsigned char patch0061F06A=addr3;
DX::memPatch(0x0061F06A, &patch0061F06A,1);
unsigned char patch0062221B=0x0;
DX::memPatch(0x0062221B, &patch0062221B,1);
unsigned char patch0062221C=0x80;
DX::memPatch(0x0062221C, &patch0062221C,1);
unsigned char patch00622B1E=addr0;
DX::memPatch(0x00622B1E, &patch00622B1E,1);
unsigned char patch00622B1F=addr1;
DX::memPatch(0x00622B1F, &patch00622B1F,1);
unsigned char patch00622B20=addr2;
DX::memPatch(0x00622B20, &patch00622B20,1);
unsigned char patch00622B21=addr3;
DX::memPatch(0x00622B21, &patch00622B21,1);
unsigned char patch006262C5=addr0;
DX::memPatch(0x006262C5, &patch006262C5,1);
unsigned char patch006262C6=addr1;
DX::memPatch(0x006262C6, &patch006262C6,1);
unsigned char patch006262C7=addr2;
DX::memPatch(0x006262C7, &patch006262C7,1);
unsigned char patch006262C8=addr3;
DX::memPatch(0x006262C8, &patch006262C8,1);
unsigned char patch00626DA3=0x0;
DX::memPatch(0x00626DA3, &patch00626DA3,1);
unsigned char patch00626DA4=0x40;
DX::memPatch(0x00626DA4, &patch00626DA4,1);
unsigned char patch00626E07=0x0;
DX::memPatch(0x00626E07, &patch00626E07,1);
unsigned char patch00626E08=0x40;
DX::memPatch(0x00626E08, &patch00626E08,1);
unsigned char patch00626E92=0x0;
DX::memPatch(0x00626E92, &patch00626E92,1);
unsigned char patch00626E93=0x40;
DX::memPatch(0x00626E93, &patch00626E93,1);
unsigned char patch00626F2C=0x0;
DX::memPatch(0x00626F2C, &patch00626F2C,1);
unsigned char patch00626F2D=0x40;
DX::memPatch(0x00626F2D, &patch00626F2D,1);
unsigned char patch006271D6=addr0;
DX::memPatch(0x006271D6, &patch006271D6,1);
unsigned char patch006271D7=addr1;
DX::memPatch(0x006271D7, &patch006271D7,1);
unsigned char patch006271D8=addr2;
DX::memPatch(0x006271D8, &patch006271D8,1);
unsigned char patch006271D9=addr3;
DX::memPatch(0x006271D9, &patch006271D9,1);
unsigned char patch0062722E=addr0;
DX::memPatch(0x0062722E, &patch0062722E,1);
unsigned char patch0062722F=addr1;
DX::memPatch(0x0062722F, &patch0062722F,1);
unsigned char patch00627230=addr2;
DX::memPatch(0x00627230, &patch00627230,1);
unsigned char patch00627231=addr3;
DX::memPatch(0x00627231, &patch00627231,1);
unsigned char patch0062F031=0x0;
DX::memPatch(0x0062F031, &patch0062F031,1);
unsigned char patch0062F032=0x40;
DX::memPatch(0x0062F032, &patch0062F032,1);
unsigned char patch0062F0EF=0x0;
DX::memPatch(0x0062F0EF, &patch0062F0EF,1);
unsigned char patch0062F0F0=0x40;
DX::memPatch(0x0062F0F0, &patch0062F0F0,1);
unsigned char patch0062F29F=0x0;
DX::memPatch(0x0062F29F, &patch0062F29F,1);
unsigned char patch0062F2A0=0x40;
DX::memPatch(0x0062F2A0, &patch0062F2A0,1);
unsigned char patch0062F500=0x0;
DX::memPatch(0x0062F500, &patch0062F500,1);
unsigned char patch0062F501=0x40;
DX::memPatch(0x0062F501, &patch0062F501,1);
unsigned char patch00630705=addr0;
DX::memPatch(0x00630705, &patch00630705,1);
unsigned char patch00630706=addr1;
DX::memPatch(0x00630706, &patch00630706,1);
unsigned char patch00630707=addr2;
DX::memPatch(0x00630707, &patch00630707,1);
unsigned char patch00630708=addr3;
DX::memPatch(0x00630708, &patch00630708,1);
unsigned char patch0063075D=addr0;
DX::memPatch(0x0063075D, &patch0063075D,1);
unsigned char patch0063075E=addr1;
DX::memPatch(0x0063075E, &patch0063075E,1);
unsigned char patch0063075F=addr2;
DX::memPatch(0x0063075F, &patch0063075F,1);
unsigned char patch00630760=addr3;
DX::memPatch(0x00630760, &patch00630760,1);
unsigned char patch006307B5=addr0;
DX::memPatch(0x006307B5, &patch006307B5,1);
unsigned char patch006307B6=addr1;
DX::memPatch(0x006307B6, &patch006307B6,1);
unsigned char patch006307B7=addr2;
DX::memPatch(0x006307B7, &patch006307B7,1);
unsigned char patch006307B8=addr3;
DX::memPatch(0x006307B8, &patch006307B8,1);
unsigned char patch0063080D=addr0;
DX::memPatch(0x0063080D, &patch0063080D,1);
unsigned char patch0063080E=addr1;
DX::memPatch(0x0063080E, &patch0063080E,1);
unsigned char patch0063080F=addr2;
DX::memPatch(0x0063080F, &patch0063080F,1);
unsigned char patch00630810=addr3;
DX::memPatch(0x00630810, &patch00630810,1);
unsigned char patch00630865=addr0;
DX::memPatch(0x00630865, &patch00630865,1);
unsigned char patch00630866=addr1;
DX::memPatch(0x00630866, &patch00630866,1);
unsigned char patch00630867=addr2;
DX::memPatch(0x00630867, &patch00630867,1);
unsigned char patch00630868=addr3;
DX::memPatch(0x00630868, &patch00630868,1);
unsigned char patch006308BD=addr0;
DX::memPatch(0x006308BD, &patch006308BD,1);
unsigned char patch006308BE=addr1;
DX::memPatch(0x006308BE, &patch006308BE,1);
unsigned char patch006308BF=addr2;
DX::memPatch(0x006308BF, &patch006308BF,1);
unsigned char patch006308C0=addr3;
DX::memPatch(0x006308C0, &patch006308C0,1);
unsigned char patch00630915=addr0;
DX::memPatch(0x00630915, &patch00630915,1);
unsigned char patch00630916=addr1;
DX::memPatch(0x00630916, &patch00630916,1);
unsigned char patch00630917=addr2;
DX::memPatch(0x00630917, &patch00630917,1);
unsigned char patch00630918=addr3;
DX::memPatch(0x00630918, &patch00630918,1);
unsigned char patch0063096D=addr0;
DX::memPatch(0x0063096D, &patch0063096D,1);
unsigned char patch0063096E=addr1;
DX::memPatch(0x0063096E, &patch0063096E,1);
unsigned char patch0063096F=addr2;
DX::memPatch(0x0063096F, &patch0063096F,1);
unsigned char patch00630970=addr3;
DX::memPatch(0x00630970, &patch00630970,1);
unsigned char patch006309C5=addr0;
DX::memPatch(0x006309C5, &patch006309C5,1);
unsigned char patch006309C6=addr1;
DX::memPatch(0x006309C6, &patch006309C6,1);
unsigned char patch006309C7=addr2;
DX::memPatch(0x006309C7, &patch006309C7,1);
unsigned char patch006309C8=addr3;
DX::memPatch(0x006309C8, &patch006309C8,1);
unsigned char patch00630F91=addr0;
DX::memPatch(0x00630F91, &patch00630F91,1);
unsigned char patch00630F92=addr1;
DX::memPatch(0x00630F92, &patch00630F92,1);
unsigned char patch00630F93=addr2;
DX::memPatch(0x00630F93, &patch00630F93,1);
unsigned char patch00630F94=addr3;
DX::memPatch(0x00630F94, &patch00630F94,1);
unsigned char patch00635E2B=0x0;
DX::memPatch(0x00635E2B, &patch00635E2B,1);
unsigned char patch00635E2C=0x40;
DX::memPatch(0x00635E2C, &patch00635E2C,1);
unsigned char patch00635EAB=0x0;
DX::memPatch(0x00635EAB, &patch00635EAB,1);
unsigned char patch00635EAC=0x40;
DX::memPatch(0x00635EAC, &patch00635EAC,1);
unsigned char patch006362C3=0x0;
DX::memPatch(0x006362C3, &patch006362C3,1);
unsigned char patch006362C4=0x40;
DX::memPatch(0x006362C4, &patch006362C4,1);
unsigned char patch006363D4=0x0;
DX::memPatch(0x006363D4, &patch006363D4,1);
unsigned char patch006363D5=0x40;
DX::memPatch(0x006363D5, &patch006363D5,1);
unsigned char patch0063785F=addr0;
DX::memPatch(0x0063785F, &patch0063785F,1);
unsigned char patch00637860=addr1;
DX::memPatch(0x00637860, &patch00637860,1);
unsigned char patch00637861=addr2;
DX::memPatch(0x00637861, &patch00637861,1);
unsigned char patch00637862=addr3;
DX::memPatch(0x00637862, &patch00637862,1);
unsigned char patch006378B7=addr0;
DX::memPatch(0x006378B7, &patch006378B7,1);
unsigned char patch006378B8=addr1;
DX::memPatch(0x006378B8, &patch006378B8,1);
unsigned char patch006378B9=addr2;
DX::memPatch(0x006378B9, &patch006378B9,1);
unsigned char patch006378BA=addr3;
DX::memPatch(0x006378BA, &patch006378BA,1);
unsigned char patch0063790F=addr0;
DX::memPatch(0x0063790F, &patch0063790F,1);
unsigned char patch00637910=addr1;
DX::memPatch(0x00637910, &patch00637910,1);
unsigned char patch00637911=addr2;
DX::memPatch(0x00637911, &patch00637911,1);
unsigned char patch00637912=addr3;
DX::memPatch(0x00637912, &patch00637912,1);
unsigned char patch0063BC8B=0x0;
DX::memPatch(0x0063BC8B, &patch0063BC8B,1);
unsigned char patch0063BC8C=0x40;
DX::memPatch(0x0063BC8C, &patch0063BC8C,1);
unsigned char patch0063BD30=0x0;
DX::memPatch(0x0063BD30, &patch0063BD30,1);
unsigned char patch0063BD31=0x40;
DX::memPatch(0x0063BD31, &patch0063BD31,1);
unsigned char patch0063BF9B=0x0;
DX::memPatch(0x0063BF9B, &patch0063BF9B,1);
unsigned char patch0063BF9C=0x40;
DX::memPatch(0x0063BF9C, &patch0063BF9C,1);
unsigned char patch0063C16C=0x0;
DX::memPatch(0x0063C16C, &patch0063C16C,1);
unsigned char patch0063C16D=0x40;
DX::memPatch(0x0063C16D, &patch0063C16D,1);
unsigned char patch0063C2FE=0x0;
DX::memPatch(0x0063C2FE, &patch0063C2FE,1);
unsigned char patch0063C2FF=0x40;
DX::memPatch(0x0063C2FF, &patch0063C2FF,1);
unsigned char patch0063C6AC=0x0;
DX::memPatch(0x0063C6AC, &patch0063C6AC,1);
unsigned char patch0063C6AD=0x40;
DX::memPatch(0x0063C6AD, &patch0063C6AD,1);
unsigned char patch006438CF=0x0;
DX::memPatch(0x006438CF, &patch006438CF,1);
unsigned char patch006438D0=0x40;
DX::memPatch(0x006438D0, &patch006438D0,1);
unsigned char patch0064395D=0x0;
DX::memPatch(0x0064395D, &patch0064395D,1);
unsigned char patch0064395E=0x40;
DX::memPatch(0x0064395E, &patch0064395E,1);
unsigned char patch00643C93=0x0;
DX::memPatch(0x00643C93, &patch00643C93,1);
unsigned char patch00643C94=0x40;
DX::memPatch(0x00643C94, &patch00643C94,1);
unsigned char patch00643E1C=0x0;
DX::memPatch(0x00643E1C, &patch00643E1C,1);
unsigned char patch00643E1D=0x40;
DX::memPatch(0x00643E1D, &patch00643E1D,1);
unsigned char patch006454BB=0x0;
DX::memPatch(0x006454BB, &patch006454BB,1);
unsigned char patch006454BC=0x40;
DX::memPatch(0x006454BC, &patch006454BC,1);
unsigned char patch006454F4=0x0;
DX::memPatch(0x006454F4, &patch006454F4,1);
unsigned char patch006454F5=0x40;
DX::memPatch(0x006454F5, &patch006454F5,1);
unsigned char patch006455AC=0x0;
DX::memPatch(0x006455AC, &patch006455AC,1);
unsigned char patch006455AD=0x40;
DX::memPatch(0x006455AD, &patch006455AD,1);
unsigned char patch006455E3=0x0;
DX::memPatch(0x006455E3, &patch006455E3,1);
unsigned char patch006455E4=0x40;
DX::memPatch(0x006455E4, &patch006455E4,1);
unsigned char patch00649AA2=0x0;
DX::memPatch(0x00649AA2, &patch00649AA2,1);
unsigned char patch00649AA3=0x40;
DX::memPatch(0x00649AA3, &patch00649AA3,1);
unsigned char patch00649B2E=0x0;
DX::memPatch(0x00649B2E, &patch00649B2E,1);
unsigned char patch00649B2F=0x40;
DX::memPatch(0x00649B2F, &patch00649B2F,1);
unsigned char patch00649E0C=0x0;
DX::memPatch(0x00649E0C, &patch00649E0C,1);
unsigned char patch00649E0D=0x40;
DX::memPatch(0x00649E0D, &patch00649E0D,1);
unsigned char patch00649F8C=0x0;
DX::memPatch(0x00649F8C, &patch00649F8C,1);
unsigned char patch00649F8D=0x40;
DX::memPatch(0x00649F8D, &patch00649F8D,1);
unsigned char patch0064AA67=addr0;
DX::memPatch(0x0064AA67, &patch0064AA67,1);
unsigned char patch0064AA68=addr1;
DX::memPatch(0x0064AA68, &patch0064AA68,1);
unsigned char patch0064AA69=addr2;
DX::memPatch(0x0064AA69, &patch0064AA69,1);
unsigned char patch0064AA6A=addr3;
DX::memPatch(0x0064AA6A, &patch0064AA6A,1);
unsigned char patch0064D401=0x0;
DX::memPatch(0x0064D401, &patch0064D401,1);
unsigned char patch0064D402=0x40;
DX::memPatch(0x0064D402, &patch0064D402,1);
unsigned char patch0064D43C=0x0;
DX::memPatch(0x0064D43C, &patch0064D43C,1);
unsigned char patch0064D43D=0x40;
DX::memPatch(0x0064D43D, &patch0064D43D,1);
unsigned char patch0064D4FC=0x0;
DX::memPatch(0x0064D4FC, &patch0064D4FC,1);
unsigned char patch0064D4FD=0x40;
DX::memPatch(0x0064D4FD, &patch0064D4FD,1);
unsigned char patch0064D533=0x0;
DX::memPatch(0x0064D533, &patch0064D533,1);
unsigned char patch0064D534=0x40;
DX::memPatch(0x0064D534, &patch0064D534,1);
unsigned char patch0064E5D7=addr0;
DX::memPatch(0x0064E5D7, &patch0064E5D7,1);
unsigned char patch0064E5D8=addr1;
DX::memPatch(0x0064E5D8, &patch0064E5D8,1);
unsigned char patch0064E5D9=addr2;
DX::memPatch(0x0064E5D9, &patch0064E5D9,1);
unsigned char patch0064E5DA=addr3;
DX::memPatch(0x0064E5DA, &patch0064E5DA,1);
unsigned char patch0064E638=addr0;
DX::memPatch(0x0064E638, &patch0064E638,1);
unsigned char patch0064E639=addr1;
DX::memPatch(0x0064E639, &patch0064E639,1);
unsigned char patch0064E63A=addr2;
DX::memPatch(0x0064E63A, &patch0064E63A,1);
unsigned char patch0064E63B=addr3;
DX::memPatch(0x0064E63B, &patch0064E63B,1);
unsigned char patch0065072B=0x0;
DX::memPatch(0x0065072B, &patch0065072B,1);
unsigned char patch0065072C=0x40;
DX::memPatch(0x0065072C, &patch0065072C,1);
unsigned char patch00650840=0x0;
DX::memPatch(0x00650840, &patch00650840,1);
unsigned char patch00650841=0x40;
DX::memPatch(0x00650841, &patch00650841,1);
unsigned char patch006508F3=0x0;
DX::memPatch(0x006508F3, &patch006508F3,1);
unsigned char patch006508F4=0x40;
DX::memPatch(0x006508F4, &patch006508F4,1);
unsigned char patch00650B24=0x0;
DX::memPatch(0x00650B24, &patch00650B24,1);
unsigned char patch00650B25=0x40;
DX::memPatch(0x00650B25, &patch00650B25,1);
unsigned char patch0066BB8A=0x0;
DX::memPatch(0x0066BB8A, &patch0066BB8A,1);
unsigned char patch0066BB8B=0x80;
DX::memPatch(0x0066BB8B, &patch0066BB8B,1);
unsigned char patch0066BC5B=0x0;
DX::memPatch(0x0066BC5B, &patch0066BC5B,1);
unsigned char patch0066BC5C=0x80;
DX::memPatch(0x0066BC5C, &patch0066BC5C,1);
unsigned char patch0066BD53=addr0;
DX::memPatch(0x0066BD53, &patch0066BD53,1);
unsigned char patch0066BD54=addr1;
DX::memPatch(0x0066BD54, &patch0066BD54,1);
unsigned char patch0066BD55=addr2;
DX::memPatch(0x0066BD55, &patch0066BD55,1);
unsigned char patch0066BD56=addr3;
DX::memPatch(0x0066BD56, &patch0066BD56,1);
unsigned char patch0066D0CA=addr4;
DX::memPatch(0x0066D0CA, &patch0066D0CA,1);
unsigned char patch0066D0CB=addr5;
DX::memPatch(0x0066D0CB, &patch0066D0CB,1);
unsigned char patch0066D0CC=addr6;
DX::memPatch(0x0066D0CC, &patch0066D0CC,1);
unsigned char patch0066D0CD=addr7;
DX::memPatch(0x0066D0CD, &patch0066D0CD,1);
unsigned char patch0066D2D1=addr4;
DX::memPatch(0x0066D2D1, &patch0066D2D1,1);
unsigned char patch0066D2D2=addr5;
DX::memPatch(0x0066D2D2, &patch0066D2D2,1);
unsigned char patch0066D2D3=addr6;
DX::memPatch(0x0066D2D3, &patch0066D2D3,1);
unsigned char patch0066D2D4=addr7;
DX::memPatch(0x0066D2D4, &patch0066D2D4,1);
unsigned char patch00670BA5=addr0;
DX::memPatch(0x00670BA5, &patch00670BA5,1);
unsigned char patch00670BA6=addr1;
DX::memPatch(0x00670BA6, &patch00670BA6,1);
unsigned char patch00670BA7=addr2;
DX::memPatch(0x00670BA7, &patch00670BA7,1);
unsigned char patch00670BA8=addr3;
DX::memPatch(0x00670BA8, &patch00670BA8,1);
unsigned char patch006734CE=0x0;
DX::memPatch(0x006734CE, &patch006734CE,1);
unsigned char patch006734CF=0x80;
DX::memPatch(0x006734CF, &patch006734CF,1);
unsigned char patch0067395E=0x0;
DX::memPatch(0x0067395E, &patch0067395E,1);
unsigned char patch0067395F=0x80;
DX::memPatch(0x0067395F, &patch0067395F,1);
unsigned char patch00673C3B=addr0;
DX::memPatch(0x00673C3B, &patch00673C3B,1);
unsigned char patch00673C3C=addr1;
DX::memPatch(0x00673C3C, &patch00673C3C,1);
unsigned char patch00673C3D=addr2;
DX::memPatch(0x00673C3D, &patch00673C3D,1);
unsigned char patch00673C3E=addr3;
DX::memPatch(0x00673C3E, &patch00673C3E,1);
unsigned char patch0067DED1=addr0;
DX::memPatch(0x0067DED1, &patch0067DED1,1);
unsigned char patch0067DED2=addr1;
DX::memPatch(0x0067DED2, &patch0067DED2,1);
unsigned char patch0067DED3=addr2;
DX::memPatch(0x0067DED3, &patch0067DED3,1);
unsigned char patch0067DED4=addr3;
DX::memPatch(0x0067DED4, &patch0067DED4,1);
unsigned char patch0067DF21=addr0;
DX::memPatch(0x0067DF21, &patch0067DF21,1);
unsigned char patch0067DF22=addr1;
DX::memPatch(0x0067DF22, &patch0067DF22,1);
unsigned char patch0067DF23=addr2;
DX::memPatch(0x0067DF23, &patch0067DF23,1);
unsigned char patch0067DF24=addr3;
DX::memPatch(0x0067DF24, &patch0067DF24,1);
unsigned char patch00680376=addr0;
DX::memPatch(0x00680376, &patch00680376,1);
unsigned char patch00680377=addr1;
DX::memPatch(0x00680377, &patch00680377,1);
unsigned char patch00680378=addr2;
DX::memPatch(0x00680378, &patch00680378,1);
unsigned char patch00680379=addr3;
DX::memPatch(0x00680379, &patch00680379,1);
unsigned char patch00683AC9=addr0;
DX::memPatch(0x00683AC9, &patch00683AC9,1);
unsigned char patch00683ACA=addr1;
DX::memPatch(0x00683ACA, &patch00683ACA,1);
unsigned char patch00683ACB=addr2;
DX::memPatch(0x00683ACB, &patch00683ACB,1);
unsigned char patch00683ACC=addr3;
DX::memPatch(0x00683ACC, &patch00683ACC,1);
unsigned char patch00683B2A=addr0;
DX::memPatch(0x00683B2A, &patch00683B2A,1);
unsigned char patch00683B2B=addr1;
DX::memPatch(0x00683B2B, &patch00683B2B,1);
unsigned char patch00683B2C=addr2;
DX::memPatch(0x00683B2C, &patch00683B2C,1);
unsigned char patch00683B2D=addr3;
DX::memPatch(0x00683B2D, &patch00683B2D,1);
unsigned char patch0068CCB1=addr0;
DX::memPatch(0x0068CCB1, &patch0068CCB1,1);
unsigned char patch0068CCB2=addr1;
DX::memPatch(0x0068CCB2, &patch0068CCB2,1);
unsigned char patch0068CCB3=addr2;
DX::memPatch(0x0068CCB3, &patch0068CCB3,1);
unsigned char patch0068CCB4=addr3;
DX::memPatch(0x0068CCB4, &patch0068CCB4,1);
unsigned char patch00691E71=addr0;
DX::memPatch(0x00691E71, &patch00691E71,1);
unsigned char patch00691E72=addr1;
DX::memPatch(0x00691E72, &patch00691E72,1);
unsigned char patch00691E73=addr2;
DX::memPatch(0x00691E73, &patch00691E73,1);
unsigned char patch00691E74=addr3;
DX::memPatch(0x00691E74, &patch00691E74,1);
unsigned char patch00691F26=addr0;
DX::memPatch(0x00691F26, &patch00691F26,1);
unsigned char patch00691F27=addr1;
DX::memPatch(0x00691F27, &patch00691F27,1);
unsigned char patch00691F28=addr2;
DX::memPatch(0x00691F28, &patch00691F28,1);
unsigned char patch00691F29=addr3;
DX::memPatch(0x00691F29, &patch00691F29,1);
unsigned char patch0069692C=0x0;
DX::memPatch(0x0069692C, &patch0069692C,1);
unsigned char patch0069692D=0x40;
DX::memPatch(0x0069692D, &patch0069692D,1);
unsigned char patch00696A44=0x0;
DX::memPatch(0x00696A44, &patch00696A44,1);
unsigned char patch00696A45=0x40;
DX::memPatch(0x00696A45, &patch00696A45,1);
unsigned char patch0069AF76=addr0;
DX::memPatch(0x0069AF76, &patch0069AF76,1);
unsigned char patch0069AF77=addr1;
DX::memPatch(0x0069AF77, &patch0069AF77,1);
unsigned char patch0069AF78=addr2;
DX::memPatch(0x0069AF78, &patch0069AF78,1);
unsigned char patch0069AF79=addr3;
DX::memPatch(0x0069AF79, &patch0069AF79,1);
unsigned char patch0069C93B=0x0;
DX::memPatch(0x0069C93B, &patch0069C93B,1);
unsigned char patch0069C93C=0x40;
DX::memPatch(0x0069C93C, &patch0069C93C,1);
unsigned char patch0069C9FC=0x0;
DX::memPatch(0x0069C9FC, &patch0069C9FC,1);
unsigned char patch0069C9FD=0x40;
DX::memPatch(0x0069C9FD, &patch0069C9FD,1);
unsigned char patch0069EE6B=0x0;
DX::memPatch(0x0069EE6B, &patch0069EE6B,1);
unsigned char patch0069EE6C=0x40;
DX::memPatch(0x0069EE6C, &patch0069EE6C,1);
unsigned char patch0069EF2C=0x0;
DX::memPatch(0x0069EF2C, &patch0069EF2C,1);
unsigned char patch0069EF2D=0x40;
DX::memPatch(0x0069EF2D, &patch0069EF2D,1);

	memcpy((void*)addr,(void*)0x847d68,0x2000);
	memcpy((void*)addrbase2,(void*)0x9af748,0x2000);
	memcpy(((void*)(addr+0xa000)),(void*)0x849d68,0x8);
	memcpy(((void*)(addrbase2+0xa000)),(void*)0x9B1748,0x8);
	Con::errorf(0, "Danger, DB Patch Completed");
	return false;
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

extern "C"
{
	TSEXTENSION_API unsigned int getModLoaderVersion(void)
	{
		return 0;
	}

	TSEXTENSION_API void ModInitialize(void)
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
		//Con::addMethodB(NULL,"setAIMove",&consetMove,"setAIMove(%aicon, x, y, z, yaw, pitch, roll)", 2,10);
		//Con::addMethodB(NULL,"setAITrigger", &consetTrigger, "setAITrigger(%aicon,triggerid,value);",2,6);
		Con::addMethodS("GrenadeProjectile", "getposition", &conGrenadeProjectileGetPosition,"Accurately gets the position of the GrenadeProjectile", 2, 2);
		Con::addMethodS("GrenadeProjectile", "getvelocity", &conGrenadeProjectileGetVelocity,"Gets the velocity of the GrenadeProjectile", 2, 2);
		Con::addMethodB("Projectile", "makeNerf", &conProjectileMakeNerf,"Makes the Projectile deal no damage", 2, 2);

		// TCPObject
#ifdef ENABLE_TCPOBJECT
		/*
		Con::addMethodS("TCPObject", "connect", &conTCPObjectConnect, "Connects to a remote server", 3, 3);
		Con::addMethodB("TCPObject", "send", &conTCPObjectSend, "Sends data to the remote server", 3, 3);
		Con::addMethodB("TCPObject", "disconnect", &conTCPObjectDisconnect, "Disconnects from the remote server", 2, 2);

		// HTTPObject
		Con::addMethodB("HTTPObject", "get", &conHTTPObjectDoNothing, "Disconnects from the remote server", 6, 6);
		Con::addMethodB("HTTPObject", "post", &conHTTPObjectDoNothing, "Disconnects from the remote server", 6, 6);
		Con::addMethodB("HTTPObject", "send", &conHTTPObjectDoNothing, "Disconnects from the remote server", 6, 6);
		Con::addMethodB("HTTPObject", "connect", &conHTTPObjectDoNothing, "Disconnects from the remote server", 6, 6);
		Con::addMethodB("HTTPObject", "listen", &conHTTPObjectDoNothing, "Disconnects from the remote server", 6, 6);
		*/
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
		// memPatch Disabler
		Con::addMethodB(NULL, "disableMempatch", &disableMempatch,"Disables memPatch.", 1, 1);
		// Regex
		Con::addMethodB(NULL, "reSearch", &reSearch,"reSearch(pattern, target): Searches for a pattern within the target string.", 3, 3);
		Con::addMethodB(NULL, "reMatch", &reMatch,"reMatch(pattern, target): Attempts to match the entire target string to a pattern.", 3, 3);
		Con::addMethodB(NULL, "reIterBegin", &reIterBegin,"reIterBegin(pattern, target): Begins an iterator search for patterns in the target string.", 3, 3);
		Con::addMethodB(NULL, "reIterEnd", &reIterEnd,"reIterEnd(): Returns true when the iterator search ends.", 1, 1);
		Con::addMethodS(NULL,"reIterNext",&reIterNext,"reIterNext(): Returns the next matched pattern in the string.", 1, 1);
		Con::addMethodS(NULL,"reReplace",&reReplace,"reReplace(pattern, target, replace): Replaces the pattern within the target string with another string.", 4, 4);

		// GUI projection
		Con::addMethodS("GuiTSCtrl","project",&conGuiTsCtrlProject,"projects a world space vector to on-screen position",3,3);

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
#ifdef MECH_MOVE_CODE
		*((unsigned int*)(mechcode+1))=(unsigned int)&updatemovehookptr;
#endif
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


		return;
	}

	TSEXTENSION_API const char* getManagementName(void) 
	{
		return "TSExtension";
	}

	TSEXTENSION_API ModLoader::ModLoaderCallables* getModCallables(void)
	{
		ModLoader::ModLoaderCallables* result = new ModLoader::ModLoaderCallables();
		result->mInitializeModPointer = ModInitialize;
		result->mGetManagementName = getManagementName;
		return result; 
	}
}

