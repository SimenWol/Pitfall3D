#pragma once

#include "../objectmodel.h"
//#include "ModelManager.h"

class PillarModel : public ObjectModel
{
public:
	PillarModel(Physics* phys);
	PillarModel(char* fileName, Physics* phys);

	~PillarModel();

	bool Update(const glm::mat4 view, const glm::mat4 projection);
	bool Draw();
};