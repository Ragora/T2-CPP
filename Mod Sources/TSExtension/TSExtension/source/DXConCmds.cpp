/**
 *	
 */

#include <LinkerAPI.h>
#include <DXAPI.h>

const char *conGetAddress(SimObject *obj, S32 argc, const char *argv[])
{
	// Hmm...
	char result[256];
	sprintf(result, "%x", obj);
	return result;
}

bool conPlayerGetJumpingState(SimObject *obj, S32 argc, const char* argv[])
{
	DX::Player operand = DX::GetPlayerPointer(obj);

	return operand.is_jumping;
}

bool conPlayerGetJettingState(SimObject *obj, S32 argc, const char* argv[])
{
	DX::Player operand = DX::GetPlayerPointer(obj);

	return operand.is_jetting;
}

bool conProjectileExplode(SimObject *obj, S32 argc, const char* argv[])
{
	Point3F position;
	position.x = 0;
	position.y = 0;
	position.z = 0;

	Point3F normal;
	normal.x = 0;
	normal.y = 0;
	normal.z = 0;

	unsigned int collideType = atoi(argv[4]);
	DX::Projectile_explode((DX::Projectile*)obj, position, normal, collideType);

	return true;
}

const char* conGrenadeProjectileGetPosition(SimObject *obj, S32 argc, const char* argv[])
{
	char result[256];

	DX::GrenadeProjectile grenade = DX::GetGrenadeProjectilePointer(obj);
	sprintf_s<256>(result, "%f %f %f", grenade.position_x, grenade.position_y, grenade.position_z);
	return result;
}

const char* conGrenadeProjectileGetVelocity(SimObject *obj, S32 argc, const char* argv[])
{
	char result[256];

	DX::GrenadeProjectile grenade = DX::GetGrenadeProjectilePointer(obj);
	sprintf_s<256>(result, "%f %f %f", grenade.velocity_x, grenade.velocity_y, grenade.velocity_z);
	return result;
}

// General Commands ---------------------------------
#include <cstdarg>
#include <vector>
#include <string.h>

const char* conSprintf(SimObject *obj, S32 argc, const char* argv[])
{
	std::vector<const char*> input;
	for (unsigned int i = 2; i < argc; i++)
		input.push_back(argv[i]);

	char result[256];
	
	va_list variable_args = reinterpret_cast<va_list>(input.data());
	vsprintf(result, argv[1], variable_args);

	return result;
}