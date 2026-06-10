#pragma once
#include <bullet/btBulletCollisionCommon.h>

//class btCollisionShape;
//class btVector3;
//class btQuaternion;
//class btRigidBody;
//class btTransform;
//class btScalar;

// this is a support class to get return data to the game status, it won't have much extra info, so its safe in here.
//The only thing we really want is the worldTransform of the MotionState which supplies us with position and orientation
// data to allow us to render objects 
class GameValues : public btDefaultMotionState
{
public:
	GameValues(const btTransform& transform) // set up with our transform matrix
		:btDefaultMotionState(transform) {}

	void GetWorldTransform(btScalar* transform)
	{
		btTransform l_transform; // create a local bullet transform
		getWorldTransform(l_transform); // load it with the World Transform for this object
		l_transform.getOpenGLMatrix(transform); // convert to OpenGL format and store in the address provided
	}
};

class PhysicsObj
{
public:
	PhysicsObj(btCollisionShape* shapeIn, float massIn, const btVector3& positionIn, const btQuaternion& rotationIn);
	~PhysicsObj();

	inline btCollisionShape* GetShape() const;
	inline void SetShape(btCollisionShape* s);

	inline btRigidBody* GetRigidBody() { return body; }
	inline void SetRigitBody(btRigidBody* RB);

	inline btMotionState* GetMotionState() { return status; }

	inline void SetOrientation(btQuaternion rotationIn);

	inline void SetPosition(const btVector3 position);

	void GetTransform(btScalar* transform) const
	{
		if (status) { status->GetWorldTransform(transform); }
	}

protected:

	btRigidBody* body;
	btCollisionShape* shape;
	GameValues* status;
};