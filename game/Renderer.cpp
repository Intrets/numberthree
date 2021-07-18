#include "Renderer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <misc/Timer.h>
#include <misc/Option.h>

#include <mem/Global.h>

#include <render/infos/DebugRenderInfo.h>
#include <render/BlitRendererArray.h>
#include <render/BlitRenderer.h>
#include <render/textures/BlockIDTextures.h>
#include <render/GLStateWrapper.h>
#include <render/BufferWrappers.h>

void Renderer::render(GLFWwindow* window, RenderInfo const& renderInfo) {
	render::bwo::FrameBuffer target{ window };

	render::bwo::Texture2D d{ render::bwo::Texture2DHelper::makeDepthBuffer(renderInfo.frameSize) };
	render::bwo::Texture2D albedo{ render::bwo::Texture2DHelper::makeLinearFiltering(renderInfo.frameSize) };
	render::bwo::Texture2D shadow{ render::bwo::Texture2DHelper::makeFloatBuffer(renderInfo.frameSize, true) };

	static render::bwo::FrameBuffer target2;

	Global<misc::Timer>->newTiming("Render");

	target.clear({ 0.5f, 0.5f, 0.5f, 1.0f }, true);

	glm::ivec4 viewport{ 0, 0, renderInfo.cameraInfo.x, renderInfo.cameraInfo.y };

	std::vector<glm::mat4> transforms;

	depthTarget.clearDepth();

	float lightFar = 230.0f;
	glm::vec3 lightPos{ 100.0f, 30.0f, 10.0f };
	auto lightVP =
		glm::perspective(glm::radians(30.0f), 1.0f, 0.1f, lightFar) *
		glm::lookAt(lightPos, { 0.0f, 30.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });

	for (size_t i = 0; i < static_cast<size_t>(ModelEnum::MAX); i++) {
		this->shadowMapRenderers[i].render(
			ogs::ShadowMapConfiguration(),
			renderInfo.modelRenderInfo[i],
			lightVP,
			depthTarget,
			{ 0, 0, depthBuffer.size.x, depthBuffer.size.y }
		);
	}

	target2.bindDepthLayer(d);
	target2.bindTextureColor(0, albedo, 0);
	target2.bindTextureColor(1, shadow, 0);
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glBindFramebuffer(GL_FRAMEBUFFER, target2.ID);
	glDrawBuffers(2, attachments);
	assert(36053 == glCheckFramebufferStatus(GL_FRAMEBUFFER));
	target2.clear({ 0.0,0.0,0.0,1.0 }, true);
	target2.clearDepth();

	for (size_t i = 0; i < static_cast<size_t>(ModelEnum::MAX); i++) {
		this->modelRenderers[i].render(
			ogs::GeneralConfiguration(),
			renderInfo.modelRenderInfo[i],
			lightPos,
			renderInfo.cameraInfo.camPos,
			depthBuffer,
			renderInfo.cameraInfo.VP(),
			lightVP,
			lightFar,
			renderInfo.cameraInfo.camPos,
			target2,
			viewport
		);
	}

	target.clearDepth();

	this->assembleRenderer.render(
		ogs::BlitConfiguration(),
		target,
		viewport,
		albedo,
		shadow
	);

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
