#pragma once

#include <wglm/glm.hpp>

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
	glm::vec3 pos;

	game::GameState& gameState;
	ui::ControlState& controlState;
	ui::State& uiState;
};
