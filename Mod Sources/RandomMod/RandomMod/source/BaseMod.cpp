// BaseMod.cpp : Defines the exported functions for the DLL application.
//

#include "BaseMod.h"
#include <ModLoader\ModLoader.h>
#include "LinkerAPI.h"
#include "randomCmds.h"

extern "C"
{
	BASEMOD_API unsigned int getModLoaderVersion(void)
	{
		return 0;
	}

	BASEMOD_API void ModInitialize(void)
	{
		Con::addMethodS(NULL,"getRandom",&conMRandom,"Random number generation based upon the Mersenne Twister algorithm.",1,3);
		Con::addMethodS(NULL,"seedRand",&conMSeed,"Reseeds the Mersenne Twister random number generator.",2,2);
		Con::addMethodS(NULL,"getRandomFloat",&conMRandomFloat,"Random number generation based upon the Mersenne Twister algorithm.",1,3);
		Con::addMethodS(NULL,"QPCBegin",&conQPCBegin,"Begins the performance counter.",1,1);
		Con::addMethodS(NULL,"QPCEnd",&conQPCEnd,"Ends the performance counter and returns the time in seconds.",2,2);

		return;
	}

	BASEMOD_API const char* getManagementName(void) 
	{
		return "RandomMod";
	}

	BASEMOD_API ModLoader::ModLoaderCallables* getModCallables(void)
	{
		ModLoader::ModLoaderCallables* result = new ModLoader::ModLoaderCallables();
		result->mInitializeModPointer = ModInitialize;
		result->mGetManagementName = getManagementName;
		return result; 
	}


}