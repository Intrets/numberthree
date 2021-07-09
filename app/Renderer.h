#pragma once

#include <render/TextRenderer.h>
#include <render/UIBackgroundRenderer.h>
#include <render/DebugRenderer.h>
#include <render/HighlightRenderer.h>
#include <render/infos/CameraInfo.h>
#include <render/infos/TextRenderInfo.h>
#include <render/infos/BlitArrayRenderInfo.h>
#include <render/loaders/ModelLoader.h>

#include <game/renderer/GeneralRenderer.h>

#include <misc/PathManager.h>
#include <mem/Global.h>

struct RenderInfo
{
	glm::ivec2 frameSize;
	render::CameraInfo cameraInfo;
	render::TextRenderInfo textRenderInfo;
	render::UIRenderInfo uiRenderInfo;
	render::BlitArrayRenderInfo tileRenderInfo;
	render::HighlightRenderInfo highlightRenderInfo;
};

struct Renderer
{
public:
	render::TextRenderer textRenderer{};
	render::UIBackgroundRenderer uiBackgroundRenderer{};
	render::DebugRenderer debugRenderer{};
	render::HighlightRenderer highlightRenderer{};

	std::filesystem::path suzannePath = Global<misc::PathManager>->getModelsPath() / "Suzanne.obj";
	std::filesystem::path groundPath = Global<misc::PathManager>->getModelsPath() / "Cube.obj";
	render::GeneralRenderer suzanneRenderer;
	render::GeneralRenderer ground;

	Renderer() :
		suzanneRenderer(render::loadModel(suzannePath.string())),
		ground(render::loadModel(groundPath.string())) {

			{
				std::vector<glm::mat4> transforms_;

				//constexpr int N = 20;
				//constexpr int M = 20;
				//constexpr int O = 1;
				//for (int32_t i = -N; i < N; i++) {
				//	for (int32_t j = -M; j < M; j++) {
				//		for (int32_t k = -O; k < O; k++) {
				//			glm::vec3 pos{ i,j,k };
				//			pos *= 0.3f;
				//			transforms_.push_back(
				//				glm::translate(pos) *
				//				glm::scale(glm::vec3(0.5f))
				//			);
				//		}
				//	}
				//}

				for (size_t i = 0; i < 50; i++) {
					auto get = [] { return rand() % 200; };
					glm::vec3 pos{ get(), get(), get() };
					transforms_.push_back(
						glm::translate(pos) *
						glm::scale(glm::vec3(5.0f))
					);
				}

				this->suzanneRenderer.size = static_cast<int32_t>(transforms_.size());

				this->suzanneRenderer.transforms.set(transforms_);
			}

			{
				std::vector<glm::mat4> transforms_;

				transforms_.push_back(
					glm::mat4()
				);

				transforms_.push_back(
					glm::translate(glm::vec3(0, 0, 0)) *
					glm::scale(glm::vec3(10000.0f, 10000.0f, 1.0f))
				);

				this->ground.size = static_cast<int32_t>(transforms_.size());
				this->ground.transforms.set(transforms_);
			}

	};


	void render(GLFWwindow* window, RenderInfo const& renderInfo);
};

