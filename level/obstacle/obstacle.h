#pragma once
#include "../../models/cube/cubemodel.h"

class Physics;
class ObjectModel;
class Shader;
class Physics;
class Collectible;

class Obstacle
{
private:
	enum ObstacleVariants
	{
		LEFT,
		MIDDLE,
		RIGHT,
		LEFT_EMPTY,
		MIDDLE_EMPTY,
		RIGHT_EMPTY,
		LEFT_LINE,
		MIDDLE_LINE,
		RIGHT_LINE,
		LEFT_FALLEN,
		MIDDLE_FALLEN,
		RIGHT_FALLEN,
	};
public:
	Obstacle(Physics* phys, Shader* shader, Collectible* collectibleIn);
	~Obstacle();

	void SpawnObstacles(const int mapChunkPos);
	void UpdateAndDraw(glm::mat4 view, glm::mat4 projection);
private:
	void loadObstacleChunk(int mapChunkPos, ObstacleVariants variant);
	void placePillar(const glm::vec3 position, const glm::vec3 rotation = glm::vec3(0.0f,0.0f,0.0f));
private:
	std::vector<ObjectModel*> pillarModels;
	Collectible* collectible;

	Physics* physics;
	Shader* shader;
};