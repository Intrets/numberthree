cmake_minimum_required(VERSION 3.21)

add_executable(${PROJECT_NAME}
	main.cpp
	Setup.h
	Setup.cpp
	MainLoop.h
	MainLoop.cpp
)

option(USE_PRECOMPILED_HEADERS "enable/disable precompiled headers" ON)

if(USE_PRECOMPILED_HEADERS)
	target_precompile_headers(game PUBLIC pch.h)
	target_precompile_headers(render PUBLIC pch.h)
	target_precompile_headers(ui PUBLIC pch.h)
	target_precompile_headers(misc PUBLIC pch.h)
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC
	opengl32
	GLFW3
	GLEW
	ui
	mem
	game
	misc
	render
	WGLM
	SDL2::SDL2-static
	SDL_Mixer
	sound
	PhysX
)

add_custom_command(
	TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>" "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
	COMMAND_EXPAND_LISTS
)
