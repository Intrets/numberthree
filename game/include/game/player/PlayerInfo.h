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
	ui::ControlState& controlState;
	ui::State& uiState;
};
