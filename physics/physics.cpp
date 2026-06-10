#include "physics.h"
#include <bullet/btBulletDynamicsCommon.h>
#include <iostream>

btScalar ContactResultCallback::addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1)
{
	// Access the colliding objects
	const btCollisionObject* colObj0 = colObj0Wrap->getCollisionObject();
	const btCollisionObject* colObj1 = colObj1Wrap->getCollisionObject();

	// Get collision groups
	const int group0 = colObj0->getBroadphaseHandle()->m_collisionFilterGroup;
	const int group1 = colObj1->getBroadphaseHandle()->m_collisionFilterGroup;

	// Check if collision involves specific groups
	if (((group0 & Physics::PLAYER_GROUP_FLAG) == Physics::PLAYER_GROUP_FLAG) && ((group1 & Physics::OBSTACLE_GROUP_FLAG) == Physics::OBSTACLE_GROUP_FLAG))
	{
#ifdef _DEBUG
		std::cout << "Collision between specified groups (player/obstacle)!" << std::endl;
#endif // _DEBUG
		obstacleCollision = true;
	}
	else if (((group0 & Physics::PLAYER_GROUP_FLAG) == Physics::PLAYER_GROUP_FLAG) && ((group1 & Physics::COLLECTIBLE_GROUP_FLAG) == Physics::COLLECTIBLE_GROUP_FLAG))
	{
#ifdef _DEBUG
		std::cout << "Collision between specified groups (player/collectible)!" << std::endl;
#endif // _DEBUG
		lastCoinCollisionBody = btRigidBody::upcast(colObj1);
		coinCollision = true;
		return 0;
	}

	return 0;
}

const btRigidBody* ContactResultCallback::getLastCoinCollisionBody()
{
	if (coinCollision)
	{
		coinCollision = false;
		return lastCoinCollisionBody;
	}
	else { return nullptr; }
}

bool ContactResultCallback::getObstacleCollision()
{
	if (obstacleCollision)
	{
		obstacleCollision = false;
		return true;
	}
	else { return false; }
}

Physics::Physics()
{
	collisionConfig = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfig);
	broadPhase = new btDbvtBroadphase();
	constraintSolver = new btSequentialImpulseConstraintSolver();
	world = new btDiscreteDynamicsWorld(dispatcher, broadPhase, constraintSolver, collisionConfig);
	world->setGravity(btVector3(0, -9.81f, 0));
}

Physics::~Physics()
{
	delete world;
	delete collisionConfig;
	delete dispatcher;
	delete broadPhase;
	delete constraintSolver;
}