#include "TileRenderer.h"

#include "SvcLog.h"
#include "SvcRender.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>


TileSetInstance::TileSetInstance()
	: count(0)
	, infosBuffer(nullptr)
	, instanceBuffer(nullptr)
	, descriptorSet(nullptr)
{
}

bool TileSetInstance::Init(RenderDevice::DescriptorPool descriptorPool, RenderDevice::DescriptorSetLayout layout)
{
	count = 4;
	{
		const size_t bufferSize = sizeof(Data) * count;
		if (!RenderDevice::GetInstance()->CreateStorageBuffer(bufferSize, &instanceBuffer))
			return false;

		std::vector<Data> instances(count);
		for (size_t i = 0; i < count; ++i)
		{
			instances[i].id = 0;
			instances[i].position = glm::vec2(float(i % 2), float(i / 2)) * 256.0f;
		}

		void* data;
		if (!RenderDevice::GetInstance()->MapBuffer(instanceBuffer, 0, bufferSize, &data))
			return false;

		memcpy(data, instances.data(), bufferSize);

		RenderDevice::GetInstance()->UnmapBuffer(instanceBuffer);
	}

	{
		const size_t bufferSize = sizeof(Infos);
		if (!RenderDevice::GetInstance()->CreateUniformBuffer(bufferSize, &infosBuffer))
			return false;

		Infos infos;
		infos.position = glm::vec2(0.0, 0.0);

		void* data;
		if (!RenderDevice::GetInstance()->MapBuffer(infosBuffer, 0, bufferSize, &data))
			return false;

		memcpy(data, &infos, bufferSize);

		RenderDevice::GetInstance()->UnmapBuffer(infosBuffer);
	}

	{
		if (!RenderDevice::GetInstance()->AllocateDescriptorSets(descriptorPool, 1, &layout, &descriptorSet))
			return false;

		RenderDevice::DescriptorBufferInfo bufferInfos[] = {
			{ infosBuffer, 0, sizeof(Infos), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER },
			{ instanceBuffer, 0, static_cast<uint32_t>(sizeof(Data)*count), VK_DESCRIPTOR_TYPE_STORAGE_BUFFER }
		};
		RenderDevice::GetInstance()->UpdateDescriptorSet(descriptorSet, 2, bufferInfos, 0, nullptr);
	}

	return true;
}

void TileSetInstance::Release()
{
	RenderDevice::GetInstance()->DestroyBuffer(infosBuffer);
	infosBuffer = nullptr;

	RenderDevice::GetInstance()->DestroyBuffer(instanceBuffer);
	instanceBuffer = nullptr;
}

TileSet::TileSet()
	: texture(nullptr)
	, uniformBuffer(nullptr)
	, descriptorSet(nullptr)
{
}

bool TileSet::Init(RenderDevice::DescriptorPool descriptorPool, RenderDevice::DescriptorSetLayout layout, const char* textureName)
{
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
	}

	{
		const size_t bufferSize = sizeof(Parameters);
		if (!RenderDevice::GetInstance()->CreateUniformBuffer(bufferSize, &uniformBuffer))
			return false;

		Parameters params;
		params.size = 1;

		void* data;
		if (!RenderDevice::GetInstance()->MapBuffer(uniformBuffer, 0, sizeof(Parameters), &data))
			return false;

		memcpy(data, &params, sizeof(Parameters));

		RenderDevice::GetInstance()->UnmapBuffer(uniformBuffer);
	}

	{
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
	}

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
	, m_tileSetInstanceDescriptorSetLayout(nullptr)
	, m_pipeline(nullptr)
	, m_indexBuffer(nullptr)
	, m_tileSetDescriptorPool(nullptr)
	, m_tileSetInstanceDescriptorPool(nullptr)
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

	const RenderDevice::DescriptorType tileSetDescriptorPoolTypes[] = {
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
	};

	if (!RenderDevice::GetInstance()->CreateDescriptorPool(2, tileSetDescriptorPoolTypes, MaxTileSetDescriptorCount, &m_tileSetDescriptorPool))
		return false;

	const RenderDevice::DescriptorType tileSetDescriptorSetTypes[] = {
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
	};

	if (!RenderDevice::GetInstance()->CreateDescriptorSetLayout(2, tileSetDescriptorSetTypes, VK_SHADER_STAGE_ALL_GRAPHICS, &m_tileSetDescriptorSetLayout))
		return false;

	const RenderDevice::DescriptorType tileSetInstanceDescriptorPoolTypes[] = {
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
	};

	if (!RenderDevice::GetInstance()->CreateDescriptorPool(2, tileSetInstanceDescriptorPoolTypes, MaxTileSetInstanceDescriptorCount, &m_tileSetInstanceDescriptorPool))
		return false;

	const RenderDevice::DescriptorType tileSetInstanceDescriptorSetTypes[] = {
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
	};

	if (!RenderDevice::GetInstance()->CreateDescriptorSetLayout(2, tileSetInstanceDescriptorSetTypes, VK_SHADER_STAGE_ALL_GRAPHICS, &m_tileSetInstanceDescriptorSetLayout))
		return false;

	m_tileSet.Init(m_tileSetDescriptorPool, m_tileSetDescriptorSetLayout, "../../data/textures/texture.jpg");
	m_tileSetInstance.Init(m_tileSetInstanceDescriptorPool, m_tileSetInstanceDescriptorSetLayout);

	RenderDevice::DescriptorSetLayout descriptorSetLayouts[] = {
		globalDescriptorSetLayout,
		m_tileSetDescriptorSetLayout,
		m_tileSetInstanceDescriptorSetLayout
	};

	if (!RenderDevice::GetInstance()->CreateGraphicsPipeline("tile", renderPass, viewportWidth, viewportHeight, 3, descriptorSetLayouts, &m_pipeline))
		return false;

	return true;
}

void TileRenderer::Release()
{
	RenderDevice::GetInstance()->DestroyGraphicsPipeline(m_pipeline);
	m_pipeline = nullptr;

	m_tileSetInstance.Release();
	m_tileSet.Release();

	RenderDevice::GetInstance()->DestroyDescriptorSetLayout(m_tileSetInstanceDescriptorSetLayout);
	m_tileSetInstanceDescriptorSetLayout = nullptr;

	RenderDevice::GetInstance()->DestroyDescriptorPool(m_tileSetInstanceDescriptorPool);
	m_tileSetInstanceDescriptorPool = nullptr;

	RenderDevice::GetInstance()->DestroyDescriptorSetLayout(m_tileSetDescriptorSetLayout);
	m_tileSetDescriptorSetLayout = nullptr;

	RenderDevice::GetInstance()->DestroyDescriptorPool(m_tileSetDescriptorPool);
	m_tileSetDescriptorPool = nullptr;

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
		m_tileSet.descriptorSet,
		m_tileSetInstance.descriptorSet
	};

	renderDevice->BindDescriptorSets(commandBuffer, m_pipeline, 0, 3, descriptorSets);
	
	renderDevice->DrawIndexed(commandBuffer, 6, static_cast<u32>(m_tileSetInstance.count), 0, 0, 0);
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
		m_tileSetDescriptorSetLayout,
		m_tileSetInstanceDescriptorSetLayout
	};

	if (!RenderDevice::GetInstance()->CreateGraphicsPipeline("tile", renderPass, viewportWidth, viewportHeight, 3, descriptorSetLayouts, &m_pipeline))
		return false;

	return true;
}
