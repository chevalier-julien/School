#include "TileSet.h"

#include "TileSetDesc.h"
#include "SvcConfig.h"

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

bool TileSet::Init(const TileSetDesc& desc)
{
	m_tileSize = desc.tileSize;
	m_setSize = desc.setSize;

	{
		std::string fullpath = SvcConfig::GetInstance()->GetDataPath();
		fullpath += desc.textureName;

		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load(fullpath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

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

bool TileSet::Load(const char* filename)
{
	std::string fullpath = SvcConfig::GetInstance()->GetDataPath();
	fullpath += filename;

	TileSetDesc desc;
	if (!LoadFromFile(fullpath.c_str(), desc))
		return false;

	return Init(desc);
}

void TileSet::Unload()
{
	Release();
}

u32 TileSet::GetTileSize() const							{ return m_tileSize; }
u32 TileSet::GetSetSize() const							{ return m_setSize; }

RenderDevice::Texture TileSet::GetTexture() const		{ return m_texture; }
RenderDevice::Buffer TileSet::GetInfosBuffer() const		{ return m_infosBuffer; }
