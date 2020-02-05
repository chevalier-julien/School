#include "TileSetInstance.h"

#include "SvcRender.h"
#include "TileSet.h"
#include "TileSetModel.h"

DEFINE_RENDER_OBJECT( TileSetInstance );

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
		{ m_model->GetTileBuffer(), 0, m_model->GetTileCount() * sizeof(TileSetModel::TileData), VK_DESCRIPTOR_TYPE_STORAGE_BUFFER }
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

const TileSet* TileSetInstance::GetTileSet() const { return m_tileSet.get(); }
const TileSetModel* TileSetInstance::GetModel() const { return m_model.get(); }
RenderDevice::DescriptorSet	TileSetInstance::GetDescriptorSet() const { return m_descriptorSet; }
