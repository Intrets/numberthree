#pragma once

#include <render/TextRenderer.h>
#include <render/UIBackgroundRenderer.h>
#include <render/DebugRenderer.h>
#include <render/HighlightRenderer.h>
#include <render/infos/CameraInfo.h>
#include <render/infos/TextRenderInfo.h>
#include <render/infos/BlitArrayRenderInfo.h>

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
	render::TextRenderer textRenderer;
	render::UIBackgroundRenderer uiBackgroundRenderer;
	render::DebugRenderer debugRenderer;
	render::HighlightRenderer highlightRenderer;

	void render(GLFWwindow* window, RenderInfo const& renderInfo);
};

