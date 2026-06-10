#pragma once

#include "../objectmodel.h"
//#include "ModelManager.h"

class PlayerModel : public ObjectModel
{
public:
	PlayerModel(Physics* phys);
	PlayerModel(char* fileName, Physics* phys);

	~PlayerModel();

	bool Update(const glm::mat4 view, const glm::mat4 projection);
	bool Draw();
};