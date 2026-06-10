#pragma once
#include <bullet/btBulletDynamicsCommon.h>

///////////////////////////////////////////////
// Overriding function ContactResultCallback //
// Code obtained and altered from ChatGPT	 //
class ContactResultCallback : public btCollisionWorld::ContactResultCallback
{
public:
	virtual btScalar addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap,
		int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap,
		int partId1, int index1) override;
	const btRigidBody* getLastCoinCollisionBody();
	bool getObstacleCollision();
private:
	// To retrace and delete the correct coins.
	bool coinCollision = false;
	bool obstacleCollision = false;
	const btRigidBody* lastCoinCollisionBody;
};

class Physics
{
public:
	Physics();
	~Physics();

	btBroadphaseInterface* broadPhase;
	btDefaultCollisionConfiguration* collisionConfig;
	btCollisionDispatcher* dispatcher;
	btSequentialImpulseConstraintSolver* constraintSolver;
	btDiscreteDynamicsWorld* world;
	// Create an instance of the contact callback
	ContactResultCallback callback;

	// Collision Groups
	static constexpr int PLAYER_GROUP_FLAG		= 5;
	static constexpr int OBSTACLE_GROUP_FLAG	= 6;
	static constexpr int COLLECTIBLE_GROUP_FLAG = 9;

	// Collision Masks
	static constexpr int ALL_OBJECTS_MASK  = 0xFFFF;
};