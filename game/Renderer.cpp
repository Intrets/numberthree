#include "Renderer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <misc/Timer.h>
#include <misc/Option.h>

#include <mem/Global.h>

#include <render/infos/DebugRenderInfo.h>
#include <render/BlitRendererArray.h>
#include <render/textures/BlockIDTextures.h>
#include <render/GLStateWrapper.h>

void Renderer::render(GLFWwindow* window, RenderInfo const& renderInfo) {
	render::bwo::FrameBuffer target{ window };

	Global<misc::Timer>->newTiming("Render");

	target.clear({ 0.5f, 0.5f, 0.5f, 1.0f }, true);

	glm::ivec4 viewport{ 0,0,renderInfo.cameraInfo.x, renderInfo.cameraInfo.y };

	std::vector<glm::mat4> transforms;

	//constexpr int N = 10;
	//constexpr int M = 10;
	//constexpr int O = 1;
	//for (int32_t i = -N; i < N; i++) {
	//	for (int32_t j = -M; j < M; j++) {
	//		for (int32_t k = -O; k < O; k++) {
	//			glm::vec3 pos{ i,j,k };
	//			pos *= 0.3f;
	//			transforms.push_back(
	//				glm::translate(pos) *
	//				glm::scale(glm::vec3(0.05f))
	//			);
	//		}
	//	}
	//}

	for (size_t i = 0; i < static_cast<size_t>(ModelEnum::MAX); i++) {
		this->modelRenderers[i].render(
			ogs::GeneralConfiguration(),
			renderInfo.modelRenderInfo[i],
			renderInfo.cameraInfo.VP(),
			renderInfo.cameraInfo.camPos,
			target,
			viewport
		);
	}

	//this->suzanneRenderer.render(
	//	ogs::GeneralConfiguration(),
	//	transforms,
	//	VP,
	//	renderInfo.cameraInfo.camPos,
	//	target,
	//	viewport
	//);

	//this->ground.render(
	//	ogs::GeneralConfiguration(),
	//	transforms,
	//	VP,
	//	renderInfo.cameraInfo.camPos,
	//	target,
	//	viewport
	//);

	target.clearDepth();

	this->uiBackgroundRenderer.render(
		renderInfo.uiRenderInfo,
		0,
		renderInfo.cameraInfo
	);

	this->textRenderer.render(
		renderInfo.textRenderInfo,
		*Global<render::Fonts>,
		target
	);

	if (misc::Option<misc::OPTION::GR_DEBUG, bool>::getVal()) {
		this->debugRenderer.render(
			*Global<render::DebugRenderInfo>,
			target,
			viewport,
			renderInfo.cameraInfo
		);

		Global<render::DebugRenderInfo>->clear();
	}
	Global<misc::Timer>->endTiming("Render");

	Global<misc::Timer>->newTiming("Swap Buffers");
	glfwSwapBuffers(window);
	Global<misc::Timer>->endTiming("Swap Buffers");
}
