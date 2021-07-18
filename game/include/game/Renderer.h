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
#include <game/renderer/GeneralShadowMapRenderer.h>
#include <game/renderer/AssembleRenderer.h>
#include <game/Game.h>

#include <misc/PathManager.h>
#include <mem/Global.h>

#include <render/BufferWrappers.h>

struct RenderInfo
{
	glm::ivec2 frameSize;
	render::CameraInfo cameraInfo;
	render::TextRenderInfo textRenderInfo;
	render::UIRenderInfo uiRenderInfo;
	render::BlitArrayRenderInfo tileRenderInfo;
	render::HighlightRenderInfo highlightRenderInfo;

	std::array<std::vector<glm::mat4>, static_cast<size_t>(ModelEnum::MAX)> modelRenderInfo;

	void addModel(ModelEnum e, glm::mat4 const& transform) {
		this->modelRenderInfo[static_cast<size_t>(e)].push_back(transform);
	};
};

struct Renderer
{
public:
	render::TextRenderer textRenderer{};
	render::UIBackgroundRenderer uiBackgroundRenderer{};
	render::DebugRenderer debugRenderer{};
	render::HighlightRenderer highlightRenderer{};

	std::vector<render::GeneralRenderer> modelRenderers{};
	std::vector<render::GeneralShadowMapRenderer> shadowMapRenderers{};

	std::filesystem::path suzannePath = Global<misc::PathManager>->getModelsPath() / "Suzanne.obj";
	std::filesystem::path groundPath = Global<misc::PathManager>->getModelsPath() / "Cube.obj";

	render::GeneralRenderer suzanneRenderer;
	render::GeneralRenderer ground;

	render::bwo::Texture2D depthBuffer{ render::bwo::Texture2DHelper::makeDepthBuffer({1024*4, 1024*4}) };
	render::bwo::FrameBuffer depthTarget;

	render::AssembleRenderer assembleRenderer;

	Renderer() :
		suzanneRenderer(render::loadModel(suzannePath.string())),
		ground(render::loadModel(groundPath.string())) {
			{
				for (size_t i = 0; i < static_cast<size_t>(ModelEnum::MAX); i++) {
					auto modelName = modelNames[i];
					std::filesystem::path path = Global<misc::PathManager>->getModelsPath() / modelName;
					{
						render::GeneralRenderer ren{ render::loadModel(path.string()) };
						this->modelRenderers.push_back(std::move(ren));
					}
					{
						render::GeneralShadowMapRenderer ren{ render::loadModel(path.string()) };
						this->shadowMapRenderers.push_back(std::move(ren));
					}
				}
			}
			this->depthTarget.bindDepthLayer(depthBuffer);
			this->depthTarget.onlyDepth();
			this->depthTarget.clearDepth();
	};


	void render(GLFWwindow* window, RenderInfo const& renderInfo);
};

