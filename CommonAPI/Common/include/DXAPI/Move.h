#pragma once

#include <DXAPI/Point3F.h>
#include <DXAPI/GameBase.h>

#include <LinkerAPI.h>
namespace DX
{
	/**
	 *	@brief Move structure. 
	 */
	struct Move {
		int px;
		int py;
		int pz;
		int pyaw;
		int ppitch;
		int proll;
		float x;
		float y;
		float z;
		float yaw;
		float pitch;
		float roll;
		int unused;
		int count;
		bool freelook;
		bool triggers[6];

	};

	struct AIMove {
		unsigned int id;
		Move move;
		bool used;
	};

	float clampFloat(float in);
	float clampMove(float in);
	void generateNullMove(Move * ret);
};