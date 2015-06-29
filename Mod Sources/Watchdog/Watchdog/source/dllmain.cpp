// dllmain.cpp : Defines the entry point for the DLL application.
#include <SDKDDKVer.h>
#include <Windows.h>
#include <time.h>
#include <stdio.h>
#include <conio.h>
#include <LinkerAPI.h>

// #define LONGHAMBURGERROUTINE

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

static bool sDogPetted = false;
static DWORD mainthreadid=0;
static bool evaldone=1;
void overrideputhex(unsigned char hex) {
	char hexstr[40]="";
	char outchar=' ';
	int counter=0;
	sprintf (hexstr,"%02X",hex);
	while (hexstr[counter] != 0x0) {
		_putch(hexstr[counter]);
		counter++;
	}
}
void overridegets(char * string) {
	int counter=0;
	char outstr[3]="";
	while (1) {
		if (_kbhit()) {
			string[counter]=_getch();
			_putch(string[counter]);
			if (string[counter]=='\n' || string[counter]=='\r') {
				string[counter]=0x0;
				break;
			} else {
				counter++;
			}
		} else {
			Sleep(4);
		}
	}
}
DWORD WINAPI WatchDogEvalThread(LPVOID lpParam)
{
	Con::evaluate((char *) lpParam,true,0,0);
	Con::printf ("Finished executing\n");
	evaldone=1;
	return 0;
}
DWORD WINAPI WatchDogThread(LPVOID lpParam)
{
	time_t lastPet = time(0);
	CONTEXT hamburger;
	HANDLE mainThread = OpenThread(THREAD_ALL_ACCESS,false,mainthreadid);
	char inputchar=0;
	char evalcode[5000]="";
	while (true)
	{
		time_t now = time(0);
		double seconds = difftime(now,lastPet);
#ifdef SLOWSERVER
		if (!sDogPetted && seconds > 15) // Wait 15 seconds to be extra safe
#else
		if (!sDogPetted && seconds > 8) // Wait 8 seconds to be safe
#endif
		{
			SuspendThread(mainThread);
			hamburger.ContextFlags=CONTEXT_FULL; // Make sure to select which parts of the context to dump
			GetThreadContext(mainThread, &hamburger); // dog grabs hamburger;
			Con::printf ("Dog has grabbed the hamburger again\n");
			Con::printf ("Either grab Hamburger back and put dog back on leash, or let dog eat hamburger\n");
			Con::printf ("the following text is from the Processor State\n");
			FILE * wlog = fopen ("watchdog.log","a");
			fprintf (wlog,"EIP: %08X    EAX: %08X    EBX: %08X    ECX: %08X    \nEDX: %08X    ESI: %08X    EDI: %08X\nEBP:%08X    ESP:%08X\n", hamburger.Eip, hamburger.Eax, hamburger.Ebx, hamburger.Ecx, hamburger.Edx, hamburger.Esi, hamburger.Edi, hamburger.Ebp, hamburger.Esp);
			fclose (wlog);
			Con::printf ("EIP: %08X    EAX: %08X    EBX: %08X    ECX: %08X    \nEDX: %08X    ESI: %08X    EDI: %08X\nEBP:%08X    ESP:%08X\n", hamburger.Eip, hamburger.Eax, hamburger.Ebx, hamburger.Ecx, hamburger.Edx, hamburger.Esi, hamburger.Edi, hamburger.Ebp, hamburger.Esp);
			Con::printf ("Please press enter to try to continue, press e to get a torquescript shell, press d to get a hexdump of the last bit of torquescript bytecode to search for in DSO files, or wait for 30 more seconds to kill T2 and write log\n");
#ifdef LONGHAMBURGERROUTINE
			for (int secondcounter=0;secondcounter<120;secondcounter++) {
#else		
			for (int secondcounter=0;secondcounter<8;secondcounter++) {
#endif
				Sleep(250);
				if (_kbhit()) {
					sDogPetted=true;
					inputchar=_getch(); // make sure to clean the keyboard buffer
					if (inputchar=='e') {
						Con::printf ("Torque script shell activated enter the code to evaluate on the next line to exit, just type exitshell and press enter\n");
						while (true) {
							overridegets(evalcode);
							if (strcmp(evalcode,"exitshell")==0) {
								break;
							}
							DWORD threadID=0;
							evaldone=0;
							HANDLE thread = CreateThread(NULL, 0, WatchDogEvalThread, evalcode, 0, &threadID);
							while (!evaldone) {
								Sleep(256);
							}
							CloseHandle(thread);
						}
						
					} else if (inputchar=='d') {
						DWORD codeOffset=*(DWORD *)(0xa3b714);
						DWORD codeBase=*(DWORD *)(0xa3b710);
						if (codeBase!=0) {
							unsigned char *basecodeptr=(unsigned char *)((codeBase+(codeOffset*4)));
							Con::printf ("CodeOffset: %08X    CodeAddr: %08X    (Codeoffset-128 to CodeOffset+512 is on the next line)\n",codeOffset, (codeBase+(codeOffset*4)));
							
							
							for (int codeOffsetCounter=-128; codeOffsetCounter<512; codeOffsetCounter++) {
								overrideputhex(*(basecodeptr+codeOffsetCounter));
								_putch(' ');
							}
							_putch('\n');
							_putch('\r');
							Con::printf("Search for those hex bytes in a binary search utility to find the DSO file with the last executed opcodes\n");
						} else {
							Con::printf ("you forgot to run this command: memPatch(\"42CED1\",getInterAddr());\n");
						}
					}
					ResumeThread(mainThread);
					break;
				}
			}
			
			if (!sDogPetted) {
			CloseHandle(mainThread);
			exit(0);
			}
		}
		else if (sDogPetted)
		{
			sDogPetted = false;
			lastPet = now;
		}
		Sleep(500);
	}
}

extern "C"
{
	__declspec(dllexport) void ModInitialize(void)
	{
	   DWORD *codeOffset=(DWORD *)(0xa3b714);
	   DWORD *codeBase=(DWORD *)(0xa3b710);
	   *codeOffset=0x0;
	   *codeBase=0x0;
	   mainthreadid = GetThreadId(GetCurrentThread());
	   SECURITY_DESCRIPTOR secDescVar;
	   DWORD threadID;
	   HANDLE token;
	   LUID debugname;
	   TOKEN_PRIVILEGES tokenpriv;
	   HANDLE thread = CreateThread(NULL, 0, WatchDogThread, NULL, 0, &threadID);
	   if (thread==NULL) {
		    DWORD errorval = GetLastError();
			Con::printf ("Error %08X, watchdog is not running properly\n",errorval);
	   }
	   OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,&token);

	   LookupPrivilegeValue(NULL,SE_DEBUG_NAME, &debugname);
	   tokenpriv.PrivilegeCount = 1;
	   tokenpriv.Privileges[0].Luid = debugname;
	   tokenpriv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	   if(AdjustTokenPrivileges(token, FALSE, &tokenpriv, sizeof tokenpriv, NULL, NULL)) {
			Con::printf ("watchdog should work now\n");
			CloseHandle(token);
	   } else {
			Con::printf ("Error, watchdog won't operate fully, WatchDog needs Admin Privileges for Debug\n");
	   }

	}

	__declspec(dllexport) void ServerProcess(unsigned int deltaTime)
	{
		sDogPetted = true;
	}
}

