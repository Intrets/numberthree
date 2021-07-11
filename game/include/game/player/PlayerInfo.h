#pragma once

#include <wglm/glm.hpp>
#include <wglm/gtc/matrix_transform.hpp>
#include <wglm/gtx/euler_angles.hpp>

#include <mem/Everything.h>

namespace game
{
	struct GameState;
}

namespace ui
{
	class State;
	class ControlState;
}

struct PlayerInfo
{
	glm::vec3 look = { 0, 0, 0 };

	QualifiedObject player;

	game::GameState& gameState;
	ui::ControlState& controlState;
	ui::State& uiState;

	PlayerInfo(game::GameState& gameState_, ui::ControlState& controlState_, ui::State& uiState_, WeakObject player_) :
		gameState(gameState_), controlState(controlState_), uiState(uiState_), player(player_) {
	}
};
