#include "menumanager.h"
#include "../game.h"
#include "../imgui/imgui.h"
#include "../random.h"
MenuManager::MenuManager(Game* gameIn)
	:game(gameIn)
{}

MenuManager::MenuType MenuManager::GetMenuState() const { return menuState; }

bool MenuManager::GetQuit() const { return quit; }

void MenuManager::Draw()
{
	switch (menuState)
	{
	case MenuManager::MAIN:
		DrawMain();
		break;
	case MenuManager::PLAYING:
		DrawPlaying();
		break;
	case MenuManager::PAUSED:
		DrawPaused();
		break;
	case MenuManager::GAMEOVER:
		DrawGameOver();
		break;
	case MenuManager::LEVELSELECT:
		DrawLevelSelect();
		break;
	default:
		break;
	}

	ImGui::Render();
}

void MenuManager::GenerateRandomName()
{
	randomName = GetRandomName(RandomUInt() % 5 + 3);
}

void MenuManager::ResetRandomName() { randomName = ""; }

void MenuManager::SetMenuState(const MenuType type) { menuState = type; }

void MenuManager::DrawMain()
{
	ImGuiStyle& style = ImGui::GetStyle();
	const ImVec2 originalWindowPadding = style.WindowPadding;
	style.WindowPadding = ImVec2(0, 0);

	ImGui::NewFrame();
	ImGui::SetNextWindowPos(ImVec2(400 - 33, 150 - 5));
	ImGui::SetNextWindowSize(ImVec2(75, 10));
	ImGui::Begin("MainMenuTitle", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	ImGui::Text("Pitfall3D");
	ImGui::End();

	ImGui::SetNextWindowPos(ImVec2(300 - 50, 300 - 25));
	ImGui::SetNextWindowSize(ImVec2(100, 50));
	ImGui::Begin("QuitButton", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	if (ImGui::Button("Quit", ImVec2(-1, -1))) { quit = true; }
	ImGui::End();

	ImGui::SetNextWindowPos(ImVec2(500 - 75, 300 - 25));
	ImGui::SetNextWindowSize(ImVec2(150, 50));
	ImGui::Begin("SelectLevelButton", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	if (ImGui::Button("Select Level", ImVec2(-1, -1))) { menuState = MenuType::LEVELSELECT; }
	ImGui::End();

#ifdef _DEBUG
	ImGui::SetNextWindowPos(ImVec2(400 - 150, 350));
	ImGui::SetNextWindowSize(ImVec2(300, 200));
#else
	ImGui::SetNextWindowPos(ImVec2(400 - 150, 350));
	ImGui::SetNextWindowSize(ImVec2(300, 75));
#endif
	ImGui::Begin("ControlsInfo", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	ImGui::Text("Controls:");
	ImGui::Text("Move left/right: A&D or arrow keys");
	ImGui::Text("Jump: Space bar or up arrow");
	ImGui::Text("Change camera view: Z");
#ifdef _DEBUG
	ImGui::Text("Following controls are DEBUG ONLY");
	ImGui::Text("Close program: Esc");
	ImGui::Text("Show debug info: P");
	ImGui::Text("Hide cursor: T");
	ImGui::Text("Freecam: F");
	ImGui::Text("Move in freecam: WASD + Q&E");
	ImGui::Text("Render Bullet Debug: R");
#endif // _DEBUG
	ImGui::End();

	ImGui::EndFrame();

	style.WindowPadding = originalWindowPadding;
}

void MenuManager::DrawLevelSelect()
{
	ImGuiStyle& style = ImGui::GetStyle();
	const ImVec2 originalWindowPadding = style.WindowPadding;
	style.WindowPadding = ImVec2(0, 0);

	ImGui::NewFrame();
	ImGui::SetNextWindowPos(ImVec2(400 - 60, 150 - 5));
	ImGui::SetNextWindowSize(ImVec2(120, 10));
	ImGui::Begin("LevelSelectTitle", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	ImGui::Text("SELECT DIFFICULTY");
	ImGui::End();

	ImGui::SetNextWindowPos(ImVec2(400 - 50, 250 - 25));
	ImGui::SetNextWindowSize(ImVec2(100, 50));
	ImGui::Begin("EasyButton", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	if (ImGui::Button("EASY", ImVec2(-1, -1)))
	{
		game->SetDifficulty(Game::Difficulty::EASY, true);
		menuState = MenuType::PLAYING;
		game->SetGameState(Game::GameStates::PLAYING);
	}
	ImGui::End();

	ImGui::SetNextWindowPos(ImVec2(400 - 50, 325 - 25));
	ImGui::SetNextWindowSize(ImVec2(100, 50));
	ImGui::Begin("MediumButton", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	if (ImGui::Button("MEDIUM", ImVec2(-1, -1)))
	{
		game->SetDifficulty(Game::Difficulty::MEDIUM, true);
		menuState = MenuType::PLAYING;
		game->SetGameState(Game::GameStates::PLAYING);
	}
	ImGui::End();

	ImGui::SetNextWindowPos(ImVec2(400 - 50, 400 - 25));
	ImGui::SetNextWindowSize(ImVec2(100, 50));
	ImGui::Begin("HardButton", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	if (ImGui::Button("HARD", ImVec2(-1, -1)))
	{
		game->SetDifficulty(Game::Difficulty::HARD, true);
		menuState = MenuType::PLAYING;
		game->SetGameState(Game::GameStates::PLAYING);
	}
	ImGui::End();

	ImGui::SetNextWindowPos(ImVec2(400 - 50, 475 - 25));
	ImGui::SetNextWindowSize(ImVec2(100, 50));
	ImGui::Begin("DynamicButton", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	if (ImGui::Button("DYNAMIC", ImVec2(-1, -1)))
	{
		game->SetDifficulty(Game::Difficulty::EASY, true, true);
		menuState = MenuType::PLAYING;
		game->SetGameState(Game::GameStates::PLAYING);
	}
	ImGui::End();

	ImGui::EndFrame();

	style.WindowPadding = originalWindowPadding;
}

void MenuManager::DrawPlaying()
{
	ImGuiStyle& style = ImGui::GetStyle();
	const ImVec2 originalWindowPadding = style.WindowPadding;
	style.WindowPadding = ImVec2(0, 0);

	ImGui::NewFrame();
	ImGui::SetNextWindowPos(ImVec2(20, 20));
	ImGui::SetNextWindowSize(ImVec2(30, 30));
	ImGui::Begin("PauseButton", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	if (ImGui::Button("II", ImVec2(-1, -1)))
	{
		menuState = MenuType::PAUSED;
		game->SetGameState(Game::GameStates::MENU);
	}
	ImGui::End();

	if (randomName != "")
	{
		ImGui::SetNextWindowPos(ImVec2(700, 20));
		ImGui::SetNextWindowSize(ImVec2(100, 50));
		ImGui::Begin("AreaName", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
		ImGui::Text("Current Area");
		ImGui::Text(randomName.c_str());
		ImGui::End();
	}

	game->DrawDebugInfo();

	ImGui::EndFrame();

	style.WindowPadding = originalWindowPadding;
}

void MenuManager::DrawPaused()
{
	ImGuiStyle& style = ImGui::GetStyle();
	const ImVec2 originalWindowPadding = style.WindowPadding;
	style.WindowPadding = ImVec2(0, 0);

	ImGui::NewFrame();
	ImGui::SetNextWindowPos(ImVec2(400 - 50, 150 - 5));
	ImGui::SetNextWindowSize(ImVec2(100, 10));
	ImGui::Begin("GamePausedTitle", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	ImGui::Text("GAME PAUSED");
	ImGui::End();

	ImGui::SetNextWindowPos(ImVec2(300 - 50, 300 - 25));
	ImGui::SetNextWindowSize(ImVec2(100, 50));
	ImGui::Begin("ContinueButton", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	if (ImGui::Button("Continue", ImVec2(-1, -1)))
	{
		menuState = MenuType::PLAYING;
		game->SetGameState(Game::GameStates::PLAYING);
	}
	ImGui::End();

	ImGui::SetNextWindowPos(ImVec2(500 - 75, 300 - 25));
	ImGui::SetNextWindowSize(ImVec2(150, 50));
	ImGui::Begin("MainMenuButton", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	if (ImGui::Button("Main Menu", ImVec2(-1, -1))) { menuState = MenuType::MAIN; }
	ImGui::End();

	ImGui::EndFrame();

	style.WindowPadding = originalWindowPadding;
}

void MenuManager::DrawGameOver()
{
	ImGuiStyle& style = ImGui::GetStyle();
	const ImVec2 originalWindowPadding = style.WindowPadding;
	style.WindowPadding = ImVec2(0, 0);

	ImGui::NewFrame();
	ImGui::SetNextWindowPos(ImVec2(400 - 100, 150 - 25));
	ImGui::SetNextWindowSize(ImVec2(200, 50));
	ImGui::Begin("GameOverTitle", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	ImGui::Text("Game Over!");
	ImGui::Text("Total Distance: %.1f", game->GetDistance());
	ImGui::Text("Coins Collected: %d", game->GetCoins());
	ImGui::End();

	ImGui::SetNextWindowPos(ImVec2(400 - 50, 300 - 25));
	ImGui::SetNextWindowSize(ImVec2(100, 50));
	ImGui::Begin("MainMenuButton", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	if (ImGui::Button("Main Menu", ImVec2(-1, -1))) { menuState = MenuType::MAIN; }
	ImGui::End();

	ImGui::EndFrame();

	style.WindowPadding = originalWindowPadding;
}
