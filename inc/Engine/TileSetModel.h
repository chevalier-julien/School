#pragma once

#include "RenderDevice.h"

#include <glm/vec4.hpp>

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
