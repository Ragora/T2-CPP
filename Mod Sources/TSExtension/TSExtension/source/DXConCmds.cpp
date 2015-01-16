/**
 *	
 */
#define endian(hex) (((hex & 0x000000FF) << 24)+((hex & 0x0000FF00) << 8)+((hex & 0x00FF0000)>>8)+((hex & 0xFF000000) >> 24))
#include <LinkerAPI.h>
#include <DXAPI/DXAPI.h>
void serverProcessReplacement(unsigned int timeDelta) {
	unsigned int servertickaddr=0x602350;
	unsigned int serverthisptr=0x9E5EC0;
	__asm 
	{
		mov ecx,serverthisptr
		push timeDelta
		call servertickaddr
	}
	
	return;
}
const char* congetServPAddr(Linker::SimObject *obj, S32 argc, const char *argv[]) {
		char test[256] = "";
		char test2[256]="";
		int spr=(signed int)*serverProcessReplacement;
		sprintf(test,"B8%8XFFD089EC5DC3",endian(spr));
		test2[0]=test[6];
		test2[1]=test[7];
		test2[2]=test[4];
		test2[3]=test[5];
		test2[4]=test[2];
		test2[5]=test[3];
		test2[6]=test[0];
		test2[7]=test[1];
		test2[8]=0;
		return test2;
}
const char *conGetAddress(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	// Hmm...
	char result[256];
	sprintf(result, "%x", obj);
	return result;
}

bool conPlayerGetJumpingState(Linker::SimObject *obj, S32 argc, const char* argv[])
{
	DX::Player operand = DX::Player((unsigned int)obj);

	return operand.is_jumping;
}
bool conPlayerGetJettingState(Linker::SimObject *obj, S32 argc, const char* argv[])
{
	DX::Player operand = DX::Player((unsigned int)obj);

	return operand.is_jetting;
}

bool conGameConnectionSetHeatLevel(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	DX::GameConnection operand = DX::GameConnection((unsigned int)obj);
	operand.getControlObject().heat_level = atof(argv[1]);
	return true;
}

bool conProjectileExplode(Linker::SimObject *obj, S32 argc, const char* argv[])
{
	Linker::Point3F position(0,0,0);
	Linker::Point3F normal(0,0,0);

	unsigned int collideType = atoi(argv[4]);
	DX::Projectile projectile((unsigned int)obj);

	projectile.net_flags |= DX::NetFlags::ScopeAlways;
	projectile.explode(position, normal, collideType);

	return true;
}

bool conProjectileMakeNerf(Linker::SimObject *obj, S32 argc, const char* argv[])
{
	DX::GrenadeProjectile grenade = DX::GrenadeProjectile((unsigned int)obj);
	grenade.hidden = true;

	return true;
}
bool conForceUpdate(Linker::SimObject *obj, S32 argc, const char* argv[]) {
	DX::NetConnection conn = DX::NetConnection((unsigned int)obj);
	DX::NetObject netobj = DX::NetObject((unsigned int)Sim::findObjectc(argv[2]));
	GhostInfo * mGhostRefs=conn.mGhostRefs;
	if (netobj.base_pointer_value!=0) {
	S32 index = conn.getGhostIndex(netobj);
		if (index > 0) {
			mGhostRefs[index].updateMask=mGhostRefs[index].updateMask | GameBaseMasks::InitialUpdateMask;
		}
		return 1;
	}
	else 
	{
		return 0;
	}
}
const char* conGetGhostIndex(Linker::SimObject *obj, S32 argc, const char* argv[]) {
	char outint[20]="";
	DX::NetConnection conn = DX::NetConnection((unsigned int)obj);
	DX::NetObject netobj = DX::NetObject((unsigned int)Sim::findObjectc(argv[2]));
	if (netobj.base_pointer_value!=0) {
		S32 index = conn.getGhostIndex(netobj);
		itoa(index,outint,10);
		return outint;
	} else {
		return "";
	}
}
const char* conResolveGhost(Linker::SimObject *obj, S32 argc, const char* argv[]) {
	char outint[20]="";
	DX::NetConnection conn = DX::NetConnection((unsigned int)obj);
	S32 id = atoi(argv[2]);
	DX::NetObject realobject = conn.resolveGhost(id);
	if (realobject.base_pointer_value) {
		return itoa(realobject.identifier,outint,10);
	}
	return "";
}
const char* conResolveGhostParent(Linker::SimObject *obj, S32 argc, const char* argv[]) {
	char outint[20]="";
	DX::NetConnection conn = DX::NetConnection((unsigned int)obj);
	S32 ghostindex = atoi(argv[2]);
	if (conn.base_pointer_value!=0) {
		if (conn.resolveGhostParent(ghostindex).base_pointer_value)
			{
				S32 objid = conn.resolveGhostParent(ghostindex).identifier;
				if (objid != 0) {
					itoa(objid,outint,10);
					return outint;
				}
			}
	}
		return "";
}
bool conclientCmdSetGhostTicks(Linker::SimObject *obj, S32 argc, const char* argv[]) {
		unsigned int result_ptr = 0;
		unsigned int my_ptr = 0;
		unsigned int ghostindex=atoi(argv[1]);
		DX::NetConnection conn = DX::NetConnection((unsigned int)Sim::findObjectc("ServerConnection"));
		if (conn.base_pointer_value) {
			DX::NetObject ghostobj = conn.resolveGhost(ghostindex);
			if (ghostobj.base_pointer_value) 
			{ 
				my_ptr=(unsigned int)ghostobj.base_pointer_value;
				if (atoi(argv[2])==1) {
					__asm
					{
						mov eax, my_ptr;
						add eax, 0x264;
						mov ebx,eax
						mov al, 1
						mov [ebx],al
						
					}	
				} else {
					__asm
					{
						mov eax, my_ptr;
						add eax, 0x264;
						mov ebx,eax
						mov al, 0
						mov [ebx],al
						
					}	
				}
			}
		}
		return 1;
}
bool conclientCmdSetProcessTicks(Linker::SimObject *obj, S32 argc, const char* argv[]) {
		unsigned int result_ptr = 0;
		unsigned int my_ptr = 0;
		DX::NetObject objptr=(unsigned int)Sim::findObjectc(argv[1]);
		if (objptr.base_pointer_value) 
		{ 
			my_ptr=objptr.base_pointer_value;
			if (atoi(argv[2])==1) {
				__asm
				{
					mov eax, my_ptr;
					add eax, 0x264;
					mov ebx,eax
					mov al, 1
					mov [ebx],al
					
				}	
			} else {
				__asm
				{
					mov eax, my_ptr;
					add eax, 0x264;
					mov ebx,eax
					mov al, 0
					mov [ebx],al
					
				}	
			}
		}
			return 1;
}
bool conSetProcessTicks(Linker::SimObject *obj, S32 argc, const char* argv[]) {

			unsigned int result_ptr = 0;
		unsigned int my_ptr = (unsigned int) obj;
		if (atoi(argv[2])==1) {
			__asm
			{
				mov eax, my_ptr;
				add eax, 0x264;
				mov ebx,eax
				mov al, 1
				mov [ebx],al
				
			}	
		} else {
			__asm
			{
				mov eax, my_ptr;
				add eax, 0x264;
				mov ebx,eax
				mov al, 0
				mov [ebx],al
				
			}	
		}

}
const char* conGrenadeProjectileGetPosition(Linker::SimObject *obj, S32 argc, const char* argv[])
{
	char result[256];

	DX::GrenadeProjectile grenade = DX::GrenadeProjectile((unsigned int)obj);
	sprintf_s<256>(result, "%f %f %f", grenade.position.x, grenade.position.y, grenade.position.z);
	return result;
}

const char* conGrenadeProjectileGetVelocity(Linker::SimObject *obj, S32 argc, const char* argv[])
{
	char result[256];

	DX::GrenadeProjectile grenade((unsigned int)obj);
	sprintf_s<256>(result, "%f %f %f", grenade.velocity.x, grenade.velocity.y, grenade.velocity.z);

	return result;
}

// General Commands ---------------------------------
#include <cstdarg>
#include <vector>
#include <string.h>

const char* conSprintf(Linker::SimObject *obj, S32 argc, const char* argv[])
{
	std::vector<const char*> input;
	for (unsigned int i = 2; i < argc; i++)
		input.push_back(argv[i]);

	char result[256];
	
	va_list variable_args = reinterpret_cast<va_list>(input.data());
	vsprintf(result, argv[1], variable_args);

	return result;
}