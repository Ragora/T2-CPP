// BaseMod.cpp : Defines the exported functions for the DLL application.
//

#include "BaseMod.h"

#include "t2api.h"
#include "randomCmds.h"

extern "C"
{
	BASEMOD_API void ModInitialize(void)
	{
		Con::addMethodS(NULL,"getRandom",&conMRandom,"Random number generation based upon the Mersenne Twister algorithm.",1,3);
		Con::addMethodS(NULL,"seedRand",&conMSeed,"Reseeds the Mersenne Twister random number generator.",2,2);
		Con::addMethodS(NULL,"getRandomFloat",&conMRandomFloat,"Random number generation based upon the Mersenne Twister algorithm.",1,3);
		Con::addMethodS(NULL,"QPCBegin",&conQPCBegin,"Begins the performance counter.",1,1);
		Con::addMethodS(NULL,"QPCEnd",&conQPCEnd,"Ends the performance counter and returns the time in seconds.",2,2);

		return;
	}
}