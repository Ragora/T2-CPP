#pragma once

#include <string.h>

void initT2Api();


//api stuff

typedef unsigned int U32;
typedef int S32;
typedef float F32;

typedef unsigned int  dsize_t;

//for addvariable
#define TypeS32 1
#define TypeBool 3
#define TypeF32 5


//dshit
inline dsize_t dStrlen(const char *str)
{
   return (dsize_t)strlen(str);
}

class Namespace {
	const char* mName;
};



class SimObject{
	SimObject* group;
	const char* objectName; //04h: objectName
	SimObject*       nextNameObject;	//8
  SimObject*       nextManagerNameObject; //c
	SimObject* nextIdObject;	//10h: nextIdObject
	U32 stuff; //14
	U32 mFlags; //18h
	U32 mNotifyList; //actually a pointer
	U32 mId; //20h: mId
	//more stuff
};

class SimIdDictionary
{
   enum
   {
      DefaultTableSize = 4096,
      TableBitMask = 4095
   };
   SimObject *table[DefaultTableSize];
};
extern SimIdDictionary* gIdDictionary;

class Point3F
{
  public:
   F32 x;
   F32 y;
   F32 z;
};


//GuiTSCtrl
class GuiTSCtrl {};
class HoverVehicle {};
void GuiTSCtrl_project(GuiTSCtrl *obj, const Point3F &pt, Point3F *dest); //fake


namespace Sim {
	extern SimObject* (*findObject)(U32 id);
}


//console

typedef const char * (*StringCallback)(SimObject *obj, S32 argc, const char *argv[]);
typedef S32             (*IntCallback)(SimObject *obj, S32 argc, const char *argv[]);
typedef F32           (*FloatCallback)(SimObject *obj, S32 argc, const char *argv[]);
typedef void           (*VoidCallback)(SimObject *obj, S32 argc, const char *argv[]);
typedef bool           (*BoolCallback)(SimObject *obj, S32 argc, const char *argv[]);


extern void (*someTest)(void);
//functions
namespace Con{

extern char *	(*getReturnBuffer)(U32 bufferSize);

extern void (*addMethodB)(const char *nsName, const char *name, BoolCallback   cb,   const char *usage, S32 minArgs, S32 maxArgs);
extern void (*addMethodS)(const char *nsName, const char *name, StringCallback cb, const char *usage, S32 minArgs, S32 maxArgs);

extern bool (*addVariable)(const char *name, S32 t, void *dp);

extern void (*printf)(const char* fmt,...);
extern void (*errorf)(U32 type, const char* fmt,...);

extern const char * (*getVariable)(const char *name);
extern const char * (*execute)(S32 argc, const char *argv[]);
extern const char * (*executef)(S32 argc, ...);
extern const char * (*executem)(SimObject *object, S32 argc, const char *argv[]);
extern const char * (*evaluate)(const char* string, bool echo, const char *fileName, bool cf);
}

//d-util
extern int (*dSscanf)(const char *buffer, const char *format, ...);
extern int (*dSprintf)(char *buffer, dsize_t bufferSize, const char *format, ...);
extern bool (*dAtob)(const char *str);