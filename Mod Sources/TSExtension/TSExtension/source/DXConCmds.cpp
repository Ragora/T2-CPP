/**
 *	
 */

#include <LinkerAPI.h>
#include <DXAPI/DXAPI.h>

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