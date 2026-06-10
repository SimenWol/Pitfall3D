#pragma once
#include <string>
class Game;

class MenuManager
{
public:
	enum MenuType
	{
		MAIN,
		PLAYING,
		PAUSED,
		GAMEOVER,
		LEVELSELECT,
	};
public:
	MenuManager(Game* gameIn);
	~MenuManager() = default;

	void SetMenuState(const MenuType type);
	MenuType GetMenuState() const;
	bool GetQuit() const;

	//void Tick();
	void Draw();
	void GenerateRandomName();
	void ResetRandomName();
private:
	void DrawMain();
	void DrawLevelSelect();
	void DrawPlaying();
	void DrawPaused();
	void DrawGameOver();
private:
	Game* game;
	std::string randomName = "";

	MenuType menuState = MenuType::MAIN;

	bool quit = false;
};