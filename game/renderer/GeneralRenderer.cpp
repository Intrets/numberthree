#include "renderer/GeneralRenderer.h"

#include <mem/Global.h>

void render::GeneralRenderer::render(
	ogs::Configuration const& config,
	std::vector<glm::mat4> const& transforms_,
	glm::mat4 VP_,
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
	this->camPos.set(glm::vec3(sin(test / 30.0f), cos(test / 41.0f), sin(test / 50.0f) * cos(test / 70.0f)) * 50.0f);
	this->time.set(test * 3.0f);
	//this->time.set(0.0f);

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
