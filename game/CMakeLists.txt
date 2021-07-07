cmake_minimum_required(VERSION 3.20)

set(MODULE_NAME game)

project(${MODULE_NAME} LANGUAGES CXX)

make_module(
	MODULE_NAME ${MODULE_NAME}
	MODULE_FILES
		player/PlayerInfo
		GameState
		ui/ConstructDebugUI
	CXX_STANDARD 20
	REQUIRED_LIBS
		WGLM
		mem
		ui
		render
		misc
)
