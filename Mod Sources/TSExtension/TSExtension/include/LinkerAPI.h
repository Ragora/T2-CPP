// Linker's Tribes 2 API

#pragma once

//api stuff

typedef unsigned int U32;
typedef int S32;
typedef float F32;

typedef unsigned int  dsize_t;
#include <string>

//for addvariable
#define TypeS32 1
#define TypeBool 3
#define TypeF32 5

//dshit
inline dsize_t dStrlen(const char *str)
{
   return (dsize_t)strlen(str);
}

//class Namespace 
//{
//	const char* mName;
//};

//class SimIdDictionary
//{
   //enum
   //{
   //  DefaultTableSize = 4096,
    //  TableBitMask = 4095
  // };
//   Linker::SimObject *table[DefaultTableSize];
//};
//extern SimIdDictionary* gIdDictionary;

namespace Linker
{
	class Point3F
	{
		public:
		Point3F(F32 *x, F32 *y, F32 *z)
		{
			this->x = x;
			this->y = y;
			this->z = z;
		}

		Point3F(void)
		{
			this->x = 0;
			this->y = 0;
			this->z = 0;
		}

		F32 *x;
		F32 *y;
		F32 *z;
	};

	struct SimObject
	{
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
}

//GuiTSCtrl
class GuiTSCtrl {};
void GuiTSCtrl_project(GuiTSCtrl *obj, const Linker::Point3F &pt, Linker::Point3F *dest); //fake

namespace Sim {
	extern Linker::SimObject* (*findObject)(U32 id);
}

//console

typedef const char * (*StringCallback)(Linker::SimObject *obj, S32 argc, const char *argv[]);
typedef S32             (*IntCallback)(Linker::SimObject *obj, S32 argc, const char *argv[]);
typedef F32           (*FloatCallback)(Linker::SimObject *obj, S32 argc, const char *argv[]);
typedef void           (*VoidCallback)(Linker::SimObject *obj, S32 argc, const char *argv[]);
typedef bool           (*BoolCallback)(Linker::SimObject *obj, S32 argc, const char *argv[]);


//functions
namespace Con
{

extern char *	(*getReturnBuffer)(U32 bufferSize);

extern void (*addMethodB)(const char *nsName, const char *name, BoolCallback   cb,   const char *usage, S32 minArgs, S32 maxArgs);
extern void (*addMethodS)(const char *nsName, const char *name, StringCallback cb, const char *usage, S32 minArgs, S32 maxArgs);

extern bool (*addVariable)(const char *name, S32 t, void *dp);

extern void (*printf)(const char* fmt,...);
extern void (*errorf)(U32 type, const char* fmt,...);

extern const char * (*getVariable)(const char *name);
extern const char * (*execute)(S32 argc, const char *argv[]);
extern const char * (*executef)(S32 argc, ...);
extern const char * (*executem)(Linker::SimObject *object, S32 argc, const char *argv[]);
extern const char * (*evaluate)(const char* string, bool echo, const char *fileName, bool cf);
}

//d-util
extern int (*dSscanf)(const char *buffer, const char *format, ...);
extern int (*dSprintf)(char *buffer, dsize_t bufferSize, const char *format, ...);
extern bool (*dAtob)(const char *str);