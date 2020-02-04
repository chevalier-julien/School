#pragma once

#include "RenderDevice.h"
#include "Resource.h"
#include "xmlUtils.h"

#include <glm/vec2.hpp>

class TileSetDesc;

class TileSet : public IResource
{
public:
	struct Infos
	{
		alignas(16) glm::uvec2	sizes;
	};

public:
	TileSet();
	~TileSet();

	bool Init(const TileSetDesc& desc);
	void Release();

	bool Load(const char* filename);
	void Unload();

	u32 GetTileSize() const;
	u32 GetSetSize() const;

	RenderDevice::Texture GetTexture() const;
	RenderDevice::Buffer GetInfosBuffer() const;

private:
	u32 m_tileSize;
	u32 m_setSize;

	RenderDevice::Texture m_texture;
	RenderDevice::Buffer m_infosBuffer;
};
