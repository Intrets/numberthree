#include "renderer/GeneralShadowMapRenderer.h"

void render::GeneralShadowMapRenderer::render(
	ogs::Configuration const& config,
	std::vector<glm::mat4> const& transforms_,
	glm::mat4 VP_,
	bwo::FrameBuffer& target,
	glm::ivec4 viewport) {

	if (transforms_.empty()) {
		return;
	}

	Global<ogs::State>->setState(config);

	this->VAO.bind();
	this->program.use();

	this->transforms.set(transforms_);
	this->VP.set(VP_);

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
