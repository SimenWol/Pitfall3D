#pragma once
#include "../../models/cube/cubemodel.h"

class Physics;
class ObjectModel;
class Shader;

class Tile
{
public:
	enum TileTypes
	{
		FLOOR,
		WALL
	};
public:
	Tile(Physics* phys, Shader* shader, TileTypes type);
	~Tile();

	void SetPos(const int posIndex);
	void Update(glm::mat4 view, glm::mat4 projection);
	void Draw() const;
public:
	static constexpr float FLOORTILELENGTH = 25.0f;
private:
	ObjectModel* cubeModel;
};