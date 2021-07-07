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
