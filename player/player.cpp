#include "player.h"
#include "../glm/glm/glm.hpp"
#include "../bullet/btBulletDynamicsCommon.h"
#include "../bullet/BulletDynamics/ConstraintSolver/btGeneric6DofConstraint.h"
#include "../models/player/playermodel.h"
#include "../graphics/shader.h"
#include "../physics/physics.h"
#include "../physics/physicsobject.h"

Player::Player(Physics* phys, Shader* shader)
{
	// Create player model and set position/rotation.
	playerModel = new PlayerModel((char*)"../../../resources/models/kermit.obj", phys);
	glm::vec3 pos = glm::vec3(0.0f, 3.0f, 0.0f);
	playerModel->Scales = glm::vec3(0.02f, 0.02f, 0.02f);
	playerModel->SetPosition(pos);
	playerModel->SetXRotation(0.0f);
	playerModel->SetYRotation(0.0f);
	playerModel->SetZRotation(0.0f);
	playerModel->StoreGraphicClass(shader);

	// Add RigidBody
	btVector3 modelPos = btVector3(playerModel->GetPositon().x, playerModel->GetPositon().y, playerModel->GetPositon().z);
	btQuaternion modelRot = btQuaternion(playerModel->GetRotations().x, playerModel->GetRotations().y, playerModel->GetRotations().z, 1);
	playerModel->myPhysObj = playerModel->CreatePhysicsObj(playerModel->CreateMyShape(ObjectModel::CAPSULE, btVector3(0.75f, 2.5, 0.75f)), 100, modelPos, modelRot);

	// Lock rotation along all axes using a 6-DOF constraint
	btGeneric6DofConstraint* rotationConstraint = new btGeneric6DofConstraint(*playerModel->myPhysObj->GetRigidBody(), btTransform::getIdentity(), true);
	rotationConstraint->setAngularLowerLimit(btVector3(0, 0, 0));  // No rotation
	rotationConstraint->setAngularUpperLimit(btVector3(0, 0, 0));
	rotationConstraint->setLinearLowerLimit(btVector3(-2.0f, 1, 1));  // Allow translation along all axes
	rotationConstraint->setLinearUpperLimit(btVector3(2.0f, 0, 0));

	phys->world->addConstraint(rotationConstraint, true);

	// Add flags
	playerModel->myPhysObj->GetRigidBody()->getBroadphaseHandle()->m_collisionFilterGroup = Physics::PLAYER_GROUP_FLAG;
	playerModel->myPhysObj->GetRigidBody()->getBroadphaseHandle()->m_collisionFilterMask  = Physics::ALL_OBJECTS_MASK;
}

Player::~Player()
{
	delete playerModel;
}

void Player::SetSpeed(const PlayerSpeed mode)
{
	switch (mode)
	{
	case Player::SLOW:			maxForwardSpeed = 8.5f; break;
	case Player::NORMAL:		maxForwardSpeed = 12.5f; break;
	case Player::FAST:			maxForwardSpeed = 17.5f; break;
	case Player::HYPERSPEED:	maxForwardSpeed = 22.5f; break;
	default: std::cout << "Unkown speedtype, speed has not been updated." << std::endl; break;
	}
}

btRigidBody* Player::GetRigidBody() const { return playerModel->myPhysObj->GetRigidBody(); }

void Player::Move(const PlayerMovement action, const float deltaTime)
{
	btTransform transform = playerModel->myPhysObj->GetRigidBody()->getWorldTransform();

	switch (action)
	{
	case LEFT:
		if (currentLane != 1) { currentLane++; posBeforeSwitch = transform.getOrigin().getX(); laneAnimationTimer = 1.0f; }
		break;
	case RIGHT:
		if (currentLane != -1) { currentLane--; posBeforeSwitch = transform.getOrigin().getX(); laneAnimationTimer = 1.0f; }
		break;
	case FORWARD:
	{
		if (deltaTime == 0.0f)
		{
			transform.setOrigin(btVector3(transform.getOrigin().getX(), transform.getOrigin().getY(), (transform.getOrigin().getZ() + 2.0f)));
		}
		else
		{
			// Acceleration during speed increase
			if (forwardSpeed < maxForwardSpeed)
			{
				forwardSpeed += 2.5f * deltaTime;
				if (forwardSpeed > maxForwardSpeed) { forwardSpeed = maxForwardSpeed; }
			}
			else if (forwardSpeed > maxForwardSpeed)
			{
				forwardSpeed -= 3.5f * deltaTime;
				if (forwardSpeed < maxForwardSpeed) { forwardSpeed = maxForwardSpeed; }
			}

			// Constant forward motion - gets triggered every frame
			playerModel->myPhysObj->GetRigidBody()->setLinearVelocity(btVector3(sideSpeed, playerModel->myPhysObj->GetRigidBody()->getLinearVelocity().getY(), forwardSpeed));
		}
	}
	break;
	case BACKWARD:
	{
		transform.setOrigin(btVector3(transform.getOrigin().getX(), transform.getOrigin().getY(), (transform.getOrigin().getZ() - 2.0f)));
	}
	break;
	case JUMP:
	{
		if (playerModel->GetPositon().y < 2.05f && playerModel->GetPositon().y > 1.95f) { canJump = true; }
		else { canJump = false; }

		if (canJump && jumpTimer == 0.0f)
		{
			playerModel->myPhysObj->GetRigidBody()->applyCentralImpulse(btVector3(0, jumpStrength * 100.0f, 0));
			canJump = false;
			jumpTimer = 0.2f;
		}
	}
	break;
	default:
		break;
	}

	// Jump timer
	if (jumpTimer > 0.0f)
	{
		jumpTimer -= deltaTime;
		if (jumpTimer <= 0.0f) { jumpTimer = 0.0f; }
	}

	// Lane switching calculations
	if (laneAnimationTimer > 0.0f)
	{
		laneAnimationTimer -= deltaTime * 2.0f;
		if (laneAnimationTimer < 0.0f) { laneAnimationTimer = 0.0f; }

		const float animationOffset = (static_cast<float>(currentLane) * 2.0f - posBeforeSwitch) * easeInOutCubic(1.0f - laneAnimationTimer);
		transform.setOrigin(btVector3(posBeforeSwitch + animationOffset,
										transform.getOrigin().getY(), transform.getOrigin().getZ()));

		playerModel->myPhysObj->GetRigidBody()->setWorldTransform(transform);
	}
}

void Player::Update(glm::mat4 view, glm::mat4 projection) const { playerModel->Update(view, projection); }

void Player::Draw() const { playerModel->Draw(); }

void Player::Reset()
{
	playerModel->myPhysObj->GetRigidBody()->getWorldTransform().setOrigin(btVector3(0.0f, 2.0f, 0.0f));
	jumpTimer = 0.0f;

	sideSpeed = 0.0f;
	upSpeed = 0.0f;
	maxForwardSpeed = 0.0f;
	forwardSpeed = 0.0f;
	currentLane = 0;
}

glm::vec3 Player::GetPosition() const { return playerModel->GetPositon(); }

float Player::GetForwardSpeed() const { return forwardSpeed; }

////////////////////////////////////////////////////////////////////////////////////////////////
//							From: https://easings.net/#easeInOutCubic						  //
// 'number' represents absolute progress in animation (0 equals start and 1 end of animation) //
float Player::easeInOutCubic(const float number)
{
	return number < 0.5f ? static_cast<float>(4 * number * number * number) : static_cast<float>(1 - std::pow(-2 * number + 2, 3) / 2);
}
