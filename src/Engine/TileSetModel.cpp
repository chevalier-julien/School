#include "TileSetModel.h"

#include "DeferredFunc.h"

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
	RenderDeferred_Destroy(m_tileBuffer, &RenderDevice::DestroyBuffer);
	m_tileBuffer = nullptr;
}

u32 TileSetModel::GetTileCount() const { return m_tileCount; }
RenderDevice::Buffer TileSetModel::GetTileBuffer() const { return m_tileBuffer; }
