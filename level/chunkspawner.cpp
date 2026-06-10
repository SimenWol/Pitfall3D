#include "chunkspawner.h"
#include <vector>
#include <cmath>
#include <iostream>
#include "../glm/glm/glm.hpp"
#include "tile/tile.h"
#include "obstacle/obstacle.h"
#include "collectible/collectible.h"
#include "../random.h"
#include <bullet/btBulletDynamicsCommon.h>
#include "../game.h"

///////////////////////////////////////////////////////////
// Class: Chunk
///////////////////////////////////////////////////////////
Chunk::Chunk(Physics* phys, Shader* shaderIn, int mapPos, Shader* tileShaderIn, int difficulty, Shader* pillarShaderIn)
	:physics(phys)
	,shader(shaderIn)
	,tileShader(tileShaderIn)
	,pillarShader(pillarShaderIn)
	,currentMapPos(mapPos)
{
	if (difficulty == 0) { noObjects = true; }
	else
	{
		noObjects = false;
		collectible = new Collectible(physics, shader);
		obstacle = new Obstacle(physics, pillarShader, collectible);
	}

	switch (difficulty)
	{
	case 0: break;
	case 1: // easy
		maxRandomNumber = 20;
		obstacleNumber = 8;
		collectibleNumber = 17;
		break;
	case 2: // medium
		maxRandomNumber = 20;
		obstacleNumber = 11;
		collectibleNumber = 17;
		break;
	case 3: // hard
		maxRandomNumber = 20;
		obstacleNumber = 13;
		collectibleNumber = 18;
		break;
	case 4: // impossible
		maxRandomNumber = 21;
		obstacleNumber = 14;
		collectibleNumber = 19;
		break;
	default:
		std::cout << "Unknown difficulty, using default values.\nKnown difficulties: 1 = easy, 2 = medium, 3 = hard, 4 = impossible." << std::endl;
		break;
	}

	CreateChunk();
}

Chunk::~Chunk()
{
	delete tile;
	if (!noObjects)
	{
		delete obstacle;
		delete collectible;
	}
}

Chunk* Chunk::CreateChunk()
{
	tile = new Tile(physics, tileShader, Tile::TileTypes::FLOOR);
	tile->SetPos(currentMapPos);
	if (!noObjects)
	{
		const int randomNumber = RandomUInt() % maxRandomNumber;

		if (randomNumber <= obstacleNumber) { obstacle->SpawnObstacles(currentMapPos); }
		else if (randomNumber <= collectibleNumber) { collectible->SpawnCollectibles(currentMapPos); }
	}
	return this;
}

void Chunk::Draw(glm::mat4 view, glm::mat4 projection, float deltaTime) const
{
	tile->Update(view, projection);
	tile->Draw();
	if (!noObjects)
	{
		obstacle->UpdateAndDraw(view, projection);
		collectible->UpdateAndDraw(view, projection, deltaTime);
	}
}

int Chunk::CollectCoin(const btRigidBody* body) const
{
	return collectible->CollectCoin(body);
}

int Chunk::GetMapPos() const { return currentMapPos; }


///////////////////////////////////////////////////////////
// Class: ChunkSpawner
///////////////////////////////////////////////////////////
ChunkSpawner::ChunkSpawner(Physics* phys, Shader* shaderIn, Shader* tileShaderIn, Shader* pillarShaderIn)
	:physics(phys)
	,shader(shaderIn)
	,tileShader(tileShaderIn)
	,pillarShader(pillarShaderIn)
{
	chunks.reserve(6);
	for (int i = 0; i < 6; i++)
	{
		chunks.push_back(new Chunk(physics, shader, i - 1, tileShader, 0, pillarShader));
	}
}

ChunkSpawner::~ChunkSpawner()
{
	// iterate through vector, freeing all memory on the heap
	std::vector<Chunk*>::iterator iter = chunks.begin();
	for (iter = chunks.begin(); iter != chunks.end(); ++iter)
	{
		delete* iter;
		*iter = nullptr;
	}
}

void ChunkSpawner::UpdateMap(float playerPos)
{
	const int newPlayerMapPos = static_cast<int>(std::round(playerPos / Tile::FLOORTILELENGTH));
	if (newPlayerMapPos == playerMapPos) { return; }
	if (newPlayerMapPos > playerMapPos)
	{
		delete chunks[nextChunk];
		chunks[nextChunk] = new Chunk(physics, shader, playerMapPos + chunks.size() - 1, tileShader, difficulty, pillarShader);

		playerMapPos = newPlayerMapPos;
		nextChunk++;
		if ((nextChunk + 1) > static_cast<int>(chunks.size())) { nextChunk = 0; }
	}
}

void ChunkSpawner::Draw(glm::mat4 view, glm::mat4 projection, float deltaTime) const
{
	for (int i = 0; i != static_cast<int>(chunks.size()); ++i) { chunks[i]->Draw(view, projection, deltaTime); }
}

int ChunkSpawner::CollectCoin(const btRigidBody* body) const
{
	for (int i = 0; i != static_cast<int>(chunks.size()); ++i)
	{
		if (chunks[i]->GetMapPos() == playerMapPos) { return chunks[i]->CollectCoin(body); }
	}
	return 0;
}

void ChunkSpawner::Reset()
{
	playerMapPos = 0;
	nextChunk = 0;

	// iterate through vector, freeing all memory on the heap
	std::vector<Chunk*>::iterator iter = chunks.begin();
	for (iter = chunks.begin(); iter != chunks.end(); ++iter)
	{
		delete* iter;
		*iter = nullptr;
	}

	// Empty vector elements
	chunks.clear();

	// create new tiles
	for (int i = 0; i < 6; i++)
	{
		chunks.push_back(new Chunk(physics, shader, i - 1, tileShader, 0, pillarShader));
	}
}

void ChunkSpawner::SetObstacleFrequency(const int difficultyIn)
{
	difficulty = difficultyIn;
}