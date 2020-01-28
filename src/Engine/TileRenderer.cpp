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

bool TileRenderer::Init(RenderDevice::RenderPass renderPass, size_t viewportWidth, size_t viewportHeight, RenderDevice::DescriptorSetLayout globalDescriptorSetLayout)
{
	RenderDevice* renderDevice = RenderDevice::GetInstance();

	RenderDevice::DescriptorSetLayout descriptorSetLayouts[] = {
		globalDescriptorSetLayout
	};

	if (!renderDevice->CreateGraphicsPipeline("tile", renderPass, viewportWidth, viewportHeight, 1, descriptorSetLayouts, &m_pipeline))
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

void TileRenderer::Render(RenderDevice::CommandBuffer commandBuffer, RenderDevice::DescriptorSet globalDescriptorSet)
{
	RenderDevice* renderDevice = RenderDevice::GetInstance();

	RenderDevice::DescriptorSet descriptorSets[] =
	{
		globalDescriptorSet
	};

	renderDevice->BindGrapchicsPipeline(commandBuffer, m_pipeline);
	renderDevice->BindDescriptorSets(commandBuffer, m_pipeline, 0, 1, descriptorSets);
	renderDevice->BindIndexBuffer(commandBuffer, m_indexBuffer, 0);
	renderDevice->DrawIndexed(commandBuffer, 6, 1, 0, 0, 0);
}