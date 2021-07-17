#pragma once

#include <vector>

#include <render/BufferWrappers.h>
#include <render/GLStateWrapper.h>

#include "../shaders/General.vert.inc"
#include "../shaders/General.frag.inc"

#include <wglm/gtx/transform.hpp>

namespace render
{
	struct GeneralRenderer
	{
		using IndexType = uint16_t;
		static constexpr GLenum GLIndexType = GL_UNSIGNED_SHORT;

		//bwo::Program program{ General_vert, General_frag, "GeneralRenderer" };
		bwo::Program program{ "General.vert", "General.frag", "GeneralRenderer", 0 };

		bwo::ArrayBuffer<glm::vec3> model;
		bwo::ArrayBuffer<glm::vec2> uv;
		bwo::ArrayBuffer<glm::vec3> normals;
		bwo::ArrayBuffer<IndexType> indices;
		int32_t indexSize;

		bwo::ArrayBuffer<glm::mat4> transforms{ bwo::BufferHint::STREAM_DRAW };

		bwo::VertexArrayObject <
			// model vertices
			bwo::Group<glm::vec3, 0,
			bwo::VInfo<float, 3>>,
			//model uv
			bwo::Group<glm::vec2, 0,
			bwo::VInfo<float, 2>>,
			// model normals
			bwo::Group<glm::vec3, 0,
			bwo::VInfo<float, 3>>,
			// indices
			bwo::Indices,

			bwo::Group<glm::mat4, 1,
			bwo::VInfo<float, 4>,
			bwo::VInfo<float, 4>,
			bwo::VInfo<float, 4>,
			bwo::VInfo<float, 4>>
			> VAO;

		bwo::UniformTexture2D texture_t{ "texture_t", program, 0 };
		bwo::UniformTexture2D shadowMap_t{ "shadowMap_t", program, 1 };
		bwo::UniformMatrix4fv VP{ "VP", program };
		bwo::UniformMatrix4fv lightVP{ "lightVP", program };
		bwo::Uniform3fv lightPos{ "lightPos", program };
		bwo::Uniform3fv viewPos{ "viewPos", program };
		bwo::Uniform1f lightFar{ "lightFar", program };

		GeneralRenderer() = default;
		GeneralRenderer(
			bwo::Model model_
		) :
			model(std::move(model_.model)),
			uv(std::move(model_.uv)),
			normals(std::move(model_.normals)),
			indices(std::move(model_.indices)),
			indexSize(model_.indexSize),
			VAO(model, uv, normals, indices, transforms) {
		}

		DEFAULT_MOVE(GeneralRenderer);
		NO_COPY(GeneralRenderer);

		~GeneralRenderer() = default;

		void render(
			ogs::Configuration const& config,
			std::vector<glm::mat4> const& transforms,
			glm::vec3 lightPos,
			glm::vec3 viewPos,
			bwo::Texture2D const& shadowMap,
			glm::mat4 VP,
			glm::mat4 lightVP,
			float lightFar,
			glm::vec3 camPos,
			bwo::FrameBuffer& target,
			glm::ivec4 viewport
		);
	};
}