#include "TileRenderer.h"

#include "SvcLog.h"
#include "SvcRender.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

TileSet::TileSet()
	: texture(nullptr)
	, uniformBuffer(nullptr)
	, descriptorSet(nullptr)
{
}

bool TileSet::Init(RenderDevice::DescriptorPool descriptorPool, RenderDevice::DescriptorSetLayout layout, const char* textureName)
{
	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load(textureName, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

	if (!pixels)
	{
		SvcLog::Printf(SvcLog::ELevel_Error, "failed to load texture image!");
		return false;
	}

	if (!RenderDevice::GetInstance()->CreateTexture(pixels, texWidth, texHeight, &texture))
	{
		stbi_image_free(pixels);
		return false;
	}

	stbi_image_free(pixels);


	const size_t bufferSize = sizeof(Parameters);
	if (!RenderDevice::GetInstance()->CreateUniformBuffer(bufferSize, &uniformBuffer))
		return false;

	if (!RenderDevice::GetInstance()->AllocateDescriptorSets(descriptorPool, 1, &layout, &descriptorSet))
		return false;

	RenderDevice::DescriptorBufferInfo bufferInfos;
	bufferInfos.buffer = uniformBuffer;
	bufferInfos.offset = 0;
	bufferInfos.range = sizeof(Parameters);
	bufferInfos.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

	RenderDevice::DescriptorImageInfo imageInfos;
	imageInfos.texture = texture;
	imageInfos.sampler = RenderDevice::GetInstance()->GetSampler(RenderDevice::SamplerType_Nearest_Clamp);
	imageInfos.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

	RenderDevice::GetInstance()->UpdateDescriptorSet(descriptorSet, 1, &bufferInfos, 1, &imageInfos);

	return true;
}

void TileSet::Release()
{
	RenderDevice::GetInstance()->DestroyBuffer(uniformBuffer);
	uniformBuffer = nullptr;

	RenderDevice::GetInstance()->DestroyTexture(texture);
	texture = nullptr;
}


TileRenderer::TileRenderer()
	: m_tileSetDescriptorSetLayout(nullptr)
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

	const RenderDevice::DescriptorType descriptorPoolTypes[] = {
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
	};

	if (!RenderDevice::GetInstance()->CreateDescriptorPool(2, descriptorPoolTypes, MaxDescriptorCount, &m_descriptorPool))
		return false;

	const RenderDevice::DescriptorType descriptorSetTypes[] = {
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
	};

	if (!RenderDevice::GetInstance()->CreateDescriptorSetLayout(2, descriptorSetTypes, VK_SHADER_STAGE_ALL_GRAPHICS, &m_tileSetDescriptorSetLayout))
		return false;

	m_tileSet.Init(m_descriptorPool, m_tileSetDescriptorSetLayout, "../../data/texture/texture.jpg");

	RenderDevice::DescriptorSetLayout descriptorSetLayouts[] = {
		globalDescriptorSetLayout,
		m_tileSetDescriptorSetLayout
	};

	if (!RenderDevice::GetInstance()->CreateGraphicsPipeline("tile", renderPass, viewportWidth, viewportHeight, 2, descriptorSetLayouts, &m_pipeline))
		return false;

	return true;
}

void TileRenderer::Release()
{
	RenderDevice::GetInstance()->DestroyGraphicsPipeline(m_pipeline);
	m_pipeline = nullptr;

	RenderDevice::GetInstance()->DestroyDescriptorSetLayout(m_tileSetDescriptorSetLayout);
	m_tileSetDescriptorSetLayout = nullptr;

	RenderDevice::GetInstance()->DestroyBuffer(m_indexBuffer);
	m_indexBuffer = nullptr;
}

void TileRenderer::Prepare()
{

}

void TileRenderer::Render(RenderDevice::CommandBuffer commandBuffer, RenderDevice::DescriptorSet globalDescriptorSet)
{
	RenderDevice* renderDevice = RenderDevice::GetInstance();
	renderDevice->BindGrapchicsPipeline(commandBuffer, m_pipeline);
	renderDevice->BindIndexBuffer(commandBuffer, m_indexBuffer, 0);

	RenderDevice::DescriptorSet descriptorSets[] =
	{
		globalDescriptorSet,
		m_tileSet.descriptorSet
	};

	renderDevice->BindDescriptorSets(commandBuffer, m_pipeline, 0, 2, descriptorSets);
	
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
