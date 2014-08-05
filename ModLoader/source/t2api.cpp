#include <t2api.h>

SimIdDictionary* gIdDictionary = reinterpret_cast<SimIdDictionary*>(0x009E9194);

//439550
namespace Sim {
	SimObject* (*findObject)(U32 id) = 
		(SimObject* (_cdecl *)(U32 id) )
		0x439550;
}

//hackey way to do member functions, ....
void GuiTSCtrl_project(GuiTSCtrl *obj, const Point3F &pt, Point3F *dest) {
	typedef void (__cdecl *projFunc)(const Point3F &pt, Point3F *dest);
	static projFunc p = (projFunc)0x4d0b40;

	__asm {
		push dest;
		push pt;
		mov ecx,obj;
		lea eax, p;
		mov eax, [eax];
		call eax;
	}
}

void (*someTest)(void) =
	(void (__cdecl *)(void))
	0x4FD9B3;

namespace Con {

char* (*getReturnBuffer)(U32 bufferSize) = 
	(char *(__cdecl *)(U32))
	0x42caa0;

void (*addMethodB)(const char *nsName, const char *name, BoolCallback   cb,   const char *usage, S32 minArgs, S32 maxArgs) = 
	(void (__cdecl *)(const char *, const char *,BoolCallback,const char *,S32,S32))
	0x426510;

void (*addMethodS)(const char *, const char *,StringCallback, const char *, S32, S32) = 
	(void (__cdecl *)(const char *,const char *,StringCallback,const char *,S32,S32))
	0x426410;

bool (*addVariable)(const char *name, S32 t, void *dp) = 
	(bool (__cdecl *)(const char *name, S32 t, void *dp))
	0x4263B0;

const char * (*execute)(S32 argc, const char *argv[]) =
	(const char * (__cdecl *)(S32 argc, const char *argv[]))
	0x4267A0;

const char * (*executef)(S32 argc, ...) =
	(const char * (__cdecl *)(S32 argc, ...))
	0x4269E0;

const char * (*evaluate)(const char* string, bool echo, const char *fileName, bool cf) = 
	(const char * (__cdecl *)(const char* string, bool echo, const char *fileName, bool cf))
	0x426690;

const char * (*executem)(SimObject *object, S32 argc, const char *argv[]) = 
	(const char * (__cdecl *)(SimObject *object, S32 argc, const char *argv[]))
	0x426800;

const char * (*getVariable)(const char *name) = 
	(const char * (__cdecl *)(const char *name))
	0x4261F0;

void (*printf)(const char* fmt,...) = 
	(void (__cdecl *)(const char* fmt,...))
	0x425F30;

void (*errorf)(U32 type, const char* fmt,...)=
	(void (__cdecl *)(U32, const char*,...))
	0x425FB0;

}

int (*dSscanf)(const char *buffer, const char *format, ...) = 
	(int (__cdecl *)(const char *,const char *,...))
	0x55b640;

int (*dSprintf)(char *buffer, dsize_t bufferSize, const char *format, ...) =
	(int (__cdecl *)(char *,dsize_t,const char *,...))
	0x55b5e0;

bool (*dAtob)(const char *str) =
	(bool (__cdecl *)(const char *str))
	0x55B490;
