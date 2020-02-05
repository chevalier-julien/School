#pragma once

#include "RenderDevice.h"
#include "RenderObject.h"

#include <memory>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

class TileSet;
class TileSetModel;

class TileSetInstance : public RenderObject< TileSetInstance >
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
	static std::vector<TileSetInstance*>		ms_collection;

	std::shared_ptr<TileSet>			m_tileSet;
	std::shared_ptr<TileSetModel>	m_model;
	RenderDevice::Buffer				m_infosBuffer;
	RenderDevice::DescriptorSet		m_descriptorSet;
};
