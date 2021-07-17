#include "renderer/AssembleRenderer.h"

#include <mem/Global.h>

void render::AssembleRenderer::render(
	ogs::Configuration const& config,
	bwo::FrameBuffer& target,
	glm::ivec4 viewport,
	bwo::Texture2D const& albedo,
	bwo::Texture2D const& lighting) {

	Global<ogs::State>->setState(config);

	this->VAO.bind();
	this->program.use();

	this->albedo_t.set(albedo);
	this->lighting_t.set(lighting);

	target.draw(
		viewport,
		[&] {
			glDrawArrays(
				GL_TRIANGLES,
				0, 6
			);
		}
	);
}

render::AssembleRenderer::AssembleRenderer() {
	static const GLfloat g_quad_vertex_buffer_data[] = {
		0.0f,  0.0f,
		1.0f,  0.0f,
		0.0f,  1.0f,
		0.0f,  1.0f,
		1.0f,  0.0f,
		1.0f,  1.0f,
	};

	this->quad.setRaw(sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data);
}
