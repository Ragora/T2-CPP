// dllmain.cpp : Defines the entry point for the DLL application.
#include <SDKDDKVer.h>
#include <Windows.h>
#include <time.h>
#include <stdio.h>
#include <conio.h>

#include <LinkerAPI.h>

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
DWORD WINAPI WatchDogThread(LPVOID lpParam)
{
	time_t lastPet = time(0);
	CONTEXT hamburger;
	HANDLE mainThread = OpenThread(THREAD_ALL_ACCESS,false,mainthreadid);
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
			Con::printf ("Please press enter to try to continue\n  or wait for 30 more seconds to kill T2 and write log\n");
			Sleep(30000);
			if (_kbhit()) {
				sDogPetted=true;
				_getch(); // make sure to clean the keyboard buffer
				ResumeThread(mainThread);
			} else {
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

