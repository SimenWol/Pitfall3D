#pragma once

#include "../objectmodel.h"

class CubeModel : public ObjectModel
{
public:
	CubeModel(Physics* phys);
	CubeModel(const char* path, Physics* phys);

	~CubeModel();

	bool Update(const glm::mat4 view, const glm::mat4 projection); // we supply an update
	bool Draw(); // and a draw 

	int NUMBER;
};