#include "TileRenderer.h"

#include "SvcRender.h"

TileRenderer::TileRenderer()
	: m_descriptorSetLayout(nullptr)
	, m_pipeline(nullptr)
	, m_indexBuffer(nullptr)
{
	
}

TileRenderer::~TileRenderer()
{
}

bool TileRenderer::Init(RenderDevice::RenderPass renderPass, size_t width, size_t height)
{
	RenderDevice* renderDevice = RenderDevice::GetInstance();

	if (!renderDevice->CreateDescriptorSetLayout(
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		VK_SHADER_STAGE_VERTEX_BIT,
		&m_descriptorSetLayout))
		return false;

	if (!renderDevice->CreateGraphicsPipeline("tile", renderPass, width, height, 1, &m_descriptorSetLayout, &m_pipeline))
		return false;

	uint16_t indices[] = {
		0, 1, 2,
		2, 1, 3
	};

	if (!renderDevice->CreateIndexBuffer(indices, sizeof(indices), &m_indexBuffer))
		return false;

	return true;
}

void TileRenderer::Release()
{
	RenderDevice::GetInstance()->DestroyBuffer(m_indexBuffer);
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