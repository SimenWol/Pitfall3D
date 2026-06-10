#pragma once
#include "../../models/cube/cubemodel.h"

class Physics;
class CoinModel;
class Shader;
class Physics;
class btRigidBody;

class Collectible
{
private:
	enum CollectibleVariants
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
		LEFT_LINE_EMPTY,
		MIDDLE_LINE_EMPTY,
		RIGHT_LINE_EMPTY,
	};
public:
	Collectible(Physics* phys, Shader* shader);
	~Collectible();

	void SpawnCollectibles(const int mapChunkPos);
	void PlaceCollectible(const glm::vec3 pos);
	void UpdateAndDraw(glm::mat4 view, glm::mat4 projection, const float deltaTime);
	int CollectCoin(const btRigidBody* body);
private:
	void loadCollectibleChunk(const int mapChunkPos, const CollectibleVariants variant);
private:
	std::vector<CoinModel*> coinModels;
	Physics* physics;
	Shader* shader;
};