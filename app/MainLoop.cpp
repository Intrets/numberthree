#include "MainLoop.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <game/ui/ConstructDebugUI.h>
#include <game/GameState.h>
#include <game/player/PlayerInfo.h>
#include <game/Game.h>

#include <ui/State.h>
#include <ui/ControlState.h>
#include <ui/Constructer.h>

#include <misc/Option.h>
#include <misc/Timer.h>
#include <misc/Log.h>
#include <misc/PathManager.h>

#include <render/infos/proxy/UIInfos.h>

#include <fstream>
#include <format>
#include <sstream>

#include <SDL_mixer.h>

ui::ControlState controlState;

static void key_callback(GLFWwindow* w, int32_t key, int32_t scancode, int32_t action, int32_t mods) {
	controlState.key_callback(w, key, scancode, action, mods);
}

static void mouse_callback(GLFWwindow* w, int32_t key, int32_t action, int32_t mods) {
	key_callback(w, key + GLFW_KEY_LAST, 0, action, mods);
}

void char_callback(GLFWwindow* w, unsigned int character) {
	controlState.char_callback(w, character);
}

void scroll_callback(GLFWwindow* w, double xoffset, double yoffset) {
	controlState.scroll_callback(w, xoffset, yoffset);
}

void prepareRender(
	GLFWwindow* window,
	RenderInfo& renderInfo,
	PlayerInfo& playerInfo) {

	auto& gameState = playerInfo.gameState;
	auto& uiState = playerInfo.uiState;

	glm::vec3 p{};
	if (playerInfo.player.isQualified()) {
		p = playerInfo.player->get<game::Transform>().pos;
	}

	int32_t frameSizeX, frameSizeY;
	glfwGetFramebufferSize(window, &frameSizeX, &frameSizeY);
	float ratio = frameSizeX / static_cast<float>(frameSizeY);
	glm::vec2 viewport(ratio, 1.0f);
	viewport *= misc::Option<misc::OPTION::CL_VIEWPORTSCALE, float>::getVal();
	renderInfo.frameSize = { frameSizeX, frameSizeY };

	renderInfo.cameraInfo = {
		.x = frameSizeX, .y = frameSizeY,
		.camPos = p + glm::vec3(0.0f, 0.0f, 1.8f),
		.rotation = glm::yawPitchRoll(-playerInfo.look.x, -playerInfo.look.y, -playerInfo.look.z),
		.P = glm::perspective(glm::radians(90.0f), ratio, 0.1f, 1000.0f),
		.viewPort = glm::vec3(viewport, 200.0f) };

	Global<misc::Timer>->newTiming("gamestate");
	gameState.addRenderInfo(renderInfo);
	Global<misc::Timer>->endTiming("gamestate");

	Global<misc::Timer>->newTiming("Prepare UI");
	render::UIInfos uiInfos{ renderInfo.uiRenderInfo, renderInfo.textRenderInfo };
	uiState.appendRenderInfo(gameState.tick, uiInfos);
	Global<misc::Timer>->endTiming("Prepare UI");
}

void mainLoop(GLFWwindow* window, std::chrono::steady_clock::time_point startTime) {
	ui::State uiState;
	game::GameState gameState;
	gameState.init();

	{
		ui::Global::push();
		ui::window("Debug Info",
			{ 0.0f, 0.0f, 0.13f, 1.0f },
			ui::WINDOW::TYPE::MINIMISE |
			ui::WINDOW::TYPE::RESIZEVERTICAL |
			ui::WINDOW::TYPE::RESIZEHORIZONTAL |
			ui::WINDOW::TYPE::RESIZE |
			ui::WINDOW::TYPE::MOVE);
		game::constructDebugUI();

		uiState.UIs.push_back(ui::Global::pop());
	}

	Renderer renderer;

	PlayerInfo playerInfo{ gameState, controlState, uiState, gameState.player.getObject() };

	glfwSetCharCallback(window, char_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	std::chrono::duration<double> startUpDuration = std::chrono::steady_clock::now() - startTime;
	Global<misc::Log>->putLine(std::format("Startup time: {} seconds\n", startUpDuration));

	for (;;) {
		if (uiState.loadGame.has_value()) {
			assert(0);

			std::ifstream file;
			Global<misc::PathManager>->openSave(file, uiState.loadGame.value());
			if (!file.good()) {
				Global<misc::Log>->putLine(std::format("failed to open save file: {}", uiState.loadGame.value()));
			}
			else {
				assert(0);
			}

			uiState.loadGame.reset();
			file.close();
		}

		if (uiState.saveGame.has_value()) {
			assert(0);
			std::ofstream file;
			Global<misc::PathManager>->openSave(file, uiState.saveGame.value());
			if (!file.good()) {
				Global<misc::Log>->putLine(std::format("failed to open save file: {}", uiState.saveGame.value()));
			}
			else {
				assert(0);
			}

			uiState.saveGame.reset();
			file.close();
		}


		Global<misc::Timer>->endTiming("fps");
		Global<misc::Timer>->newTiming("fps");

		Global<misc::Timer>->newTiming("game logic");
		gameState.runTick();
		Global<misc::Timer>->endTiming("game logic");

		uiState.updateSize(window);

		RenderInfo renderInfo;
		Global<misc::Timer>->newTiming("prep render");
		prepareRender(window, renderInfo, playerInfo);
		Global<misc::Timer>->endTiming("prep render");

		controlState.cycleStates();
		glfwPollEvents();

		uiState.updateCursor(window, { glm::vec2() });

		uiState.run(playerInfo);

		if (glfwWindowShouldClose(window)) {
			break;
		}

		renderer.render(window, renderInfo);
	}
}
