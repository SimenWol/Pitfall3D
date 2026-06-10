#pragma once

#include "../objectmodel.h"
//#include "ModelManager.h"

class ShipModel : public ObjectModel
{
public:
	ShipModel(Physics* phys);
	ShipModel(char* fileName, Physics* phys);

	~ShipModel();

	bool Update(const glm::mat4 view, const glm::mat4 projection);
	bool Draw();
};