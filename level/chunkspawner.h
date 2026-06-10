#pragma once
#include <vector>
#include "../glm/glm/glm.hpp"

class Tile;
class Physics;
class Shader;
class Obstacle;
class Collectible;
class btRigidBody;
class Difficulty;

class Chunk
{
public:
	Chunk(Physics* phys, Shader* shader, int mapPos, Shader* tileShaderIn, int difficulty, Shader* pillarShaderIn);
	~Chunk();
	void Draw(glm::mat4 view, glm::mat4 projection, float deltaTime) const;
	int CollectCoin(const btRigidBody* body) const;
	int GetMapPos() const;
private:
	Chunk* CreateChunk();
private:
	Physics* physics;
	Shader* shader;
	Shader* tileShader;
	Shader* pillarShader;
	const int currentMapPos;

	bool noObjects;
	Tile* tile;
	Obstacle* obstacle;
	Collectible* collectible;

	// Random object spawning
	int maxRandomNumber = 20;
	int obstacleNumber = 11;
	int collectibleNumber = 17;
};

class ChunkSpawner
{
public:
	ChunkSpawner(Physics* phys, Shader* shader, Shader* tileShaderIn, Shader* pillarShaderIn);
	~ChunkSpawner();

	void UpdateMap(float playerPos);
	void Draw(glm::mat4 view, glm::mat4 projection, float deltaTime) const;
	int CollectCoin(const btRigidBody* body) const;
	void Reset();

	void SetObstacleFrequency(const int difficultyIn);
private:
	Physics* physics;
	Shader* shader;
	Shader* tileShader;
	Shader* pillarShader;

	std::vector<Chunk*> chunks;

	int playerMapPos = 0;
	int nextChunk = 0;
	int difficulty = 1;
};