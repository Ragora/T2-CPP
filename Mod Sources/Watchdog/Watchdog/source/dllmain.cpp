// dllmain.cpp : Defines the entry point for the DLL application.
#include <SDKDDKVer.h>
#include <Windows.h>
#include <time.h>

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
static HANDLE sMainThread = NULL;
DWORD WINAPI WatchDogThread(LPVOID lpParam)
{
	time_t lastPet = clock();

	while (true)
	{
		time_t now = clock();
		time_t seconds = (now - lastPet) / CLOCKS_PER_SEC;

		if (!sDogPetted && seconds > 8) // Wait 8 seconds to be safe
		{
			CloseHandle(sMainThread);
			exit(0);
		}
		else if (sDogPetted)
		{
			sDogPetted = false;
			lastPet = now;
		}
	}
}

extern "C"
{
	__declspec(dllexport) void ModInitialize(void)
	{
	   sMainThread = GetCurrentThread();

	   DWORD threadID;

	   HANDLE thread = CreateThread(NULL, 0, WatchDogThread, NULL, 0, &threadID);
	}

	__declspec(dllexport) void ServerProcess(unsigned int deltaTime)
	{
		sDogPetted = true;
	}
}

