/**
 *	
 */

#include <LinkerAPI.h>
#include <DXAPI/DXAPI.h>

const char *conDumpHex(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	// Hmm...
	char result[256];

	unsigned int addr=atoi(argv[1]);
	DX::memToHex(addr,result,atoi(argv[2]),dAtob(argv[3]));
	return result;

}
const char *conDumpUInt(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	// Hmm...
	char result[256];

	unsigned int addr=atoi(argv[1]);
	sprintf(result,"%d",DX::memToUInt(addr));
	return result;

}
const char *conDumpFloat(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	// Hmm...
	char result[256];

	unsigned int addr=atoi(argv[1]);
	sprintf(result,"%f",DX::memToFloat(addr));
	return result;

}
const char *conFloatToHex(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	char formatResult[256];
	float input=atof(argv[1]);
	float * inputptr=&input;
	void * inputptr2 = (void *)inputptr;
	unsigned int * inputptr3=(unsigned int*)inputptr2;
	sprintf (formatResult,"%08X",*inputptr3);

	char* result = new char[256];
	memcpy(result, formatResult, 256);

	return result;
}
const char *conGetAddress(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	// Hmm...
	char formatResult[256];
	sprintf(formatResult, "%x", obj);

	char* result = new char[256];
	memcpy(result, formatResult, 256);

	return result;
}
const char *conGetAddressDec(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	// Hmm...
	char formatResult[256];
	sprintf(formatResult, "%d", obj);

	char* result = new char[256];
	memcpy(result, formatResult, 256);

	return result;
}
bool conShapeBaseSetCloakValue(Linker::SimObject *obj, S32 argc, const char* argv[])
{
	DX::ShapeBase operand = DX::ShapeBase((unsigned int)obj);
	operand.cloaked=dAtob(argv[2]);

	operand.cloak_level = std::stof(argv[3]);
	operand.setMaskBits(0x40);

	return true;
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
	operand.getControlObject().heat_level = std::stof(argv[1]);

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

bool conForceUpdate(Linker::SimObject *obj, S32 argc, const char* argv[]) 
{
	if (obj == NULL || (unsigned int)Sim::findObjectc(argv[2]) == NULL)
		return 0;

	DX::NetConnection conn = DX::NetConnection((unsigned int)obj);
	DX::NetObject netobj = DX::NetObject((unsigned int)Sim::findObjectc(argv[2]));
	GhostInfo * mGhostRefs=conn.mGhostRefs;
	if (netobj.base_pointer_value!=0) 
	{
		S32 index = conn.getGhostIndex(netobj);

		if (index > 0) 
			mGhostRefs[index].updateMask=mGhostRefs[index].updateMask | GameBaseMasks::InitialUpdateMask;

		return 1;
	}
	else 
		return 0;
}

S32 conGetGhostIndex(Linker::SimObject *obj, S32 argc, const char* argv[]) 
{
	unsigned int objptr2=(unsigned int)Sim::findObjectc(argv[2]);

	if ((unsigned int)obj == NULL || objptr2==NULL) 
		return -1;

	DX::NetConnection conn = DX::NetConnection((unsigned int)obj);

	char aicommand[255]="";
	sprintf (aicommand,"return (%d.isAIControlled());", conn.identifier);
	if (dAtob(Con::evaluate(aicommand, false, NULL, false)) == true) 
		return -1;

	char command[255]="";
	sprintf (command,"return (%d.getAddress());",conn.identifier);
	if (strcmp(Con::evaluate(command, false, NULL, false), "local") == 0) 
		return atoi(argv[2]);

	DX::NetObject netobj = DX::NetObject(objptr2);
	if (netobj.base_pointer_value!=0)
		return conn.getGhostIndex(netobj);
	
	return -1;
}

S32 conResolveGhost(Linker::SimObject *obj, S32 argc, const char* argv[]) 
{
	if (obj==NULL)
		return -1;

	DX::NetConnection conn = DX::NetConnection((unsigned int)obj);

	S32 id = atoi(argv[2]);
	if (id == -1)
		return -1;

	if (conn.resolveGhost(id) != NULL)
		return conn.resolveGhost(id);

	return -1;
}

S32 conResolveGhostParent(Linker::SimObject *obj, S32 argc, const char* argv[]) {
	if (((unsigned int)obj) == NULL) 
		return -1;

	DX::NetConnection conn = DX::NetConnection((unsigned int)obj);
	S32 ghostindex = atoi(argv[2]);
	if (conn.base_pointer_value!=0) 
		if (conn.resolveGhostParent(ghostindex))
		{
			S32 objid = DX::NetObject(conn.resolveGhostParent(ghostindex)).identifier;
			if (objid != 0)
				return objid;
		}

	return -1;
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

bool conclientCmdSetProcessTicks(Linker::SimObject *obj, S32 argc, const char* argv[]) 
{
	unsigned int result_ptr = 0;
	unsigned int my_ptr = 0;
	DX::NetObject objptr = (unsigned int)Sim::findObjectc(argv[1]);

	if (objptr.base_pointer_value) 
	{ 
		my_ptr=objptr.base_pointer_value;

		if (atoi(argv[2]) == 1) 
		{
			__asm
			{
				mov eax, my_ptr;
				add eax, 0x264;
				mov ebx,eax
				mov al, 1
				mov [ebx],al
					
			}	
		} 
		else 
		{
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

bool conSetProcessTicks(Linker::SimObject *obj, S32 argc, const char* argv[]) 
{
	unsigned int result_ptr = 0;
	unsigned int my_ptr = (unsigned int) obj;

	if (atoi(argv[2])==1) 
	{
		__asm
		{
			mov eax, my_ptr;
			add eax, 0x264;
			mov ebx,eax
			mov al, 1
			mov [ebx],al
				
		}	
	} 
	else 
	{
		__asm
		{
			mov eax, my_ptr;
			add eax, 0x264;
			mov ebx,eax
			mov al, 0
			mov [ebx],al
				
		}	
	}

	return true;
}

const char* conGrenadeProjectileGetPosition(Linker::SimObject *obj, S32 argc, const char* argv[])
{
	char formatResult[256];

	DX::GrenadeProjectile grenade = DX::GrenadeProjectile((unsigned int)obj);
	sprintf_s<256>(formatResult, "%f %f %f", grenade.position.x, grenade.position.y, grenade.position.z);

	char* result = new char[256];
	memcpy(result, formatResult, 256);

	return result;
}

const char* conGrenadeProjectileGetVelocity(Linker::SimObject *obj, S32 argc, const char* argv[])
{
	char formatResult[256];
	DX::GrenadeProjectile grenade((unsigned int)obj);
	sprintf_s<256>(formatResult, "%f %f %f", grenade.velocity.x, grenade.velocity.y, grenade.velocity.z);

	char* result = new char[256];
	memcpy(result, formatResult, 256);

	return result;
}

// General Commands ---------------------------------
#include <cstdarg>
#include <vector>
#include <string.h>

const char* conSprintf(Linker::SimObject *obj, S32 argc, const char* argv[])
{
	std::vector<const char*> input;
	for (int i = 2; i < argc; i++)
		input.push_back(argv[i]);

	char formatResult[256];
	
	va_list variable_args = reinterpret_cast<va_list>(input.data());
	vsprintf(formatResult, argv[1], variable_args);

	char* result = new char[256];
	memcpy(result, formatResult, 256);

	return result;
}

// RE Commands -----------------------------------

#include <regex>

bool reMatch(Linker::SimObject* obj, S32 argc, const char* argv[])
{
	try
	{
		std::smatch match;

		std::regex sequence(argv[1], std::regex::extended);
		std::regex_match(std::string(argv[2]), match, sequence);

		return !match.empty() && match.size() != 0;
	}
	catch (std::regex_error)
	{
		Con::errorf(0, "RE: Invalid regex sequence: %s", argv[1]);
		return false;
	}

	return false;
}

bool reSearch(Linker::SimObject* obj, S32 argc, const char* argv[])
{
	try
	{
		std::smatch match;

		std::regex sequence(argv[1], std::regex::extended);
		std::regex_search(std::string(argv[2]), match, sequence);

		return !match.empty() && match.size() != 0;
	}
	catch (std::regex_error)
	{
		Con::errorf(0, "RE: Invalid regex sequence: %s", argv[1]);
		return false;
	}

	return false;
}

const char* reReplace(Linker::SimObject* obj, S32 argc, const char* argv[])
{
	try
	{
		std::string result = std::regex_replace(std::string(argv[2]),
				std::regex(argv[1], std::regex::extended), 
				std::string(argv[3]));
		return result.c_str();
	}
	catch (std::regex_error)
	{
		Con::errorf(0, "RE: Invalid regex sequence: %s", argv[1]);
		return "-1";
	}

	return "-1";
}

static std::string currentString = "";
static std::regex currentRegex = std::regex("", std::regex::extended);
static auto matchBegin = std::sregex_iterator(currentString.begin(), currentString.end(), currentRegex);
static auto matchEnd = std::sregex_iterator();
static std::sregex_iterator currentMatchIter = matchEnd;

bool reIterBegin(Linker::SimObject* obj, S32 argc, const char* argv[])
{
	try
	{
		currentRegex = std::regex(argv[1], std::regex::extended);
		currentString = std::string(argv[2]);
		matchBegin = std::sregex_iterator(currentString.begin(), currentString.end(), currentRegex);
		matchEnd = std::sregex_iterator();
		currentMatchIter = matchBegin;

		return true;
	}
	catch (std::regex_error)
	{
		Con::errorf(0, "RE: Invalid regex sequence: %s", argv[1]);
		return false;
	}

	return false;
}

bool reIterEnd(Linker::SimObject* obj, S32 argc, const char* argv[])
{
	return currentMatchIter == matchEnd;
}

const char* reIterNext(Linker::SimObject* obj, S32 argc, const char* argv[])
{
	std::string currentResult = (*currentMatchIter).str();
	++currentMatchIter;

	return currentResult.c_str();
}