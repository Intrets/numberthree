#pragma once

namespace game
{
	struct GameState
	{
		int32_t tick{};
		void runTick() { this->tick++; };
	};
}