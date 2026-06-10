#include "tile.h"
#include "../glm/glm/glm.hpp"
#include "../../bullet/btBulletDynamicsCommon.h"
#include "../../physics/physics.h"
#include "../../models/cube/cubemodel.h"
#include "../../graphics/shader.h"
#include "../../physics/physicsobject.h"

Tile::Tile(Physics* phys, Shader* shader, TileTypes type)
{
	// Create cube model and set position/rotation.
	cubeModel = new CubeModel((char*)"../../../resources/testing/container2.png", phys);
	const glm::vec3 pos = glm::vec3(0.0f, -0.5f, 0.0f);
	cubeModel->Scales = glm::vec3(10.0f, 1.0f, FLOORTILELENGTH);
	cubeModel->SetPosition(pos);
	cubeModel->SetXRotation(0.0f);
	cubeModel->SetYRotation(0.0f);
	cubeModel->SetZRotation(0.0f);
	cubeModel->StoreGraphicClass(shader);

	// Add RigidBody
	btVector3 modelPos = btVector3(cubeModel->GetPositon().x, cubeModel->GetPositon().y, cubeModel->GetPositon().z);
	btQuaternion modelRot = btQuaternion(cubeModel->GetRotations().x, cubeModel->GetRotations().y, cubeModel->GetRotations().z, 1);
	this->cubeModel->myPhysObj = cubeModel->CreatePhysicsObj(cubeModel->CreateMyShape(ObjectModel::BOX, btVector3(5.0f, 0.5f, (FLOORTILELENGTH / 2.0f))), 0, modelPos, modelRot);

	cubeModel->myPhysObj->GetRigidBody()->getBroadphaseHandle()->m_collisionFilterMask = Physics::ALL_OBJECTS_MASK;
}

Tile::~Tile() { delete cubeModel; }

void Tile::SetPos(const int posIndex)
{
	btTransform transform = cubeModel->myPhysObj->GetRigidBody()->getWorldTransform();
	transform.setOrigin(btVector3(0.0f, -0.5f, static_cast<float>(posIndex) * FLOORTILELENGTH));
	cubeModel->myPhysObj->GetRigidBody()->setWorldTransform(transform);
	this->cubeModel->SetPosition(glm::vec3(0.0f, -0.5f, static_cast<float>(posIndex) * FLOORTILELENGTH));
}

void Tile::Update(glm::mat4 view, glm::mat4 projection) { this->cubeModel->Update(view, projection); }

void Tile::Draw() const { this->cubeModel->Draw(); }
