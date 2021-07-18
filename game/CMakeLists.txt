cmake_minimum_required(VERSION 3.20)

set(MODULE_NAME game)

project(${MODULE_NAME} LANGUAGES CXX)

make_module(
	MODULE_NAME ${MODULE_NAME}
	MODULE_FILES
		player/PlayerInfo
		GameState
		ui/ConstructDebugUI
		ui/ConstructMovementHandlers
		ui/ConstructSpeedometer
		renderer/GeneralRenderer
		renderer/GeneralShadowMapRenderer
		renderer/AssembleRenderer
		Renderer
		Game
	CXX_STANDARD 20
	REQUIRED_LIBS
		WGLM
		mem
		ui
		render
		misc
		PhysX
)
set(SHADER_FILES
	GeneralShadowMap
)

foreach(SHADER ${SHADER_FILES})
	set(FULL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/shaders/${SHADER}")

	file(READ "${FULL_PATH}.frag" FRAGMENT_SHADER)
	file(WRITE "${FULL_PATH}.frag.inc" "static const char* ${SHADER}_frag = R\"(${FRAGMENT_SHADER})\";\n")

	file(READ "${FULL_PATH}.vert" VERTEX_SHADER)
	file(WRITE "${FULL_PATH}.vert.inc" "static const char* ${SHADER}_vert = R\"(${VERTEX_SHADER})\";\n")
endforeach()

target_include_directories(${MODULE_NAME} PRIVATE "./shaders")
