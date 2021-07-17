#include "renderer/GeneralRenderer.h"

#include <render/loaders/TextureLoader.h>

#include <mem/Global.h>

#include <misc/PathManager.h>

void render::GeneralRenderer::render(
	ogs::Configuration const& config,
	std::vector<glm::mat4> const& transforms_,
	glm::vec3 lightPos_,
	glm::vec3 viewPos_,
	bwo::Texture2D const& shadowMap,
	glm::mat4 VP_,
	glm::mat4 lightVP_,
	float lightFar_,
	glm::vec3 camPos_,
	bwo::FrameBuffer& target,
	glm::ivec4 viewport) {

	if (transforms_.empty()) {
		return;
	}

	static float test = 0.0f;

	test += 0.2f;

	Global<ogs::State>->setState(config);

	this->VAO.bind();
	this->program.use();

	this->transforms.set(transforms_);
	this->VP.set(VP_);
	this->lightVP.set(lightVP_);
	this->lightPos.set(lightPos_);
	this->viewPos.set(viewPos_);
	this->lightFar.set(lightFar_);

	static bwo::Texture2D tex = load2DTexture(Global<misc::PathManager>->getTexturesPath() / "dev.dds");

	this->texture_t.set(tex);
	this->shadowMap_t.set(shadowMap);

	target.draw(
		viewport,
		[&] {
			glDrawElementsInstanced(
				GL_TRIANGLES,
				this->indexSize,
				this->GLIndexType,
				(void*)0,
				static_cast<GLsizei>(transforms_.size())
			);
		}
	);
}
