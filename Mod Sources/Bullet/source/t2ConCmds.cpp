/*
 *	t2ConCmds.cpp
 *	Original code by Linker
 *	Modified by Robert MacGregor
*/

#include "stdafx.h"
#include "t2ConCmds.h"

// Linker's implementations
const char* conGuiTsCtrlProject(Linker::SimObject *obj,S32 argc, const char* argv[]) {
	Linker::Point3F pt;
	Linker::Point3F rt;
	dSscanf(argv[2],"%g %g %g",&pt.x,&pt.y,&pt.z);
	GuiTSCtrl_project(reinterpret_cast<GuiTSCtrl *>(obj),pt,&rt);

	char* buffer = Con::getReturnBuffer(255);
	dSprintf(buffer,255,"%g %g %g",rt.x,rt.y,rt.z);

	return buffer;
}

bool conNetObjectSetGhostable(Linker::SimObject *obj,S32 argc, const char* argv[]) {
	if (dAtob(argv[2])) {
		__asm {
			push ecx
			mov ecx, [obj]
			mov eax,[ecx+40h]
			or eax, 100h
			mov [ecx+40h], eax
			pop ecx
		}
	} else {
		__asm {
			xor eax, eax
			or eax, 100h
			not eax
			push ecx
			mov ecx,[obj]
			and eax, [ecx+40h]
			mov [ecx+40h],eax
			push    40h
			mov eax, 0x585BE0
			call eax
			pop ecx
		}
	}
	return 1;
}

const char* conGetVariable(Linker::SimObject *obj,S32 argc, const char* argv[]) {
	return Con::getVariable(argv[1]);
}

// BulletDLL Implementations
// *** NOTE ***
// All vectors should have their Y and Z axiis switched due to the Z axis being up and down in Tribes 2
Game::CGameTime *game_time;
btDefaultCollisionConfiguration *bullet_config = NULL;
btCollisionDispatcher *bullet_dispatcher = NULL;
btDbvtBroadphase *bullet_broadphase = NULL;
btSequentialImpulseConstraintSolver *bullet_constraint = NULL;
btDiscreteDynamicsWorld *bullet_world = NULL;

btBoxShape *box_shapes[MAXIMUM_BULLET_NODES];
btRigidBody *rigid_bodies[MAXIMUM_BULLET_NODES];

unsigned int bullet_node_count;

bool conBulletInitialize(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	Con::printf("Bullet: Attempting to initialize ...");
	if (!bullet_world)
	{
		// Try to create the game timer
		game_time = Game::CGameTime::getPointer();
		if (!game_time)
		{
			Con::errorf(1, "Bullet: Failed to initialize! Unable to create timing singleton.");
			return false;
		}
		else
			Con::printf("Bullet: Created timing singleton ...");

		// Try to create the config
		bullet_config = new btDefaultCollisionConfiguration;
		if (!bullet_config)
		{
			Con::errorf(1, "Bullet: Failed to initialize! Unable to create configuration object.");
			return false;
		}
		else
			Con::printf("Bullet: Bullet configuration object created ...");

		//Try to create the dispatcher
		bullet_dispatcher = new btCollisionDispatcher(bullet_config);
		if (!bullet_dispatcher)
		{
			Con::errorf(1, "Bullet: Failed to initialize! Unable to create collision dispatch object.");
			delete bullet_config;
			return false;
		}
		else
			Con::printf("Bullet: Bullet collision dispatch object created ...");

		// Try to create the broadphase object
		bullet_broadphase = new btDbvtBroadphase;
		if (!bullet_broadphase)
		{
			Con::errorf(1, "Bullet: Failed to initialize! Unable to create broadphase object.");
			delete bullet_dispatcher;
			delete bullet_config;
			return false;
		}
		Con::printf("Bullet: Bullet broadphase object created ...");

		// Try to create the sequential impulse constraint solver
		bullet_constraint = new btSequentialImpulseConstraintSolver;
		if (!bullet_constraint)
		{
			Con::errorf(1, "Bullet: Failed to initialize! Unable to create sequential impulse constraint solver object.");
			delete bullet_dispatcher;
			delete bullet_config;
			delete bullet_broadphase;
			return false;
		}
		else
			Con::printf("Bullet: Bullet sequential impulse constraint solver object created ...");

		// Now try to create the bullet world
		bullet_world = new btDiscreteDynamicsWorld(bullet_dispatcher, bullet_broadphase, bullet_constraint, bullet_config);
		if (!bullet_world)
		{
			Con::errorf(1, "Bullet: Failed to initialize! Unable to create discrete dynamics world.");
			delete bullet_dispatcher;
			delete bullet_config;
			delete bullet_broadphase;
			delete bullet_constraint;
			return false;
		}
		else
			Con::printf("Bullet: Bullet discrete dynamics world created! Bullet is now ready.");

		bullet_world->setGravity(btVector3(0,-100,0));
		bullet_node_count = 0;
		return true;
	}
	Con::errorf(1, "Bullet: Already initialized!");
	return true;
}

bool conBulletDeinitialize(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	if (bullet_world)
	{
		delete bullet_world;
		bullet_world = NULL;
		delete bullet_dispatcher;
		bullet_dispatcher = NULL;
		delete bullet_config;
		bullet_config = NULL;
		delete bullet_broadphase;
		bullet_broadphase = NULL;
		delete bullet_constraint;
		bullet_constraint = NULL;
		Game::CGameTime::destroy();
		Con::printf("Bullet: Successfully deinitialized.");
		return true;
	}
	Con::errorf(1, "Bullet: Not initialized!");
	return false;
}

bool conBulletUpdate(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	if (bullet_world)
	{
		if (argc >= 2)
		{
			btScalar delta = btScalar(atof(argv[1]));
			bullet_world->stepSimulation(delta);
		}
		else
			bullet_world->stepSimulation(game_time->getDelta());
		game_time->update();
		return true;
	}
	return false;
}

const char *conBulletGetPosition(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	if (!bullet_world)
	{
		Con::errorf(1, "Bullet: Attempted to retrieve the position of a node without having initialized Bullet!");
		return "0 0 0";
	}

	unsigned int id = atoi(argv[1]);
	if (id >= MAXIMUM_BULLET_NODES)
	{
		Con::errorf(1, "Bullet: Attempted to retrieve the position of a out-of-range node! (%u > %u)", id, MAXIMUM_BULLET_NODES);
		return "0 0 0";
	}
	if (id > bullet_node_count)
	{
		Con::errorf(1, "Bullet: Attempted to retrieve the position of a non-existent node! (%u)", id);
		return "0 0 0";
	}

	btRigidBody *body = rigid_bodies[id];
	if (!body)
	{
		Con::errorf(1, "Bullet: Node retrieval for object %u successful, however it is nonexistent!", id);
		return "0 0 0";
	}

	btTransform transform = body->getCenterOfMassTransform();
	btVector3 position = transform.getOrigin();

	char *output = (char*)malloc(sizeof(char)*256);
	sprintf_s(output, 256, "%f %f %f", position.x(), position.z(), position.y());
	return output;
}

const char *conBulletGetRotation(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	if (!bullet_world)
	{
		Con::errorf(1, "Bullet: Attempted to retrieve the rotation of a node without having initialized Bullet!");
		return "0 0 0";
	}

	unsigned int id = atoi(argv[1]);
	if (id >= MAXIMUM_BULLET_NODES)
	{
		Con::errorf(1, "Bullet: Attempted to retrieve the rotation of a out-of-range node! (%u > %u)", id, MAXIMUM_BULLET_NODES);
		return "0 0 0";
	}
	if (id > bullet_node_count)
	{
		Con::errorf(1, "Bullet: Attempted to retrieve the rotation of a non-existent node! (%u)", id);
		return "0 0 0";
	}

	btRigidBody *body = rigid_bodies[id];
	if (!body)
	{
		Con::errorf(1, "Bullet: Node retrieval for object %u successful, however it is nonexistent!", id);
		return "0 0 0";
	}

	btQuaternion rotation = body->getOrientation();

	char *output = (char*)malloc(sizeof(char)*256);
	sprintf_s(output, 256, "%f %f %f %f", rotation.x(), rotation.y(), rotation.z(), rotation.w());
	return output;
}

bool conBulletInitialized(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	if (!bullet_world)
		return false;
	return true;
}

const char *conBulletCreateCube(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	if (!bullet_world)
	{
		Con::errorf(1, "Bullet: Attempted to create a cube without having initialized Bullet!");
		return "0";
	}
	btScalar mass = btScalar(atof(argv[1]));
	btScalar X = btScalar(atof(argv[2]));
	btScalar Y = btScalar(atof(argv[4]));
	btScalar Z = btScalar(atof(argv[3]));

	// Create the Bullet Object
	btTransform start_transform;

	btBoxShape *shape = new btBoxShape(btVector3(X,Y,Z));
	btRigidBody::btRigidBodyConstructionInfo info(mass, NULL, shape, btVector3(btScalar(0.0f), btScalar(0.0f), btScalar(0.0f)));
	btRigidBody *body = new btRigidBody(info);
	bullet_world->addRigidBody(body);

	unsigned int identification = bullet_node_count;
	rigid_bodies[identification] = body;
	bullet_node_count++;

	char *output = (char*)malloc(sizeof(char)*256);
	sprintf_s(output, 256, "%u", identification);
	return output;
}

bool conBulletSetPosition(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	unsigned int id = atoi(argv[1]);
	if (!bullet_world)
	{
		Con::errorf(1, "Bullet: Attempted to set the position of an object %u without having Bullet initialized!", id);
		return false;
	}

	if (id >= MAXIMUM_BULLET_NODES)
	{
		Con::errorf(1, "Bullet: Attempted to set the rotation of a out-of-range node! (%u > %u)", id, MAXIMUM_BULLET_NODES);
		return "0 0 0";
	}
	if (id > bullet_node_count)
	{
		Con::errorf(1, "Bullet: Attempted to set the rotation of a non-existent node! (%u)", id);
		return "0 0 0";
	}

	btRigidBody *body = rigid_bodies[id];
	if (!body)
	{
		Con::errorf(1, "Bullet: Node retrieval for object %u successful, however it is nonexistent!", id);
		return "0 0 0";
	}

	btScalar X = btScalar(atof(argv[2]));
	btScalar Y = btScalar(atof(argv[4]));
	btScalar Z = btScalar(atof(argv[3]));

	btTransform transform = body->getCenterOfMassTransform();
	btTransform new_transform(transform.getRotation(),btVector3(btScalar(X),btScalar(Y),btScalar(Z)));
	body->setWorldTransform(new_transform);
	return true;
}

bool conBulletSetGravity(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	if (!bullet_world)
	{
		Con::errorf(1, "Bullet: Attempted to set the gravity of the Bullet world without having Bullet initialized!");
		return false;
	}

	btScalar X = btScalar(atof(argv[1]));
	btScalar Y = btScalar(atof(argv[3]));
	btScalar Z = btScalar(atof(argv[2]));
	bullet_world->setGravity(btVector3(btScalar(X),btScalar(Y),btScalar(Z)));
	return true;
}

bool conBulletSetMass(Linker::SimObject *obj, S32 argc, const char *argv[])
{
    unsigned int id = atoi(argv[1]);
    if (!bullet_world)
    {
        Con::errorf(1, "Bullet: Attempted to set mass of object %u without having Bullet initialized!", id);
        return false;
    }
    if (id >= MAXIMUM_BULLET_NODES)
	{
		Con::errorf(1, "Bullet: Attempted to set the rotation of a out-of-range node! (%u > %u)", id, MAXIMUM_BULLET_NODES);
		return "0 0 0";
	}
	if (id > bullet_node_count)
	{
		Con::errorf(1, "Bullet: Attempted to set the rotation of a non-existent node! (%u)", id);
		return "0 0 0";
	}

    btRigidBody *body = rigid_bodies[id];
	if (!body)
	{
		Con::errorf(1, "Bullet: Node retrieval for object %u successful, however it is nonexistent!", id);
		return "0 0 0";
	}
    btScalar mass = btScalar(atof(argv[2]));
  //  body->setMass(mass);

    return true;
}
