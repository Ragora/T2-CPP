/**
 *	
 */
 
#include "LinkerAPI.h"
#include "DXAPI.h"

const char *conGetAddress(SimObject *obj, S32 argc, const char *argv[])
{
	// Hmm...
	char result[256];
	sprintf(result, "%x", obj);
	return result;
}

const char *conPlayerSetZ(SimObject *obj, S32 argc, const char *argv[])
{
	DX::Player test = DX::GetPlayerPointer(obj);
	test.position_y = 100;
	test.position_z = 300;
	test.position_x = 500;

	char result[256];
	sprintf(result, "LOL");
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