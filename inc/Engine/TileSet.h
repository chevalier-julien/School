#pragma once

#include "RenderDevice.h"
#include "Resource.h"

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

class TileSet : public IResource
{
public:
	struct Infos
	{
		alignas(16) u32	size;
	};

public:
	TileSet();
	~TileSet();
	bool Init(const char* textureName);
	void Release();

	bool Load(const char* name);
	void Unload();
	bool Save(const char* name);

	RenderDevice::Texture GetTexture() const;
	RenderDevice::Buffer GetInfosBuffer() const;

private:
	RenderDevice::Texture m_texture;
	RenderDevice::Buffer m_infosBuffer;
};

class TileSetModel
{
public:
	struct TileData
	{
		alignas(16) u32 id;
		alignas(16) glm::vec4 offset_scale;
	};

public:
	TileSetModel();
	~TileSetModel();
	bool Init(u32 tileCount, const TileData* tileData);
	void Release();

	u32 GetTileCount() const;
	RenderDevice::Buffer GetTileBuffer() const;

private:
	u32							m_tileCount;
	RenderDevice::Buffer			m_tileBuffer;
};

class TileSetInstance
{
public:
	struct Infos
	{
		alignas(16) glm::vec4 position_scale;
	};

public:
	TileSetInstance();
	bool Init(const std::shared_ptr<TileSet>& tileSet, const std::shared_ptr<TileSetModel>& model, const glm::vec2 position, const glm::vec2 scale = glm::vec2(1.0f));
	void Release();

	const TileSet* GetTileSet() const;
	const TileSetModel* GetModel() const;
	RenderDevice::DescriptorSet	GetDescriptorSet() const;

private:
	std::shared_ptr<TileSet>			m_tileSet;
	std::shared_ptr<TileSetModel>	m_model;
	RenderDevice::Buffer				m_infosBuffer;
	RenderDevice::DescriptorSet		m_descriptorSet;
};
