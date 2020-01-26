#include "TileRenderer.h"

#include "SvcRender.h"

TileRenderer::TileRenderer()
	: m_pipeline(nullptr)
	, m_indexBuffer(nullptr)
{
	
}

TileRenderer::~TileRenderer()
{
}

bool TileRenderer::Init(RenderDevice::RenderPass renderPass, size_t width, size_t height)
{
	if (!RenderDevice::GetInstance()->CreateGraphicsPipeline("tile", renderPass, width, height, &m_pipeline))
		return false;

	uint16_t indices[] = {
		0, 1, 2,
		2, 1, 3
	};

	if (!RenderDevice::GetInstance()->CreateIndexBuffer(indices, sizeof(indices), &m_indexBuffer))
		return false;

	return true;
}

void TileRenderer::Release()
{
	RenderDevice::GetInstance()->DestroyIndexBuffer(m_indexBuffer);
	RenderDevice::GetInstance()->DestroyGraphicsPipeline(m_pipeline);
}

void TileRenderer::Prepare()
{

}

void TileRenderer::Render(RenderDevice::CommandBuffer commandBuffer)
{
	RenderDevice* renderDevice = RenderDevice::GetInstance();

	renderDevice->BindGrapchicsPipeline(commandBuffer, m_pipeline);
	renderDevice->BindIndexBuffer(commandBuffer, m_indexBuffer, 0);
	renderDevice->DrawIndexed(commandBuffer, 6, 1, 0, 0, 0);
}