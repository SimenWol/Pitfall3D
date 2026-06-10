#include "game.h"

#include <iostream>
#include <sys/time.h>
#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h> // OpenGL logging
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "graphics/stb_image.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"

#include "random.h"

#include "window/window.h"
#include "graphics/shader.h"

#include "physics/physics.h"
#include "physics/physicsdraw.h"

#include "camera.h"

#include "models/skybox/skybox.h"
#include "player/player.h"
#include "level/chunkspawner.h"
#include "menu/menumanager.h"

Game::Game()
{
	// Create game window
	window = new GameWindow();
#ifdef _DEBUG
	window->Init(SCRWIDTH, SCRHEIGHT, "Pitfall3D - Debug");
#else
	window->Init(SCRWIDTH, SCRHEIGHT, "Pitfall3D");
#endif // _DEBUG

	// Window settings
	window->Vsync(false);
	window->GetSize(winWidth, winHeight);
	glViewport(0, 0, winWidth, winHeight);

	// OpenGL logging | Code by Lasse (Y2)
	const auto pegl_debug_message_control_khr = reinterpret_cast<PFNGLDEBUGMESSAGECALLBACKKHRPROC>(eglGetProcAddress("glDebugMessageCallback"));
	if (pegl_debug_message_control_khr == nullptr) {
		std::cout << "failed to eglGetProcAddress eglDebugMessageControlKHR" << std::endl;
	}
	else {
		const GLDEBUGPROCKHR debug_fn = [](GLenum source, GLenum type, GLuint id, const GLenum severity, GLsizei length, const GLchar* message, const void*)
			{
				switch (severity)
				{
				case GL_DEBUG_SEVERITY_HIGH_KHR:
				case GL_DEBUG_SEVERITY_MEDIUM_KHR:
					std::cout << message << std::endl;
				case GL_DEBUG_SEVERITY_LOW_KHR:
				case GL_DEBUG_SEVERITY_NOTIFICATION_KHR:
				default:
					break; //Ignore.
				}
			};
		pegl_debug_message_control_khr(debug_fn, nullptr);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_KHR);
	}

	// ImGui setup
	const char* glsl_version = "#version 300 es";
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Bullet setup
	physics = new Physics();
	physicsDraw = new PhysicsDraw();
	physics->world->setDebugDrawer(physicsDraw);
	physicsDraw->setDebugMode(1);

	// Camera
	camera = new Camera(glm::vec3(0.0f, 3.0f, -10.0f));

	Init();
}

Game::~Game()
{
	delete window;

	delete camera;
	delete lightingShader;
	delete testShader;
	delete skyBoxShader;
	delete pillarShader;
	delete player;
	delete map;
	delete skyBox;
	delete menu;

	delete physics;
	delete physicsDraw;
}

void Game::Init()
{
	// Load shaders
	lightingShader = new Shader((char*)"../../../resources/shaders/vshader.txt", (char*)"../../../resources/shaders/fshader.txt");
	testShader = new Shader((char*)"../../../resources/shaders/lightvshader.txt", (char*)"../../../resources/shaders/lightfshader.txt");
	skyBoxShader = new Shader((char*)"../../../resources/shaders/skyboxvshader.txt", (char*)"../../../resources/shaders/skyboxfshader.txt");
	pillarShader = new Shader((char*)"../../../resources/shaders/pillarvshader.txt", (char*)"../../../resources/shaders/pillarfshader.txt");

	// Menu
	menu = new MenuManager(this);

	// Player
	player = new Player(physics, lightingShader);

	// Map
	map = new ChunkSpawner(physics, lightingShader, testShader, pillarShader);
	skyBox = new SkyBox(skyBoxShader);

	// Setup default shader
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	lightingShader->use();
	const glm::vec4 Amb = { 1.0f, 1.0f, 1.0f, 1.0f };
	lightingShader->setVec4("Ambient", Amb);
	glClearColor(0.5, 0.5, 0.5, 1.0f);

	// Other
	glEnable(GL_DEPTH_TEST);
	gettimeofday(&t1, &tz);
	InitSeed(t1.tv_usec);

	projection = glm::perspective(glm::radians(camera->zoom), static_cast<float>(winWidth) / static_cast<float>(winHeight), 0.1f, 100.0f);

	ImGuiIO& tempIO = ImGui::GetIO();
	io = &tempIO;
	io->DisplaySize = ImVec2(winWidth, winHeight);

	// ImGui
	ImGui_ImplOpenGL3_NewFrame();
}

void Game::Tick()
{
	while (window->open)
	{
		// Update deltaTime
		gettimeofday(&t2, &tz);
		deltaTime = (t2.tv_sec - t1.tv_sec) * 1000.0f;
		deltaTime += (t2.tv_usec - t1.tv_usec) / 1000.0f;
		deltaTime /= 1000.0f;
		t1 = t2;
		io->DeltaTime = deltaTime;

		// Input
		ProcessInput();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Create transformations
		view = camera->GetViewMatrix();
		projection = glm::perspective(glm::radians(camera->zoom), static_cast<float>(winWidth) / static_cast<float>(winHeight), 0.1f, 100.0f);

		// Update lights
		pillarShader->use();
		pillarShader->setInt("ourTexture", 0);
		pillarShader->setInt("diffuse", 1);

		testShader->use();
		glm::vec3 lightPos = player->GetPosition();
		lightPos.y += 40.0f;
		lightPos.x = -10.0f;
		lightPos.z -= 20.0f;
		testShader->setVec3("pointLights[0].position", lightPos);
		testShader->setVec3("pointLights[0].color", 0.6f, 0.6f, 0.6f);
		lightPos.x = 10.0f;
		testShader->setVec3("pointLights[1].position", lightPos);
		testShader->setVec3("pointLights[1].color", 0.6f, 0.6f, 0.6f);
		testShader->setVec3("viewPos", camera->position);

		if (gameState == GameStates::PLAYING)
		{
			if (movingForward)
			{
				player->Move(Player::PlayerMovement::FORWARD, deltaTime);
				physics->world->stepSimulation(1 / 60.0f, 4);
			}

			// Tile Spawning
			map->UpdateMap(player->GetPosition().z);
			map->Draw(view, projection, deltaTime);

			player->Update(view, projection);
			if (currentAngle != View_Angles::FIRSTPERSON) { player->Draw(); }

			skyBox->Draw(view, projection);

			// Perform collision detection ONLY ON PLAYER using the contact callback
			physics->world->contactTest(player->GetRigidBody(), physics->callback);

			// Collect coin
			const btRigidBody* coinRB = physics->callback.getLastCoinCollisionBody();
			if (coinRB != nullptr) { collectedCoins += map->CollectCoin(coinRB); }

			// Player Death
			if (physics->callback.getObstacleCollision())
			{
				movingForward = false;
				dynamic = false;
				menu->SetMenuState(MenuManager::MenuType::GAMEOVER);
				SetGameState(GameStates::MENU);
			}

			// Dynamic difficulty
			if (dynamic)
			{
				if (currentDifficulty == Difficulty::EASY && (collectedCoins >= 15 || player->GetPosition().z >= 500.0f)) { SetDifficulty(Difficulty::MEDIUM, false, true); }
				else if (currentDifficulty == Difficulty::MEDIUM && (collectedCoins >= 50 || player->GetPosition().z >= 1500.0f)) { SetDifficulty(Difficulty::HARD, false, true); }
				else if (currentDifficulty == Difficulty::HARD && (collectedCoins >= 100 || player->GetPosition().z >= 3000.0f)) { SetDifficulty(Difficulty::IMPOSSIBLE, false, true); }

				if (static_cast<int>(player->GetPosition().z / 100.0f) > lastNameChange)
				{
					menu->GenerateRandomName();
					lastNameChange++;
				}
			}

			if (!freeCam)
			{
				glm::vec3 camOffset = player->GetPosition();

				if (currentAngle == View_Angles::BACK)
				{
					camOffset.z -= 10.0f;
					camOffset.y += 5.0f;
					camera->SetRotation(90.0f, -20.0f);
				}
				else if (currentAngle == View_Angles::FRONT)
				{
					camOffset.z += 10.0f;
					camera->SetRotation(270.0f, 0.0f);
				}
				else if (currentAngle == View_Angles::TOP)
				{
					camOffset.y += 40.0f;
					camera->SetRotation(90.0f, -89.9f);
				}
				else if (currentAngle == View_Angles::FIRSTPERSON)
				{
					camera->SetRotation(90.0f, 0.0f);
				}
				camera->SetPosition(camOffset);
			}

			DrawDebug();
			menu->Draw();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}
		else if (gameState == GameStates::MENU)
		{
			map->Draw(view, projection, deltaTime);

			player->Update(view, projection);
			if (currentAngle != View_Angles::FIRSTPERSON) { player->Draw(); }

			skyBox->Draw(view, projection);

			DrawDebug();
			menu->Draw();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		// Finish frame
		glFlush();
		window->SwapBuffers();

		// Report frame rate
		totalTime += deltaTime;
		frames++;
		if (totalTime >= 1.0f)
		{
			FPS = static_cast<float>(frames) / totalTime;
			appAverage = (totalTime * 1000) / FPS;

			frames = 0;
			totalTime = 0.0f;
		}
	}
}

void Game::DrawDebug() const
{
#ifdef _DEBUG
	if (doDebugDraw)
	{
		physics->world->debugDrawWorld();

		glUseProgram(physicsDraw->ProgramObject);
		GLuint LineError = glGetError();
		if (LineError != GL_NO_ERROR)
		{
			std::cout << "Debug LineDraw Error 1: " << LineError << std::endl;
		}
		else
		{
			glm::mat4 VP = projection * view;

			glUniformMatrix4fv(glGetUniformLocation(physicsDraw->ProgramObject, "Matrix"), 1, GL_FALSE, &VP[0][0]);
			LineError = glGetError();
			if (LineError != GL_NO_ERROR) { std::cout << "Debug LineDraw Error 2: " << LineError << std::endl; }
		}
		LineError = glGetError();
		if (LineError != GL_NO_ERROR) { std::cout << "Debug LineDraw Error 3: " << LineError << std::endl; }
		physicsDraw->setDebugMode(1);
		physicsDraw->DoDebugDraw();
	}
	else
	{
		physicsDraw->setDebugMode(0);
	}
#endif // _DEBUG
}

void Game::DrawDebugInfo()
{
#ifdef _DEBUG
	if (!showDebugInfo) { return; }
	ImGui::Begin("Debug Info");
	ImGui::Text("Window size: %ix%i", winWidth, winHeight);
	ImGui::Text("Application average %1.3f ms/frame (%1.1f FPS)", appAverage, FPS);
	ImGui::End();

	ImGui::Begin("Camera");
	float* camPos[] = { &camera->position.x, &camera->position.y, &camera->position.z };
	ImGui::InputFloat3("Position", *camPos, "%.2f", 0);
	ImGui::SliderFloat("Pitch", &camera->pitch, -89.0f, 89.0f, "%.1f", 0);
	ImGui::SliderFloat("Yaw", &camera->yaw, 0.0f, 359.9f, "%.1f", 0);
	ImGui::SliderFloat("Zoom", &camera->zoom, 0.1f, 100.0f, "%.1f", 0);
	ImGui::End();

	ImGui::Begin("Player");
	float playerPos[] = { player->GetPosition().x, player->GetPosition().y, player->GetPosition().z };
	ImGui::InputFloat3("Position", playerPos, "%.2f", 0);

	int speed = playerSpeed;
	ImGui::SliderInt("Difficulty", &speed, 0, 4);
	if (speed != playerSpeed)
	{
		switch (speed)
		{
		case 0: movingForward = false; playerSpeed = 0; break;
		case 1: SetDifficulty(EASY); break;
		case 2: SetDifficulty(MEDIUM); break;
		case 3: SetDifficulty(HARD); break;
		case 4: SetDifficulty(IMPOSSIBLE); break;
		default:
			break;
		}
	}
	ImGui::Text("forwardSpeed: %.2f", player->GetForwardSpeed());
	ImGui::End();

	ImGui::Begin("Collected Coins");
	ImGui::InputInt("Total", &collectedCoins);
	ImGui::End();
	camera->ProcessMouseMovement(0.0f, 0.0f);
#endif // _DEBUG
}

void Game::SetGameState(const GameStates state) { gameState = state; }

void Game::SetDifficulty(const Difficulty difficulty, const bool reset, const bool dynamicIn)
{
	dynamic = dynamicIn;
	if (dynamic) { menu->GenerateRandomName(); }
	else { menu->ResetRandomName(); }

	if (reset)
	{
		player->Reset();
		player->Update(view, projection);
		map->Reset();

		collectedCoins = 0;
		movingForward = true;
	}

	switch (difficulty)
	{
	case Difficulty::EASY:
		player->SetSpeed(Player::PlayerSpeed::SLOW);
		map->SetObstacleFrequency(1);
		playerSpeed = 1;
#ifdef _DEBUG
		std::cout << "SELECTED EASY DIFFICULTY" << std::endl;
#endif // _DEBUG
		break;
	case Difficulty::MEDIUM:
		player->SetSpeed(Player::PlayerSpeed::NORMAL);
		map->SetObstacleFrequency(2);
		playerSpeed = 2;
#ifdef _DEBUG
		std::cout << "SELECTED NORMAL DIFFICULTY" << std::endl;
#endif // _DEBUG
		break;
	case Difficulty::HARD:
		player->SetSpeed(Player::PlayerSpeed::FAST);
		map->SetObstacleFrequency(3);
		playerSpeed = 3;
#ifdef _DEBUG
		std::cout << "SELECTED HARD DIFFICULTY" << std::endl;
#endif // _DEBUG
		break;
	case Difficulty::IMPOSSIBLE:
		player->SetSpeed(Player::PlayerSpeed::HYPERSPEED);
		map->SetObstacleFrequency(4);
		playerSpeed = 4;
#ifdef _DEBUG
		std::cout << "SELECTED IMPOSSIBLE DIFFICULTY" << std::endl;
#endif // _DEBUG
		break;
	default:
		break;
	}

	currentDifficulty = difficulty;
	movingForward = true;
}

float Game::GetDistance() const{ return player->GetPosition().z; }

int Game::GetCoins() const { return collectedCoins; }

unsigned int Game::LoadTexture(const char* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format = 0;
		if (nrComponents == 1) { format = GL_RED; }
		else if (nrComponents == 3) { format = GL_RGB; }
		else if (nrComponents == 4) { format = GL_RGBA; }

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		std::cout << "Because: " << stbi_failure_reason() << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

void Game::ProcessInput()
{
	if (menu->GetQuit() == true) { window->open = false; } // Check for quit button in menu

	// Window close event
	Atom deleteWindow = XInternAtom(window->xDisplay, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(window->xDisplay, window->xWin, &deleteWindow, 1);


	while (XPending(window->xDisplay))
	{
		XEvent event;
		char* str = nullptr;
		KeySym keySym = 0;

		int mouseX, mouseY;

		XNextEvent(window->xDisplay, &event);
		switch (event.type)
		{
		case ClientMessage:
			if (event.xclient.data.l[0] == static_cast<long int>(deleteWindow))
			{
				std::cout << "Closing program." << std::endl;
				window->open = false;
			}
			break;
		case KeyPress:
			XLookupString(&event.xkey, str, 25, &keySym, nullptr);
			switch (keySym)
			{
#ifdef _DEBUG
			case XK_Escape:
				window->open = false;
				std::cout << "Escape pressed. Closing program." << std::endl;
				break;
			case XK_q:
				if (freeCam) { camera->ProcessKeyboard(DOWN, deltaTime); }
				break;
			case XK_e:
				if (freeCam) { camera->ProcessKeyboard(UP, deltaTime); }
				break;
			case XK_p:
				if (showDebugInfo) { showDebugInfo = false; }
				else { showDebugInfo = true; }
				break;
			case XK_t:
				if (lockCursor)
				{
					// Make cursor visible
					XUndefineCursor(window->xDisplay, window->xWin);

					// Unlock cursor (for menu navigation)
					lockCursor = false;
				}
				else
				{
					// Make cursor invisible - https://stackoverflow.com/questions/660613/how-do-you-hide-the-mouse-pointer-under-linux-x11
					XColor black;
					static char noData[] = { 0,0,0,0,0,0,0,0 };
					black.red = black.green = black.blue = 0;

					const Pixmap bitmapNoData = XCreateBitmapFromData(window->xDisplay, window->xWin, noData, 8, 8);
					const Cursor invisibleCursor = XCreatePixmapCursor(window->xDisplay, bitmapNoData, bitmapNoData, &black, &black, 0, 0);
					XDefineCursor(window->xDisplay, window->xWin, invisibleCursor);
					XFreeCursor(window->xDisplay, invisibleCursor);
					XFreePixmap(window->xDisplay, bitmapNoData);

					// Lock cursor (for movement)
					firstMouse = true;
					lockCursor = true;
				}
				break;
			case XK_r:
				if (doDebugDraw) { doDebugDraw = false; }
				else { doDebugDraw = true; }
				break;
			case XK_f:
				if (freeCam) { freeCam = false; }
				else { freeCam = true; }
				break;
			case XK_w:
				if (freeCam) { camera->ProcessKeyboard(FORWARD, deltaTime); }
				else if (!movingForward) { player->Move(Player::FORWARD); }
				break;
			case XK_s:
				if (freeCam) { camera->ProcessKeyboard(BACKWARD, deltaTime); }
				else if (!movingForward) { player->Move(Player::BACKWARD); }
				break;
#endif // _DEBUG
			case XK_Left:
			case XK_a:
				if (freeCam) { camera->ProcessKeyboard(LEFT, deltaTime); }
				else if (movingForward) { player->Move(Player::LEFT); }
				break;
			case XK_Right:
			case XK_d:
				if (freeCam) { camera->ProcessKeyboard(RIGHT, deltaTime); }
				else if (movingForward) { player->Move(Player::RIGHT); }
				break;
			case XK_space:
				if (!freeCam && movingForward) { player->Move(Player::JUMP, deltaTime); }
				break;
			case XK_z:
			{
				if (currentAngle == View_Angles::BACK) { currentAngle = View_Angles::FRONT; }
				else if (currentAngle == View_Angles::FRONT) { currentAngle = View_Angles::TOP; camera->SetRotation(90.0f, -89.9f); }
				else if (currentAngle == View_Angles::TOP) { currentAngle = View_Angles::FIRSTPERSON; }
				else { currentAngle = View_Angles::BACK; }
			}
			default:
				break;
			}
			break;
		case ButtonPress:
			switch (event.xbutton.button)
			{
			case 1:
				io->MouseDown[0] = true;
				break;
			case 3:
				io->MouseDown[1] = true;
				break;
			default:
				break;
			}
			break;
		case ButtonRelease:
			switch (event.xbutton.button)
			{
			case 1:
				io->MouseDown[0] = false;
				break;
			case 3:
				io->MouseDown[1] = false;
				break;
			default:
				break;
			}
			break;
		case MotionNotify:
			mouseX = event.xmotion.x;
			mouseY = event.xmotion.y;
			if (freeCam && lockCursor) { CalcMouseOffset(mouseX, mouseY); }
			else { io->MousePos = ImVec2(static_cast<float>(mouseX), static_cast<float>(mouseY)); }
			break;
		default:
			break;
		}
	}
}

void Game::CalcMouseOffset(int x, int y)
{
	int midw, midh;
	window->GetSize(midw, midh);
	midw = midw / 2;
	midh = midh / 2;

	if (x == midw && y == midh) { return; }

	float xpos = static_cast<float>(x);
	float ypos = static_cast<float>(y);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	XWarpPointer(window->xDisplay, None, window->xWin, 0, 0, 0, 0, midw, midh);
	camera->ProcessMouseMovement(xoffset, yoffset);

	lastX = static_cast<float>(midw);
	lastY = static_cast<float>(midh);
}
