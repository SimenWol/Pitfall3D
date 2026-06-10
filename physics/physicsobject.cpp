#include "physicsobject.h"
#include <bullet/btBulletDynamicsCommon.h>
#include "physics.h"

PhysicsObj::PhysicsObj(btCollisionShape* shapeIn, float massIn, const btVector3& positionIn, const btQuaternion& rotationIn)
{
	// Create startup transform
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(positionIn);
	transform.setRotation(rotationIn);

	// Create the Game Values
	status = new GameValues(transform);
	shape = shapeIn;

	// Set up inertia, start with 0, and test if we have an infinite mass(0.0), if so, no inertia is possible
	btVector3 inertia(0, 0, 0);
	if (massIn != 0.0f) { shape->calculateLocalInertia(massIn, inertia); }

	// Make a new RB with this info
	btRigidBody::btRigidBodyConstructionInfo RBinfo(massIn, status, shape, inertia);
	body = new btRigidBody(RBinfo);
}

PhysicsObj::~PhysicsObj()
{
	delete shape;
	delete status;
	delete body;
}

inline btCollisionShape* PhysicsObj::GetShape() const { return shape; }
inline void PhysicsObj::SetShape(btCollisionShape* S) { shape = S; }

inline void PhysicsObj::SetRigitBody(btRigidBody* RB) { body = RB; }

inline void PhysicsObj::SetOrientation(btQuaternion rotationIn)
{
	GetRigidBody()->setWorldTransform(btTransform(rotationIn, body->getWorldTransform().getOrigin()));
}

inline void PhysicsObj::SetPosition(const btVector3 position)
{
	GetRigidBody()->setWorldTransform(btTransform(body->getOrientation(), position));
}