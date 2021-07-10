#pragma once

#include <wglm/glm.hpp>
#include <wglm/gtc/matrix_transform.hpp>
#include <wglm/gtx/euler_angles.hpp>

namespace game
{
	struct GameState;
}

namespace ui
{
	class State;
	class ControlState;
}

inline static int test{};
struct PlayerInfo
{
	glm::vec3 pos = { 0.0f, 0.0f, 10.0f };
	glm::vec3 look = { 0, 0, 0 };

	game::GameState& gameState;
	ui::ControlState& controlState;
	ui::State& uiState;

	PlayerInfo(game::GameState& gameState_, ui::ControlState& controlState_, ui::State& uiState_) :
		gameState(gameState_), controlState(controlState_), uiState(uiState_) {
	}
};
