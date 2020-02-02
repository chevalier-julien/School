#include "TileSet.h"

#include "SvcRender.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "SvcLog.h"

TileSet::TileSet()
	: m_texture(nullptr)
	, m_infosBuffer(nullptr)
{
}

TileSet::~TileSet()
{
	Release();
}

bool TileSet::Init(const char* textureName)
{
	m_tileSize = 8;
	m_setSize = 16;

	{
		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load(textureName, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

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
	}

	{
		const size_t bufferSize = sizeof(Infos);
		if (!RenderDevice::GetInstance()->CreateUniformBuffer(bufferSize, &m_infosBuffer))
			return false;

		Infos infos;
		infos.sizes[0] = m_tileSize;
		infos.sizes[1] = m_setSize;

		void* data;
		if (!RenderDevice::GetInstance()->MapBuffer(m_infosBuffer, 0, bufferSize, &data))
			return false;

		memcpy(data, &infos, bufferSize);

		RenderDevice::GetInstance()->UnmapBuffer(m_infosBuffer);
	}

	return true;
}

void TileSet::Release()
{
	RenderDevice::GetInstance()->DestroyBuffer(m_infosBuffer);
	m_infosBuffer = nullptr;

	RenderDevice::GetInstance()->DestroyTexture(m_texture);
	m_texture = nullptr;
}

bool TileSet::Load(const char* name)
{
	return Init(name);
}

void TileSet::Unload()
{
	Release();
}

bool TileSet::Save(const char* name)
{
	return true;
}

u32 TileSet::GetTileSize() const							{ return m_tileSize; }
u32 TileSet::GetSetSize() const							{ return m_setSize; }

RenderDevice::Texture TileSet::GetTexture() const		{ return m_texture; }
RenderDevice::Buffer TileSet::GetInfosBuffer() const		{ return m_infosBuffer; }


TileSetModel::TileSetModel()
	: m_tileCount(0)
	, m_tileBuffer(nullptr)
{
}

TileSetModel::~TileSetModel()
{
	Release();
}

bool TileSetModel::Init(u32 tileCount, const TileData* tileData)
{
	m_tileCount = tileCount;

	const size_t bufferSize = sizeof(TileData) * m_tileCount;
	if (!RenderDevice::GetInstance()->CreateStorageBuffer(bufferSize, &m_tileBuffer))
		return false;

	void* data;
	if (!RenderDevice::GetInstance()->MapBuffer(m_tileBuffer, 0, bufferSize, &data))
		return false;

	memcpy(data, tileData, bufferSize);

	RenderDevice::GetInstance()->UnmapBuffer(m_tileBuffer);

	return true;
}

void TileSetModel::Release()
{
	RenderDevice::GetInstance()->DestroyBuffer(m_tileBuffer);
	m_tileBuffer = nullptr;
}

u32 TileSetModel::GetTileCount() const						{ return m_tileCount; }
RenderDevice::Buffer TileSetModel::GetTileBuffer() const		{ return m_tileBuffer;  }


TileSetInstance::TileSetInstance()
	: m_infosBuffer(nullptr)
	, m_descriptorSet(nullptr)
{
}

bool TileSetInstance::Init(const std::shared_ptr<TileSet>& tileSet, const std::shared_ptr<TileSetModel>& model, const glm::vec2 position, const glm::vec2 scale)
{
	m_tileSet = tileSet;
	m_model = model;

	const size_t bufferSize = sizeof(Infos);
	if (!RenderDevice::GetInstance()->CreateUniformBuffer(bufferSize, &m_infosBuffer))
		return false;

	Infos infos;
	infos.position_scale = glm::vec4(position, scale);

	void* data;
	if (!RenderDevice::GetInstance()->MapBuffer(m_infosBuffer, 0, bufferSize, &data))
		return false;

	memcpy(data, &infos, bufferSize);

	RenderDevice::GetInstance()->UnmapBuffer(m_infosBuffer);

	const TileRenderer& tileRenderer = SvcRender::GetInstance()->GetTileRenderer();
	RenderDevice::DescriptorPool descriptorPool = tileRenderer.GetTileSetInstanceDescriptorPool();
	RenderDevice::DescriptorSetLayout layout = tileRenderer.GetTileSetInstanceDescriptorSetLayout();

	if (!RenderDevice::GetInstance()->AllocateDescriptorSets(descriptorPool, 1, &layout, &m_descriptorSet))
		return false;

	RenderDevice::DescriptorBufferInfo bufferInfos[] = {
		{ m_tileSet->GetInfosBuffer(), 0, sizeof(TileSet::Infos), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER },
		{ m_infosBuffer, 0, sizeof(Infos), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER },
		{ m_model->GetTileBuffer(), 0, m_model->GetTileCount()*sizeof(TileSetModel::TileData), VK_DESCRIPTOR_TYPE_STORAGE_BUFFER }
	};

	RenderDevice::DescriptorImageInfo imageInfos = {
		m_tileSet->GetTexture(),
		RenderDevice::GetInstance()->GetSampler(RenderDevice::SamplerType_Nearest_Clamp),
		VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
	};
	RenderDevice::GetInstance()->UpdateDescriptorSet(m_descriptorSet, 3, bufferInfos, 1, &imageInfos);

	return true;
}

void TileSetInstance::Release()
{
	RenderDevice::GetInstance()->DestroyBuffer(m_infosBuffer);
	m_infosBuffer = nullptr;

	m_model = nullptr;

	ResourceFactory< TileSet >::ReleaseResource(m_tileSet);
}

const TileSet* TileSetInstance::GetTileSet() const						{ return m_tileSet.get(); }
const TileSetModel* TileSetInstance::GetModel() const					{ return m_model.get(); }
RenderDevice::DescriptorSet	TileSetInstance::GetDescriptorSet() const	{ return m_descriptorSet; }


TileSetInstance* CreateTileSetInstance_Text(const std::string& text, const glm::vec2& position, const glm::vec2& scale)
{
	std::shared_ptr< TileSet > tileSet = ResourceFactory< TileSet >::GetResource("../../data/textures/font.png");

	if (tileSet == nullptr)
		return nullptr;

	std::vector<TileSetModel::TileData> tileData(text.size());

	u32 tileCount = 0;
	glm::vec4 offset_scale(0.0f, 0.0f, 1.0f, 1.0f);
	for (size_t i = 0; i < text.size(); ++i)
	{
		char c = text[i];
		if (c == '\n')
		{
			offset_scale.x = 0.0f;
			offset_scale.y += tileSet->GetTileSize();
			continue;
		}

		tileData[tileCount].id = c;
		tileData[tileCount].offset_scale = offset_scale;
		offset_scale.x += tileSet->GetTileSize();
		++tileCount;
	}

	std::shared_ptr< TileSetModel > model(new TileSetModel);
	if (!model->Init(tileCount, tileData.data()))
		return nullptr;

	TileSetInstance* instance = new TileSetInstance;
	if (!instance->Init(tileSet, model, position, scale))
	{
		delete instance;
		return nullptr;
	}

	return instance;
}