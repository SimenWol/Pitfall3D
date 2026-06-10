#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <sys/time.h>
#include "camera.h"

class GameWindow;
class Physics;
class PhysicsDraw;
class ObjectModel;
class Camera;
class ImGuiIO;
class Player;
class ChunkSpawner;
class SkyBox;
class MenuManager;

class Shader;

class Game
{
public:
	enum Difficulty
	{
		EASY,
		MEDIUM,
		HARD,
		IMPOSSIBLE,
	};

	enum GameStates
	{
		MENU,
		PLAYING,
	};
public:
	Game();
	~Game();

	void Tick();

	void DrawDebugInfo();

	void SetGameState(const GameStates state);
	void SetDifficulty(const Difficulty difficulty, const bool reset = false, const bool dynamic = false);

	float GetDistance() const;
	int GetCoins() const;
private:
	void Init();

	void DrawDebug() const;
	unsigned int LoadTexture(const char* path);
	void ProcessInput();
	void CalcMouseOffset(int x, int y);

private:
	GameWindow* window;
	ImGuiIO* io;

	// Physics
	Physics* physics;
	PhysicsDraw* physicsDraw;

	// Utility
	Camera* camera;
	Shader* lightingShader;
	Shader* testShader;
	Shader* skyBoxShader;
	Shader* pillarShader;

	MenuManager* menu;

	// Level
	Player* player;
	ChunkSpawner* map;
	SkyBox* skyBox;

	Difficulty currentDifficulty;
	bool dynamic = false;
	int lastNameChange = 0;

	GameStates gameState = GameStates::MENU;

	// Window
	int winWidth, winHeight;
	const int SCRWIDTH = 800;
	const int SCRHEIGHT = 600;

	// Camera
	View_Angles currentAngle = View_Angles::BACK;
	float lastX = static_cast<float>(SCRWIDTH) / 2.0f;
	float lastY = static_cast<float>(SCRHEIGHT) / 2.0f;
	bool firstMouse = true;
	bool lockCursor = true;

	// Player
	bool movingForward = false;
	int playerSpeed = 0;

	int collectedCoins = 0;

	// Debug
	bool doDebugDraw = false;
	bool freeCam = false;
	bool showDebugInfo = false;

	// Time
	float deltaTime;
	float totalTime = 0.0f;
	float FPS, appAverage;
	unsigned int frames = 0;
	struct timeval t1, t2;
	struct timezone tz;

	glm::mat4 projection = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
};