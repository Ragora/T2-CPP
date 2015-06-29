/*
 *	t2ConCmds.cpp
 *	Original code by Linker
 *	Modified by Robert MacGregor
*/

#include "stdafx.h"
#include "t2ConCmds.h"

// BulletDLL Implementations
// *** NOTE ***
// All vectors should have their Y and Z axiis switched due to the Z axis being up and down in Tribes 2
static btDefaultCollisionConfiguration *sBulletConfig = NULL;
static btCollisionDispatcher *sCollisionDispatcher = NULL;
static btDbvtBroadphase *sBroadphase = NULL;
static btSequentialImpulseConstraintSolver *sConstraintSolver = NULL;
static btDiscreteDynamicsWorld *sBulletWorld = NULL;

struct NodeInformation
{
	unsigned int mID;
	btBoxShape *mShape;
	btRigidBody *mRigidBody;
};

static NodeInformation *sActiveNodes[MAXIMUM_BULLET_NODES];

static unsigned int sRigidBodyCount = 0;

bool conBulletInitialize(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	Con::printf("Bullet: Attempting to initialize ...");

	if (!sBulletWorld)
	{
		// Try to create the config
		sBulletConfig = new btDefaultCollisionConfiguration;
		if (!sBulletConfig)
		{
			Con::errorf(1, "Bullet: Failed to initialize! Unable to create configuration object.");
			return false;
		}
		else
			Con::printf("Bullet: Bullet configuration object created ...");

		//Try to create the dispatcher
		sCollisionDispatcher = new btCollisionDispatcher(sBulletConfig);
		if (!sCollisionDispatcher)
		{
			Con::errorf(1, "Bullet: Failed to initialize! Unable to create collision dispatch object.");
			delete sBulletConfig;
			return false;
		}
		else
			Con::printf("Bullet: Bullet collision dispatch object created ...");

		// Try to create the broadphase object
		sBroadphase = new btDbvtBroadphase;
		if (!sBroadphase)
		{
			Con::errorf(1, "Bullet: Failed to initialize! Unable to create broadphase object.");
			delete sCollisionDispatcher;
			delete sBulletConfig;
			return false;
		}
		Con::printf("Bullet: Bullet broadphase object created ...");

		// Try to create the sequential impulse constraint solver
		sConstraintSolver = new btSequentialImpulseConstraintSolver;
		if (!sConstraintSolver)
		{
			Con::errorf(1, "Bullet: Failed to initialize! Unable to create sequential impulse constraint solver object.");
			delete sCollisionDispatcher;
			delete sBulletConfig;
			delete sBroadphase;
			return false;
		}
		else
			Con::printf("Bullet: Bullet sequential impulse constraint solver object created ...");

		// Now try to create the bullet world
		sBulletWorld = new btDiscreteDynamicsWorld(sCollisionDispatcher, sBroadphase, sConstraintSolver, sBulletConfig);
		if (!sBulletWorld)
		{
			Con::errorf(1, "Bullet: Failed to initialize! Unable to create discrete dynamics world.");
			delete sCollisionDispatcher;
			delete sBulletConfig;
			delete sBroadphase;
			delete sConstraintSolver;
			return false;
		}
		else
			Con::printf("Bullet: Bullet discrete dynamics world created! Bullet is now ready.");

		sBulletWorld->setGravity(btVector3(0,-100,0));
		sRigidBodyCount = 0;
		return true;
	}
	Con::errorf(1, "Bullet: Already initialized!");
	return true;
}

bool conBulletDeinitialize(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	if (sBulletWorld)
	{
		delete sBulletWorld;
		sBulletWorld = NULL;

		delete sCollisionDispatcher;
		sCollisionDispatcher = NULL;

		delete sBulletConfig;
		sBulletConfig = NULL;

		delete sBroadphase;
		sBroadphase = NULL;

		delete sConstraintSolver;
		sConstraintSolver = NULL;

		Con::printf("Bullet: Successfully deinitialized.");
		return true;
	}
	Con::errorf(1, "Bullet: Not initialized!");
	return false;
}

void updateBullet(const unsigned int& timeDifference)
{
	if (sBulletWorld)
	{
		sBulletWorld->stepSimulation(timeDifference);

		// When we pump an update, we need to step through and push updates to the nodes
		for (size_t iteration = 0; iteration < sRigidBodyCount; iteration++)
		{
			char *result = new char[256];

			// Get the position
			btTransform transform = sActiveNodes[iteration]->mRigidBody->getCenterOfMassTransform();
			btVector3 position = transform.getOrigin();

			// Then the rotation
			btQuaternion rotation = sActiveNodes[iteration]->mRigidBody->getOrientation();

			// Assemble the output
			sprintf(result, "%u.setTransform(\"%f %f %f %f %f %f %f\");", sActiveNodes[iteration]->mID, position.x(), position.z(), position.y(), rotation.x(), rotation.z(), rotation.y(), rotation.w());
			Con::evaluate(result, false, NULL, false);
		}
	}
}

const char *conBulletGetPosition(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	if (!sBulletWorld)
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
	if (id > sRigidBodyCount)
	{
		Con::errorf(1, "Bullet: Attempted to retrieve the position of a non-existent node! (%u)", id);
		return "0 0 0";
	}

	NodeInformation *node = sActiveNodes[id];
	if (!node)
	{
		Con::errorf(1, "Bullet: Node retrieval for object %u successful, however it is nonexistent!", id);
		return "0 0 0";
	}

	btTransform transform = node->mRigidBody->getCenterOfMassTransform();
	btVector3 position = transform.getOrigin();

	char *output = (char*)malloc(sizeof(char)*256);
	sprintf_s(output, 256, "%f %f %f", position.x(), position.z(), position.y());
	return output;
}

const char *conBulletGetRotation(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	if (!sBulletWorld)
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
	if (id > sRigidBodyCount)
	{
		Con::errorf(1, "Bullet: Attempted to retrieve the rotation of a non-existent node! (%u)", id);
		return "0 0 0";
	}

	NodeInformation *node = sActiveNodes[id];
	if (!node)
	{
		Con::errorf(1, "Bullet: Node retrieval for object %u successful, however it is nonexistent!", id);
		return "0 0 0";
	}

	btQuaternion rotation = node->mRigidBody->getOrientation();

	char *output = (char*)malloc(sizeof(char)*256);
	sprintf_s(output, 256, "%f %f %f %f", rotation.x(), rotation.y(), rotation.z(), rotation.w());
	return output;
}

bool conBulletInitialized(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	if (!sBulletWorld)
		return false;
	return true;
}

const char *conBulletCreateCube(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	if (!sBulletWorld)
	{
		Con::errorf(1, "Bullet: Attempted to create a cube without having initialized Bullet!");
		return "0";
	}

	btScalar mass = btScalar(atof(argv[1]));
	btScalar X = btScalar(atof(argv[2]));
	btScalar Y = btScalar(atof(argv[4]));
	btScalar Z = btScalar(atof(argv[3]));
	const unsigned int boundID = atoi(argv[4]);

	// Create the Bullet Object
	btTransform start_transform;

	btBoxShape *shape = new btBoxShape(btVector3(X,Y,Z));
	btRigidBody::btRigidBodyConstructionInfo info(mass, NULL, shape, btVector3(btScalar(0.0f), btScalar(0.0f), btScalar(0.0f)));
	btRigidBody *body = new btRigidBody(info);
	sBulletWorld->addRigidBody(body);

	NodeInformation *newNode = new NodeInformation;
	newNode->mRigidBody = body;
	newNode->mShape = shape;
	newNode->mID = boundID;

	unsigned int identification = sRigidBodyCount;
	sActiveNodes[identification] = newNode;
	sRigidBodyCount++;

	char *output = (char*)malloc(sizeof(char)*256);
	sprintf_s(output, 256, "%u", identification);
	return output;
}

bool conBulletSetPosition(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	unsigned int id = atoi(argv[1]);
	if (!sBulletWorld)
	{
		Con::errorf(1, "Bullet: Attempted to set the position of an object %u without having Bullet initialized!", id);
		return false;
	}

	if (id >= MAXIMUM_BULLET_NODES)
	{
		Con::errorf(1, "Bullet: Attempted to set the rotation of a out-of-range node! (%u > %u)", id, MAXIMUM_BULLET_NODES);
		return "0 0 0";
	}
	if (id > sRigidBodyCount)
	{
		Con::errorf(1, "Bullet: Attempted to set the rotation of a non-existent node! (%u)", id);
		return "0 0 0";
	}

	NodeInformation *node = sActiveNodes[id];
	if (!node)
	{
		Con::errorf(1, "Bullet: Node retrieval for object %u successful, however it is nonexistent!", id);
		return "0 0 0";
	}

	btScalar X = btScalar(atof(argv[2]));
	btScalar Y = btScalar(atof(argv[4]));
	btScalar Z = btScalar(atof(argv[3]));

	btTransform transform = node->mRigidBody->getCenterOfMassTransform();
	btTransform new_transform(transform.getRotation(),btVector3(btScalar(X),btScalar(Y),btScalar(Z)));
	node->mRigidBody->setWorldTransform(new_transform);
	return true;
}

bool conBulletSetGravity(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	if (!sBulletWorld)
	{
		Con::errorf(1, "Bullet: Attempted to set the gravity of the Bullet world without having Bullet initialized!");
		return false;
	}

	btScalar X = btScalar(atof(argv[1]));
	btScalar Y = btScalar(atof(argv[3]));
	btScalar Z = btScalar(atof(argv[2]));
	sBulletWorld->setGravity(btVector3(btScalar(X),btScalar(Y),btScalar(Z)));
	return true;
}

bool conBulletSetMass(Linker::SimObject *obj, S32 argc, const char *argv[])
{
    unsigned int id = atoi(argv[1]);
    if (!sBulletWorld)
    {
        Con::errorf(1, "Bullet: Attempted to set mass of object %u without having Bullet initialized!", id);
        return false;
    }
    if (id >= MAXIMUM_BULLET_NODES)
	{
		Con::errorf(1, "Bullet: Attempted to set the rotation of a out-of-range node! (%u > %u)", id, MAXIMUM_BULLET_NODES);
		return "0 0 0";
	}
	if (id > sRigidBodyCount)
	{
		Con::errorf(1, "Bullet: Attempted to set the rotation of a non-existent node! (%u)", id);
		return "0 0 0";
	}

    NodeInformation *node = sActiveNodes[id];
	if (!node)
	{
		Con::errorf(1, "Bullet: Node retrieval for object %u successful, however it is nonexistent!", id);
		return "0 0 0";
	}
    btScalar mass = btScalar(atof(argv[2]));
  //  body->setMass(mass);

    return true;
}
