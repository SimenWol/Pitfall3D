#pragma once

#include "../objectmodel.h"
//#include "ModelManager.h"

class CoinModel : public ObjectModel
{
public:
	CoinModel(Physics* phys);
	CoinModel(char* fileName, Physics* phys);

	~CoinModel();

	void Rotate(const float deltaTime);
	bool Update(const glm::mat4 view, const glm::mat4 projection);  // we supply an update
	bool Draw();  // and a draw 
private:
	float currentRotAngle = 0.0f;
};