#pragma once

#include <render/BufferWrappers.h>
#include <render/GLStateWrapper.h>

#include "../shaders/GeneralShadowMap.vert.inc"
#include "../shaders/GeneralShadowMap.frag.inc"

namespace render
{
	struct GeneralShadowMapRenderer
	{
		using IndexType = uint16_t;
		static constexpr GLenum GLIndexType = GL_UNSIGNED_SHORT;

		bwo::Program program{ GeneralShadowMap_vert, GeneralShadowMap_frag, "GeneralShadowMapRenderer" };

		bwo::ArrayBuffer<glm::vec3> model;
		bwo::ArrayBuffer<IndexType> indices;
		int32_t indexSize;

		bwo::ArrayBuffer<glm::mat4> transforms{ bwo::BufferHint::STREAM_DRAW };

		bwo::VertexArrayObject<
			bwo::Group<glm::vec3, 0,
			bwo::VInfo<float, 3>>,

			bwo::Indices,

			bwo::Group<glm::mat4, 1,
			bwo::VInfo<float, 4>,
			bwo::VInfo<float, 4>,
			bwo::VInfo<float, 4>,
			bwo::VInfo<float, 4>>
			> VAO;

		bwo::UniformMatrix4fv VP{ "VP", program };

		GeneralShadowMapRenderer() = default;
		GeneralShadowMapRenderer(bwo::Model model_) :
			model(std::move(model_.model)),
			indices(std::move(model_.indices)),
			indexSize(model_.indexSize),
			VAO(model, indices, transforms) {
		}

		DEFAULT_MOVE(GeneralShadowMapRenderer);
		NO_COPY(GeneralShadowMapRenderer);

		~GeneralShadowMapRenderer() = default;

		void render(
			ogs::Configuration const& config,
			std::vector<glm::mat4> const& transforms,
			glm::mat4 VP,
			bwo::FrameBuffer& target,
			glm::ivec4 viewport
		);
	};
}