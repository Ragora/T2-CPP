#include <LinkerAPI.h>

//SimIdDictionary* gIdDictionary = reinterpret_cast<SimIdDictionary*>(0x009E9194);

//439550
namespace Sim {
	Linker::SimObject* (*findObject)(U32 id) = 
		(Linker::SimObject* (_cdecl *)(U32 id) )
		0x439550;
	Linker::SimObject* (*findObjectc)(const char* name) = 
		(Linker::SimObject* (_cdecl *)(const char* name) )
		0x439450;
}

//hackey way to do member functions, ....
void GuiTSCtrl_project(GuiTSCtrl *obj, const Linker::Point3F &pt, Linker::Point3F *dest) {
	typedef void (__cdecl *projFunc)(const Linker::Point3F &pt, Linker::Point3F *dest);
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

namespace Con 
{

char* (*getReturnBuffer)(U32 bufferSize) = 
	(char *(__cdecl *)(U32))
	0x42caa0;
const char * (*getMatrixRotation)(void * matptr, unsigned int *, unsigned int) = (const char * (__cdecl *)(void *, unsigned int *, unsigned int)) 0x5503A0;
void (*setMatrixRotation)(void * matptr, S32 argc, const char **argv, unsigned int *, unsigned int) = (void (__cdecl *)(void *, S32, const char **, unsigned int *, unsigned int)) 0x550420;
const char * (*getMatrixPosition)(void * matptr, unsigned int *, unsigned int) = (const char * (__cdecl *)(void *, unsigned int *, unsigned int)) 0x550260;
void (*setMatrixPosition)(void * matptr, S32 argc, const char **argv, unsigned int *, unsigned int) = (void (__cdecl *)(void *, S32, const char **, unsigned int *, unsigned int)) 0x550300;

void (*addMethodI)(const char *nsName, const char *name, IntCallback   cb,   const char *usage, S32 minArgs, S32 maxArgs) = 
	(void (__cdecl *)(const char *, const char *,IntCallback,const char *,S32,S32))
	0x426490;

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

const char * (*executem)(Linker::SimObject *object, S32 argc, const char *argv[]) = 
	(const char * (__cdecl *)(Linker::SimObject *object, S32 argc, const char *argv[]))
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