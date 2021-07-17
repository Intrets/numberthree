#pragma once

#include <render/BufferWrappers.h>
#include <render/GLStateWrapper.h>

namespace render
{
	struct AssembleRenderer
	{
		bwo::Program program{ "Assemble.vert", "Assemble.frag", "AssembleRenderer", 0 };

		bwo::ArrayBuffer<glm::vec2> quad{ bwo::BufferHint::STATIC_DRAW };

		bwo::VertexArrayObject<
			bwo::Group<glm::vec2, 0,
			bwo::VInfo<float, 2>>
			> VAO{ quad };

		bwo::UniformTexture2D albedo_t{ "albedo_t", program, 0 };
		bwo::UniformTexture2D lighting_t{ "lighting_t", program, 1 };

		void render(
			ogs::Configuration const& config,
			bwo::FrameBuffer& target,
			glm::ivec4 viewport,
			bwo::Texture2D const& albedo,
			bwo::Texture2D const& lighting
		);

		AssembleRenderer();
		~AssembleRenderer() = default;
	};
}