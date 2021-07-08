#include "renderer/GeneralRenderer.h"

void render::GeneralRenderer::render(
	ogs::Configuration const& config,
	std::vector<glm::mat4> const& transforms_,
	bwo::FrameBuffer& target,
	glm::ivec4 viewport) {

	Global<ogs::State>->setState(config);

	this->VAO.bind();
	this->program.use();

	this->transforms.set(transforms_);

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
