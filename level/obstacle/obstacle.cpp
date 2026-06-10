#include "obstacle.h"
#include <iostream>
#include "../glm/glm/glm.hpp"
#include "../../bullet/btBulletDynamicsCommon.h"
#include "../../physics/physics.h"
#include "../../random.h"
#include "../../models/pillar/pillarmodel.h"
#include "../../graphics/shader.h"
#include "../../physics/physicsobject.h"
#include "../tile/tile.h"
#include "../collectible/collectible.h"

Obstacle::Obstacle(Physics* phys, Shader* shaderIn, Collectible* collectibleIn)
	:collectible(collectibleIn)
	,physics(phys)
	,shader(shaderIn)
{}

Obstacle::~Obstacle()
{
	// iterate through vector, freeing all memory on the heap
	std::vector<ObjectModel*>::iterator iter = pillarModels.begin();
	for (iter = pillarModels.begin(); iter != pillarModels.end(); ++iter)
	{
		delete* iter;
		*iter = nullptr;
	}
}

void Obstacle::SpawnObstacles(const int mapChunkPos)
{
	const unsigned int randomNumber = RandomUInt() % 12;

	switch (randomNumber)
	{
	case 1:  loadObstacleChunk(mapChunkPos, ObstacleVariants::LEFT);		     break;
	case 2:  loadObstacleChunk(mapChunkPos, ObstacleVariants::MIDDLE);		 break;
	case 3:  loadObstacleChunk(mapChunkPos, ObstacleVariants::RIGHT);		 break;
	case 4:  loadObstacleChunk(mapChunkPos, ObstacleVariants::LEFT_EMPTY);	 break;
	case 5:  loadObstacleChunk(mapChunkPos, ObstacleVariants::MIDDLE_EMPTY);  break;
	case 6:  loadObstacleChunk(mapChunkPos, ObstacleVariants::RIGHT_EMPTY);   break;
	case 7:  loadObstacleChunk(mapChunkPos, ObstacleVariants::LEFT_LINE);	 break;
	case 8:  loadObstacleChunk(mapChunkPos, ObstacleVariants::MIDDLE_LINE);	 break;
	case 9:  loadObstacleChunk(mapChunkPos, ObstacleVariants::RIGHT_LINE);	 break;
	case 10: loadObstacleChunk(mapChunkPos, ObstacleVariants::LEFT_FALLEN);	 break;
	case 11: loadObstacleChunk(mapChunkPos, ObstacleVariants::MIDDLE_FALLEN); break;
	case 12: loadObstacleChunk(mapChunkPos, ObstacleVariants::RIGHT_FALLEN);  break;
	default:																		 break;
	}
}

void Obstacle::UpdateAndDraw(glm::mat4 view, glm::mat4 projection)
{
	for (unsigned int i = 0; i != pillarModels.size(); ++i)
	{
		pillarModels[i]->Update(view, projection);
		pillarModels[i]->Draw();
	}
}

void Obstacle::loadObstacleChunk(int mapChunkPos, ObstacleVariants variant)
{
	glm::vec3 pos(0.0f, 0.0f, 0.0f);

	constexpr float leftLane = -2.0f;
	constexpr float rightLane = 2.0f;
	constexpr float coinGroundHeight = 1.0f;
	constexpr float coinPillarHeight = 2.5f;
	
	switch (variant)
	{
	case Obstacle::LEFT:
	{
		// Obstacles
		pos = glm::vec3(leftLane, 2.5f, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH);
		placePillar(pos);

		// Collectibles
		unsigned int randomNumber = RandomUInt() % 4;
		switch (randomNumber)
		{
		case 1:
		{
			collectible->PlaceCollectible(glm::vec3(0.0f, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
		}
		break;
		case 2:
		{
			collectible->PlaceCollectible(glm::vec3(rightLane, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
		}
		break;
		case 3:
		{
			collectible->PlaceCollectible(glm::vec3(0.0f, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
			collectible->PlaceCollectible(glm::vec3(rightLane, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
		}
		break;
		default: // 4 - nothing
			break;
		}
	}
		break;
	case Obstacle::MIDDLE:
	{
		// Obstacles
		pos = glm::vec3(0.0f, 2.5f, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH);
		placePillar(pos);

		// Collectibles
		unsigned int randomNumber = RandomUInt() % 4;
		switch (randomNumber)
		{
		case 1:
		{
			collectible->PlaceCollectible(glm::vec3(leftLane, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
		}
		break;
		case 2:
		{
			collectible->PlaceCollectible(glm::vec3(rightLane, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
		}
		break;
		case 3:
		{
			collectible->PlaceCollectible(glm::vec3(leftLane, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
			collectible->PlaceCollectible(glm::vec3(rightLane, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
		}
		break;
		default: // 4 - nothing
			break;
		}
	}
		break;
	case Obstacle::RIGHT:
	{
		// Obstacles
		pos = glm::vec3(rightLane, 2.5f, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH);
		placePillar(pos);

		// Collectibles
		unsigned int randomNumber = RandomUInt() % 4;
		switch (randomNumber)
		{
		case 1:
		{
			collectible->PlaceCollectible(glm::vec3(leftLane, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
		}
		break;
		case 2:
		{
			collectible->PlaceCollectible(glm::vec3(0.0f, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
		}
		break;
		case 3:
		{
			collectible->PlaceCollectible(glm::vec3(leftLane, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
			collectible->PlaceCollectible(glm::vec3(0.0f, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
		}
		break;
		default: // 4 - nothing
			break;
		}
	}
		break;
	case Obstacle::LEFT_EMPTY:
	{
		// Obstacles
		pos = glm::vec3(0.0f, 2.5f, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH);
		placePillar(pos);
		pos = glm::vec3(rightLane, 2.5f, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH);
		placePillar(pos);

		// Collectibles
		unsigned int randomNumber = RandomUInt() % 2;
		switch (randomNumber)
		{
		case 1:
		{
			collectible->PlaceCollectible(glm::vec3(leftLane, coinGroundHeight, static_cast<float>(mapChunkPos)* Tile::FLOORTILELENGTH));
		}
		break;
		default: // 2 - nothing
			break;
		}
	}
		break;
	case Obstacle::MIDDLE_EMPTY:
	{
		// Obstacles
		pos = glm::vec3(leftLane, 2.5f, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH);
		placePillar(pos);
		pos = glm::vec3(rightLane, 2.5f, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH);
		placePillar(pos);

		// Collectibles
		unsigned int randomNumber = RandomUInt() % 2;
		switch (randomNumber)
		{
		case 1:
		{
			collectible->PlaceCollectible(glm::vec3(0.0f, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
		}
		break;
		default: // 2 - nothing
			break;
		}
	}
		break;
	case Obstacle::RIGHT_EMPTY:
	{
		// Obstacles
		pos = glm::vec3(leftLane, 2.5f, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH);
		placePillar(pos);
		pos = glm::vec3(0.0f, 2.5f, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH);
		placePillar(pos);

		// Collectibles
		unsigned int randomNumber = RandomUInt() % 2;
		switch (randomNumber)
		{
		case 1:
		{
			collectible->PlaceCollectible(glm::vec3(rightLane, coinGroundHeight, static_cast<float>(mapChunkPos)* Tile::FLOORTILELENGTH));
		}
		break;
		default: // 2 - nothing
			break;
		}
	}
		break;
	case Obstacle::LEFT_LINE:
	{
		// Obstacles
		pos = glm::vec3(leftLane, 2.5f, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH - 5.0f);
		placePillar(pos);
		pos = glm::vec3(leftLane, 2.5f, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH);
		placePillar(pos);
		pos = glm::vec3(leftLane, 2.5f, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH + 5.0f);
		placePillar(pos);

		// Collectibles
		unsigned int randomNumber = RandomUInt() % 4;
		switch (randomNumber)
		{
		case 1:
		{
			collectible->PlaceCollectible(glm::vec3(0.0f, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH - 5.0f));
			collectible->PlaceCollectible(glm::vec3(0.0f, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
			collectible->PlaceCollectible(glm::vec3(0.0f, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH + 5.0f));
		}
		break;
		case 2:
		{
			collectible->PlaceCollectible(glm::vec3(rightLane, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH - 5.0f));
			collectible->PlaceCollectible(glm::vec3(rightLane, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
			collectible->PlaceCollectible(glm::vec3(rightLane, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH + 5.0f));
		}
		break;
		case 3:
		{
			collectible->PlaceCollectible(glm::vec3(0.0f, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH - 5.0f));
			collectible->PlaceCollectible(glm::vec3(0.0f, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
			collectible->PlaceCollectible(glm::vec3(0.0f, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH + 5.0f));
			collectible->PlaceCollectible(glm::vec3(rightLane, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH - 5.0f));
			collectible->PlaceCollectible(glm::vec3(rightLane, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
			collectible->PlaceCollectible(glm::vec3(rightLane, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH + 5.0f));
		}
		break;
		default: // 4 - nothing
			break;
		}
	}
		break;
	case Obstacle::MIDDLE_LINE:
	{
		// Obstacles
		pos = glm::vec3(0.0f, 2.5f, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH - 5.0f);
		placePillar(pos);
		pos = glm::vec3(0.0f, 2.5f, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH);
		placePillar(pos);
		pos = glm::vec3(0.0f, 2.5f, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH + 5.0f);
		placePillar(pos);

		// Collectibles
		unsigned int randomNumber = RandomUInt() % 4;
		switch (randomNumber)
		{
		case 1:
		{
			collectible->PlaceCollectible(glm::vec3(leftLane, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH - 5.0f));
			collectible->PlaceCollectible(glm::vec3(leftLane, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
			collectible->PlaceCollectible(glm::vec3(leftLane, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH + 5.0f));
		}
		break;
		case 2:
		{
			collectible->PlaceCollectible(glm::vec3(rightLane, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH - 5.0f));
			collectible->PlaceCollectible(glm::vec3(rightLane, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
			collectible->PlaceCollectible(glm::vec3(rightLane, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH + 5.0f));
		}
		break;
		case 3:
		{
			collectible->PlaceCollectible(glm::vec3(leftLane, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH - 5.0f));
			collectible->PlaceCollectible(glm::vec3(leftLane, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
			collectible->PlaceCollectible(glm::vec3(leftLane, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH + 5.0f));
			collectible->PlaceCollectible(glm::vec3(rightLane, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH - 5.0f));
			collectible->PlaceCollectible(glm::vec3(rightLane, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
			collectible->PlaceCollectible(glm::vec3(rightLane, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH + 5.0f));
		}
		break;
		default: // 4 - nothing
			break;
		}
	}
		break;
	case Obstacle::RIGHT_LINE:
	{
		// Obstacles
		pos = glm::vec3(rightLane, 2.5f, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH - 5.0f);
		placePillar(pos);
		pos = glm::vec3(rightLane, 2.5f, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH);
		placePillar(pos);
		pos = glm::vec3(rightLane, 2.5f, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH + 5.0f);
		placePillar(pos);

		// Collectibles
		unsigned int randomNumber = RandomUInt() % 4;
		switch (randomNumber)
		{
		case 1:
		{
			collectible->PlaceCollectible(glm::vec3(leftLane, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH - 5.0f));
			collectible->PlaceCollectible(glm::vec3(leftLane, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
			collectible->PlaceCollectible(glm::vec3(leftLane, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH + 5.0f));
		}
		break;
		case 2:
		{
			collectible->PlaceCollectible(glm::vec3(0.0f, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH - 5.0f));
			collectible->PlaceCollectible(glm::vec3(0.0f, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
			collectible->PlaceCollectible(glm::vec3(0.0f, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH + 5.0f));
		}
		break;
		case 3:
		{
			collectible->PlaceCollectible(glm::vec3(leftLane, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH - 5.0f));
			collectible->PlaceCollectible(glm::vec3(leftLane, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
			collectible->PlaceCollectible(glm::vec3(leftLane, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH + 5.0f));
			collectible->PlaceCollectible(glm::vec3(0.0f, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH - 5.0f));
			collectible->PlaceCollectible(glm::vec3(0.0f, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
			collectible->PlaceCollectible(glm::vec3(0.0f, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH + 5.0f));
		}
		break;
		default: // 4 - nothing
			break;
		}
	}
		break;
	case Obstacle::LEFT_FALLEN:
	{
		// Obstacles
		pos = glm::vec3(leftLane, 0.75f, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH);
		placePillar(pos, glm::vec3(0.0f, 0.0f, -90.0f));

		// Collectibles
		unsigned int randomNumber = RandomUInt() % 5;
		switch (randomNumber)
		{
		case 1:
		{
			collectible->PlaceCollectible(glm::vec3(leftLane, coinPillarHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
		}
		break;
		case 2:
		{
			collectible->PlaceCollectible(glm::vec3(0.0f, coinPillarHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
		}
		break;
		case 3:
		{
			collectible->PlaceCollectible(glm::vec3(rightLane, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
		}
		break;
		case 4:
		{
			collectible->PlaceCollectible(glm::vec3(leftLane, coinPillarHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
			collectible->PlaceCollectible(glm::vec3(0.0f, coinPillarHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
		}
		break;
		default: // 5 - nothing
			break;
		}
	}
		break;
	case Obstacle::MIDDLE_FALLEN:
	{
		// Obstacles
		pos = glm::vec3(0.0f, 0.75f, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH);
		if (RandomUInt() % 2 == 0) { placePillar(pos, glm::vec3(0.0f, 0.0f, 90.0f)); }
		else { placePillar(pos, glm::vec3(0.0f, 0.0f, -90.0f)); }

		// Collectibles
		unsigned int randomNumber = RandomUInt() % 5;
		switch (randomNumber)
		{
		case 1:
		{
			collectible->PlaceCollectible(glm::vec3(leftLane, coinPillarHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
		}
		break;
		case 2:
		{
			collectible->PlaceCollectible(glm::vec3(0.0f, coinPillarHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
		}
		break;
		case 3:
		{
			collectible->PlaceCollectible(glm::vec3(rightLane, coinPillarHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
		}
		break;
		case 4:
		{
			collectible->PlaceCollectible(glm::vec3(leftLane, coinPillarHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
			collectible->PlaceCollectible(glm::vec3(0.0f, coinPillarHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
			collectible->PlaceCollectible(glm::vec3(rightLane, coinPillarHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
		}
		break;
		default: // 5 - nothing
			break;
		}
	}
		break;
	case Obstacle::RIGHT_FALLEN:
	{
		// Obstacles
		pos = glm::vec3(rightLane, 0.75f, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH);
		placePillar(pos, glm::vec3(0.0f, 0.0f, 90.0f));

		// Collectibles
		unsigned int randomNumber = RandomUInt() % 5;
		switch (randomNumber)
		{
		case 1:
		{
			collectible->PlaceCollectible(glm::vec3(leftLane, coinGroundHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
		}
		break;
		case 2:
		{
			collectible->PlaceCollectible(glm::vec3(0.0f, coinPillarHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
		}
		break;
		case 3:
		{
			collectible->PlaceCollectible(glm::vec3(rightLane, coinPillarHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
		}
		break;
		case 4:
		{
			collectible->PlaceCollectible(glm::vec3(0.0f, coinPillarHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
			collectible->PlaceCollectible(glm::vec3(rightLane, coinPillarHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
		}
		break;
		default: // 5 - nothing
			break;
		}
	}
		break;
	default:
		std::cout << "Unknown variant" << std::endl;
		break;
	}
}

void Obstacle::placePillar(const glm::vec3 position, const glm::vec3 rotation)
{
	// Create pillar model and set position/rotation.
	pillarModels.push_back(new PillarModel((char*)"../../../resources/models/pillar.obj", physics));
	const int thisModel = pillarModels.size() - 1;
	pillarModels[thisModel]->Scales = glm::vec3(0.01f, 0.01f, 0.01f);
	pillarModels[thisModel]->SetPosition(position);
	pillarModels[thisModel]->SetXRotation(rotation.x * SIMD_RADS_PER_DEG); //
	pillarModels[thisModel]->SetYRotation(rotation.y * SIMD_RADS_PER_DEG); // Rotations are stored as radians!
	pillarModels[thisModel]->SetZRotation(rotation.z * SIMD_RADS_PER_DEG); //
	pillarModels[thisModel]->StoreGraphicClass(shader);

	// Add RigidBody
	btVector3 modelPos = btVector3(pillarModels[thisModel]->GetPositon().x, pillarModels[thisModel]->GetPositon().y, pillarModels[thisModel]->GetPositon().z);
	btQuaternion modelRot;
	modelRot.setEuler(rotation.x * SIMD_RADS_PER_DEG, rotation.y * SIMD_RADS_PER_DEG, rotation.z * SIMD_RADS_PER_DEG);
	this->pillarModels[thisModel]->myPhysObj = pillarModels[thisModel]->CreatePhysicsObj(pillarModels[thisModel]->CreateMyShape(ObjectModel::CONVEXHULL), 0, modelPos, modelRot);

	// Set obstacle flags
	pillarModels[thisModel]->myPhysObj->GetRigidBody()->getBroadphaseHandle()->m_collisionFilterGroup = Physics::OBSTACLE_GROUP_FLAG;
	pillarModels[thisModel]->myPhysObj->GetRigidBody()->getBroadphaseHandle()->m_collisionFilterMask  = Physics::ALL_OBJECTS_MASK;
}
