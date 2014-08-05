#include "stdafx.h"
#include <math.h>
#include <cstdlib>
#include <stdio.h>
#include "mt.h"

#include "t2api.h"

// Merscenne Twister
MersenneTwister *mt = NULL;
const char *conMRandom(SimObject *obj, S32 argc, const char *argv[])
{
	if (!mt)
	{
		mt = new MersenneTwister;
		mt->init_genrand(time(NULL));
	}

	char mem[256];
	memset(mem, 0x00, 255);
	if (argc == 1)
	{
		sprintf_s(mem, 256, "%f", mt->random());
		return mem;
	}

	char *a_c = (char*)argv[1];
	char *b_c;
	if (argc == 3)
		b_c = (char*)argv[2];
	else if (argc == 2)
	{
		b_c = a_c;
		a_c = "0";
	}

	int a = atoi(a_c);
	int b = atoi(b_c);

	// Check to see if we have any negative arguments
	if (a < 0)
	{
		 int max = b + abs(a);
		 int rand = mt->genrand_int32() % (max);
		 if (rand > b)
			 rand = -(rand-b);
		sprintf_s(mem, 256, "%d", rand);
		 return mem;
	}

	sprintf_s(mem, 256, "%f", floor(mt->random() * b + a));
	return mem;
}

const char *conMRandomFloat(SimObject *obj, S32 argc, const char *argv[])
{
	if (!mt)
	{
		mt = new MersenneTwister;
		mt->init_genrand(time(NULL));
	}

	char mem[256];
	memset(mem, 0x00, 255);
	if (argc == 1)
	{
		sprintf_s(mem, 256, "%f", mt->random());
		return mem;
	}

	char *a_c = (char*)argv[1];
	char *b_c;
	if (argc == 3)
		b_c = (char*)argv[2];
	else if (argc == 2)
	{
		b_c = a_c;
		a_c = "0";
	}

	float a = atof(a_c);
	float b = atof(b_c);
	if (a > b)
	{
		int d = b;
		b = a;
		a = d;
	}

	sprintf_s(mem, 256, "%f", mt->random() * b + a);
	return mem;
}

const char *conMSeed(SimObject *obj, S32 argc, const char *argv[])
{
	if (!mt)
	{
		mt = new MersenneTwister;
		mt->init_genrand(time(NULL));
	}
	const char *a_c = argv[1];

	unsigned long a = 0;
	unsigned int len = strlen(a_c);
	for (unsigned int i = 0; i < len; i++)
		if ( a_c[i] > 57 || a_c[i] < 48)
		{
			mt->init_by_array((unsigned long*)a_c, len+1);
			return "1";
		}
	
	a = atof(a_c);
	mt->init_genrand(a);
	return "1";
}


#define QPC_MAX_STOPWATCHES 20

static unsigned int CURRENT_STOPWATCH_COUNT = 0;
static __int64 QPC_STOPWATCHES[QPC_MAX_STOPWATCHES];

__int64 STARTING_QPC_TIME;
const char*conQPCBegin(SimObject *obj, S32 argc, const char *argv[])
{
	if (!QueryPerformanceCounter((LARGE_INTEGER*)&QPC_STOPWATCHES[CURRENT_STOPWATCH_COUNT]))
	{
		Con::errorf(0, "QPCBegin(): Failed to start timer! (%u)", GetLastError());
		return "-1";
	}
	else if (CURRENT_STOPWATCH_COUNT >= QPC_MAX_STOPWATCHES)
	{
		Con::errorf(0, "QPCBegin(): Too many stopwatches active! (Maximum: %u)", QPC_MAX_STOPWATCHES);
		return "-1";
	}

	unsigned int return_value = CURRENT_STOPWATCH_COUNT;
	char result[256];
	sprintf_s<256>(result, "%u", return_value);

	CURRENT_STOPWATCH_COUNT++;

	return result;
}

const char*conQPCEnd(SimObject *obj, S32 argc, const char *argv[])
{
	__int64 ENDING_QPC_TIME;
	if (!QueryPerformanceCounter((LARGE_INTEGER*)&ENDING_QPC_TIME))
	{
		Con::errorf(0, "QPCEnd(): Failed to stop timer! (%u)", GetLastError());
		return "-1";
	}
	else if (CURRENT_STOPWATCH_COUNT == 0)
	{
		Con::errorf(0, "QPCEnd(): No stopwatches to stop!");
		return "-1";
	}

	__int64 qpc_frequency;
	QueryPerformanceFrequency((LARGE_INTEGER*)&qpc_frequency);

	char result[256];
	sprintf_s<256>(result, "%f", ((QPC_STOPWATCHES[CURRENT_STOPWATCH_COUNT - 1]) - ENDING_QPC_TIME) * 1.0 / qpc_frequency);

	CURRENT_STOPWATCH_COUNT--;

	return result;
}