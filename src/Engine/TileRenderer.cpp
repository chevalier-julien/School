#include "TileRenderer.h"

#include "SvcLog.h"
#include "SvcRender.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

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
	RenderDevice::DescriptorSetLayout descriptorSetLayouts[] = {
		globalDescriptorSetLayout
	};

	if (!RenderDevice::GetInstance()->CreateGraphicsPipeline("tile", renderPass, viewportWidth, viewportHeight, 1, descriptorSetLayouts, &m_pipeline))
		return false;

	uint16_t indices[] = {
		0, 1, 2,
		2, 1, 3
	};

	if (!RenderDevice::GetInstance()->CreateIndexBuffer(indices, sizeof(indices), &m_indexBuffer))
		return false;

	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load("../../data/textures/texture.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

	if (!pixels)
	{
		SvcLog::Printf(SvcLog::ELevel_Error, "failed to load texture image!");
		return false;
	}

	if (!RenderDevice::GetInstance()->CreateTexture(pixels, texWidth, texHeight, &m_texture))
	{
		stbi_image_free(pixels);
		return false;
	}

	stbi_image_free(pixels);

	return true;
}

void TileRenderer::Release()
{
	RenderDevice::GetInstance()->DestroyTexture(m_texture);
	m_texture = nullptr;

	RenderDevice::GetInstance()->DestroyBuffer(m_indexBuffer);
	m_indexBuffer = nullptr;

	RenderDevice::GetInstance()->DestroyGraphicsPipeline(m_pipeline);
	m_pipeline = nullptr;
}

void TileRenderer::Prepare()
{

}

void TileRenderer::Render(RenderDevice::CommandBuffer commandBuffer, RenderDevice::DescriptorSet globalDescriptorSet)
{
	RenderDevice::DescriptorSet descriptorSets[] =
	{
		globalDescriptorSet
	};

	RenderDevice* renderDevice = RenderDevice::GetInstance();
	renderDevice->BindGrapchicsPipeline(commandBuffer, m_pipeline);
	renderDevice->BindDescriptorSets(commandBuffer, m_pipeline, 0, 1, descriptorSets);
	renderDevice->BindIndexBuffer(commandBuffer, m_indexBuffer, 0);
	renderDevice->DrawIndexed(commandBuffer, 6, 1, 0, 0, 0);
}

void TileRenderer::CleanupSwapChain()
{
	RenderDevice::GetInstance()->DestroyGraphicsPipeline(m_pipeline);
	m_pipeline = nullptr;
}

bool TileRenderer::RecreateSwapChain(RenderDevice::RenderPass renderPass, size_t viewportWidth, size_t viewportHeight, RenderDevice::DescriptorSetLayout globalDescriptorSetLayout)
{
	RenderDevice::DescriptorSetLayout descriptorSetLayouts[] = {
		globalDescriptorSetLayout
	};

	if (!RenderDevice::GetInstance()->CreateGraphicsPipeline("tile", renderPass, viewportWidth, viewportHeight, 1, descriptorSetLayouts, &m_pipeline))
		return false;

	return true;
}
