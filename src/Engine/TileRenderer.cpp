#include "TileRenderer.h"

#include "SvcRender.h"
#include "TileSet.h"
#include "TileSetModel.h"
#include "TileSetInstance.h"
#include "TileSetInstanceHelper.h"

#include "SvcLog.h"

TileRenderer::TileRenderer()
	: m_tileSetInstanceDescriptorPool(nullptr)
	, m_tileSetInstanceDescriptorSetLayout(nullptr)
	, m_pipeline(nullptr)
	, m_indexBuffer(nullptr)
{
}

TileRenderer::~TileRenderer()
{
}

bool TileRenderer::Init(RenderDevice::RenderPass renderPass, size_t viewportWidth, size_t viewportHeight, RenderDevice::DescriptorSetLayout globalDescriptorSetLayout)
{
	const uint16_t indices[] = {
		0, 1, 2,
		2, 1, 3
	};

	if (!RenderDevice::GetInstance()->CreateIndexBuffer(indices, sizeof(indices), &m_indexBuffer))
		return false;

	const RenderDevice::DescriptorType tileSetInstanceDescriptorPoolTypes[] = {
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
	};

	if (!RenderDevice::GetInstance()->CreateDescriptorPool(3, tileSetInstanceDescriptorPoolTypes, MaxTileSetInstanceDescriptorCount, &m_tileSetInstanceDescriptorPool))
		return false;

	const RenderDevice::DescriptorType tileSetInstanceDescriptorSetTypes[] = {
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
		VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
	};

	if (!RenderDevice::GetInstance()->CreateDescriptorSetLayout(4, tileSetInstanceDescriptorSetTypes, VK_SHADER_STAGE_ALL_GRAPHICS, &m_tileSetInstanceDescriptorSetLayout))
		return false;

	RenderDevice::DescriptorSetLayout descriptorSetLayouts[] = {
		globalDescriptorSetLayout,
		m_tileSetInstanceDescriptorSetLayout
	};

	if (!RenderDevice::GetInstance()->CreateGraphicsPipeline("tile", renderPass, viewportWidth, viewportHeight, 2, descriptorSetLayouts, &m_pipeline))
		return false;

	return true;
}

void TileRenderer::Release()
{
	RenderDevice::GetInstance()->DestroyGraphicsPipeline(m_pipeline);
	m_pipeline = nullptr;

	RenderDevice::GetInstance()->DestroyDescriptorSetLayout(m_tileSetInstanceDescriptorSetLayout);
	m_tileSetInstanceDescriptorSetLayout = nullptr;

	RenderDevice::GetInstance()->DestroyDescriptorPool(m_tileSetInstanceDescriptorPool);
	m_tileSetInstanceDescriptorPool = nullptr;

	RenderDevice::GetInstance()->DestroyBuffer(m_indexBuffer);
	m_indexBuffer = nullptr;
}

void TileRenderer::Prepare()
{
	for (const TileSetInstance* instance : TileSetInstance::GetCollection())
	{
		m_tileMap.push_back(instance);
	}
}

void TileRenderer::Render(RenderDevice::CommandBuffer commandBuffer, RenderDevice::DescriptorSet globalDescriptorSet)
{
	RenderDevice* renderDevice = RenderDevice::GetInstance();
	renderDevice->BindGrapchicsPipeline(commandBuffer, m_pipeline);
	renderDevice->BindIndexBuffer(commandBuffer, m_indexBuffer, 0);

	for (const TileSetInstance* instance : m_tileMap)
	{
		const RenderDevice::DescriptorSet descriptorSets[] =
		{
			globalDescriptorSet,
			instance->GetDescriptorSet()
		};

		renderDevice->BindDescriptorSets(commandBuffer, m_pipeline, 0, 2, descriptorSets);

		renderDevice->DrawIndexed(commandBuffer, 6, instance->GetModel()->GetTileCount(), 0, 0, 0);
	}

	m_tileMap.clear();
}

void TileRenderer::CleanupSwapChain()
{
	RenderDevice::GetInstance()->DestroyGraphicsPipeline(m_pipeline);
	m_pipeline = nullptr;
}

bool TileRenderer::RecreateSwapChain(RenderDevice::RenderPass renderPass, size_t viewportWidth, size_t viewportHeight, RenderDevice::DescriptorSetLayout globalDescriptorSetLayout)
{
	RenderDevice::DescriptorSetLayout descriptorSetLayouts[] = {
		globalDescriptorSetLayout,
		m_tileSetInstanceDescriptorSetLayout
	};

	if (!RenderDevice::GetInstance()->CreateGraphicsPipeline("tile", renderPass, viewportWidth, viewportHeight, 2, descriptorSetLayouts, &m_pipeline))
		return false;

	return true;
}

RenderDevice::DescriptorSetLayout TileRenderer::GetTileSetInstanceDescriptorSetLayout() const
{
	return m_tileSetInstanceDescriptorSetLayout;
}

RenderDevice::DescriptorPool TileRenderer::GetTileSetInstanceDescriptorPool() const
{
	return m_tileSetInstanceDescriptorPool;
}
