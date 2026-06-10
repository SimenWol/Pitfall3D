#pragma once
#include "../models/player/playermodel.h"

class Physics;
class ObjectModel;
class Shader;

class Player
{
public:
	enum PlayerMovement
	{
		LEFT,
		RIGHT,
		FORWARD,
		BACKWARD, // temp
		JUMP,
		//CROUCH (?)
	};
	enum PlayerSpeed
	{
		SLOW,			// 8.5f
		NORMAL,			// 12.5f
		FAST,			// 17.5f
		HYPERSPEED,		// 22.5f
	};
public:
	Player(Physics* phys, Shader* shader);
	~Player();

	void SetSpeed(const PlayerSpeed mode);
	btRigidBody* GetRigidBody() const;

	void Move(const PlayerMovement action, const float deltaTime = 0.0f); // processkeyboard(?)
	void Update(glm::mat4 view, glm::mat4 projection) const;
	void Draw() const;
	void Reset();

	glm::vec3 GetPosition() const;
	float GetForwardSpeed() const;
private:
	float easeInOutCubic(const float number);
private:
	ObjectModel* playerModel;

	bool canJump = true;
	float jumpTimer = 0.0f;
	float jumpStrength = 8.0f;

	float sideSpeed = 0.0f;
	float laneAnimationTimer = 0.0f;
	float posBeforeSwitch = 0.0f;

	float upSpeed = 0.0f;
	float forwardSpeed = 0.0f;
	float maxForwardSpeed = 0.0f; // Speed should be set upon loading a level, default speed is 20.0f.
	int currentLane = 0;
};