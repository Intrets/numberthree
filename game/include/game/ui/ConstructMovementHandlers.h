#pragma once

namespace ui
{
	class State;
	class Base;
}

template<class, class>
class UniqueReference;

namespace game
{
	UniqueReference<ui::Base, ui::Base> constructMovementHandlers();
}
