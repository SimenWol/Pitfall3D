#include "collectible.h"
#include "../glm/glm/glm.hpp"
#include "../../bullet/btBulletDynamicsCommon.h"
#include "../../physics/physics.h"
#include "../../random.h"
#include "../../models/coin/coinmodel.h"
#include "../../graphics/shader.h"
#include "../../physics/physicsobject.h"
#include "../tile/tile.h"

Collectible::Collectible(Physics* phys, Shader* shaderIn)
	:physics(phys)
	,shader(shaderIn)
{}

Collectible::~Collectible()
{
	// Iterate through vector, freeing all memory on the heap
	std::vector<CoinModel*>::iterator iter = coinModels.begin();
	for (iter = coinModels.begin(); iter != coinModels.end(); ++iter)
	{
		delete* iter;
		*iter = nullptr;
	}
}

void Collectible::SpawnCollectibles(const int mapChunkPos)
{
	unsigned int randomNumber = RandomUInt() % 12;

	switch (randomNumber)
	{
	case 1:  loadCollectibleChunk(mapChunkPos, CollectibleVariants::LEFT);		  break;
	case 2:  loadCollectibleChunk(mapChunkPos, CollectibleVariants::MIDDLE);		  break;
	case 3:  loadCollectibleChunk(mapChunkPos, CollectibleVariants::RIGHT);		  break;
	case 4:  loadCollectibleChunk(mapChunkPos, CollectibleVariants::LEFT_EMPTY);   break;
	case 5:  loadCollectibleChunk(mapChunkPos, CollectibleVariants::MIDDLE_EMPTY); break;
	case 6:  loadCollectibleChunk(mapChunkPos, CollectibleVariants::RIGHT_EMPTY);  break;
	case 7:  loadCollectibleChunk(mapChunkPos, CollectibleVariants::LEFT_LINE);	  break;
	case 8:  loadCollectibleChunk(mapChunkPos, CollectibleVariants::MIDDLE_LINE);  break;
	case 9:  loadCollectibleChunk(mapChunkPos, CollectibleVariants::RIGHT_LINE);   break;
	default: break;
	}
}

void Collectible::PlaceCollectible(const glm::vec3 pos)
{
	// Create pillar model and set position/rotation.
	coinModels.push_back(new CoinModel((char*)"../../../resources/models/coin.obj", physics));
	int thisModel = coinModels.size() - 1;
	coinModels[thisModel]->SetPosition(pos);
	coinModels[thisModel]->Scales = glm::vec3(0.07f, 0.07f, 0.07f);
	coinModels[thisModel]->SetXRotation(0.0f);
	coinModels[thisModel]->SetYRotation(0.0f);
	coinModels[thisModel]->SetZRotation(0.0f);
	coinModels[thisModel]->StoreGraphicClass(shader);

	// Add RigidBody
	btVector3 modelPos = btVector3(coinModels[thisModel]->GetPositon().x, coinModels[thisModel]->GetPositon().y + 0.6f, coinModels[thisModel]->GetPositon().z);
	btQuaternion modelRot = btQuaternion(coinModels[thisModel]->GetRotations().x, coinModels[thisModel]->GetRotations().y, coinModels[thisModel]->GetRotations().z, 1);
	this->coinModels[thisModel]->myPhysObj = coinModels[thisModel]->CreatePhysicsObj(coinModels[thisModel]->CreateMyShape(ObjectModel::BOX, btVector3(0.6f, 0.6f, 0.2f)), 0, modelPos, modelRot);

	// Set collectible flags
	coinModels[thisModel]->myPhysObj->GetRigidBody()->getBroadphaseHandle()->m_collisionFilterGroup = Physics::COLLECTIBLE_GROUP_FLAG;
	// Make sure it's still collideable but the player should be able to pass through.
	coinModels[thisModel]->myPhysObj->GetRigidBody()->getBroadphaseHandle()->m_collisionFilterMask = Physics::ALL_OBJECTS_MASK;
	coinModels[thisModel]->myPhysObj->GetRigidBody()->setCollisionFlags(coinModels[thisModel]->myPhysObj->GetRigidBody()->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
}

void Collectible::UpdateAndDraw(glm::mat4 view, glm::mat4 projection, const float deltaTime)
{
	for (unsigned int i = 0; i != coinModels.size(); ++i)
	{
		coinModels[i]->Rotate(deltaTime);
		coinModels[i]->Update(view, projection);
		coinModels[i]->Draw();
	}
}

int Collectible::CollectCoin(const btRigidBody* body)
{
	// Check for matching coin rigidbody (and remove if match)
	int i = 0;
	std::vector<CoinModel*>::iterator iter = coinModels.begin();
	for (iter = coinModels.begin(); iter != coinModels.end(); ++iter, i++)
	{
		if (coinModels[i]->myPhysObj->GetRigidBody() == body)
		{
			delete coinModels[i];
			coinModels.erase(iter);
			return 1;
		}
	}

	// If no matching coin is found, return 0.
	return 0;
}

void Collectible::loadCollectibleChunk(const int mapChunkPos, const CollectibleVariants variant)
{
	constexpr float leftLane = -2.0f;
	constexpr float rightLane = 2.0f;
	constexpr float coinHeight = 1.0f;

	switch (variant)
	{
	case Collectible::LEFT:
		PlaceCollectible(glm::vec3(leftLane, coinHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
		break;
	case Collectible::MIDDLE:
		PlaceCollectible(glm::vec3(0.0f, coinHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
		break;
	case Collectible::RIGHT:
		PlaceCollectible(glm::vec3(rightLane, coinHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
		break;
	case Collectible::LEFT_EMPTY:
		PlaceCollectible(glm::vec3(0.0f, coinHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
		PlaceCollectible(glm::vec3(rightLane, coinHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
		break;
	case Collectible::MIDDLE_EMPTY:
		PlaceCollectible(glm::vec3(leftLane, coinHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
		PlaceCollectible(glm::vec3(rightLane, coinHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
		break;
	case Collectible::RIGHT_EMPTY:
		PlaceCollectible(glm::vec3(leftLane, coinHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
		PlaceCollectible(glm::vec3(0.0f, coinHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
		break;
	case Collectible::LEFT_LINE:
		PlaceCollectible(glm::vec3(leftLane, coinHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH - 5.0f));
		PlaceCollectible(glm::vec3(leftLane, coinHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
		PlaceCollectible(glm::vec3(leftLane, coinHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH + 5.0f));
		break;
	case Collectible::MIDDLE_LINE:
		PlaceCollectible(glm::vec3(0.0f, coinHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH - 5.0f));
		PlaceCollectible(glm::vec3(0.0f, coinHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
		PlaceCollectible(glm::vec3(0.0f, coinHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH + 5.0f));
		break;
	case Collectible::RIGHT_LINE:
		PlaceCollectible(glm::vec3(rightLane, coinHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH - 5.0f));
		PlaceCollectible(glm::vec3(rightLane, coinHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
		PlaceCollectible(glm::vec3(rightLane, coinHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH + 5.0f));
		break;
	case Collectible::LEFT_LINE_EMPTY:
		PlaceCollectible(glm::vec3(0.0f, coinHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH - 5.0f));
		PlaceCollectible(glm::vec3(0.0f, coinHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
		PlaceCollectible(glm::vec3(0.0f, coinHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH + 5.0f));

		PlaceCollectible(glm::vec3(rightLane, coinHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH - 5.0f));
		PlaceCollectible(glm::vec3(rightLane, coinHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
		PlaceCollectible(glm::vec3(rightLane, coinHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH + 5.0f));
		break;
	case Collectible::MIDDLE_LINE_EMPTY:
		PlaceCollectible(glm::vec3(leftLane, coinHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH - 5.0f));
		PlaceCollectible(glm::vec3(leftLane, coinHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
		PlaceCollectible(glm::vec3(leftLane, coinHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH + 5.0f));

		PlaceCollectible(glm::vec3(rightLane, coinHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH - 5.0f));
		PlaceCollectible(glm::vec3(rightLane, coinHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
		PlaceCollectible(glm::vec3(rightLane, coinHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH + 5.0f));
		break;
	case Collectible::RIGHT_LINE_EMPTY:
		PlaceCollectible(glm::vec3(leftLane, coinHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH - 5.0f));
		PlaceCollectible(glm::vec3(leftLane, coinHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
		PlaceCollectible(glm::vec3(leftLane, coinHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH + 5.0f));

		PlaceCollectible(glm::vec3(0.0f, coinHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH - 5.0f));
		PlaceCollectible(glm::vec3(0.0f, coinHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH));
		PlaceCollectible(glm::vec3(0.0f, coinHeight, static_cast<float>(mapChunkPos) * Tile::FLOORTILELENGTH + 5.0f));
		break;
	default:
		break;
	}
}
