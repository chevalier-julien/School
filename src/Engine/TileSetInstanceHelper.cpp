#include "TileSetInstanceHelper.h"

#include "SvcConfig.h"
#include "ResourceManager.h"
#include "TileSet.h"
#include "TileSetModel.h"
#include "TileSetInstance.h"
#include "TileSetMap.h"
#include "Bitmap.h"

TileSetInstance* CreateTileSetInstance_Text(const std::string& text, const glm::vec2& position, const glm::vec2& scale)
{
	std::shared_ptr< TileSet > tileSet = ResourceFactory< TileSet >::GetResource(SvcConfig::GetInstance()->GetFontTileSetName());

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

u32 uvec3ToColor(const glm::uvec3 c)
{
	return (c.r & 0xff) | (c.g & 0xff) << 8 | (c.b & 0xff) << 16;
}

TileSetInstance* CreateTileSetInstance_Map(const std::string& filename, const glm::vec2& position, const glm::vec2& scale)
{
	TileSetMap map;
	{
		const std::string tileSetMapPath = SvcConfig::GetInstance()->GetDataPath() + filename;
		if (!LoadFromFile(tileSetMapPath.c_str(), map))
			return nullptr;
	}

	std::map<u32, u32> colorToID;
	{
		TileSetMapping mapping;

		const std::string tileSetMappingPath = SvcConfig::GetInstance()->GetDataPath() + map.m_mappingName;
		if (!LoadFromFile(tileSetMappingPath.c_str(), mapping))
			return nullptr;

		for (TileSetMapping_Element e : mapping.m_elements)
		{
			colorToID[uvec3ToColor(e.m_color)] = e.m_id;
		}
	}

	std::shared_ptr< TileSet > tileSet;
	{
		tileSet = ResourceFactory< TileSet >::GetResource(map.m_tileSetName);
		if (tileSet == nullptr)
			return nullptr;
	}

	std::vector<TileSetModel::TileData> tileData;
	{
		const std::string imagePath = SvcConfig::GetInstance()->GetDataPath() + map.m_imageName;

		Bitmap image;
		if (!image.Load(imagePath.c_str()))
			return nullptr;

		unsigned char* p = image.GetData();
		for (size_t j = 0; j < image.GetHeight(); ++j)
		{
			float y = static_cast<float>(((image.GetHeight() - 1) - j) * tileSet->GetTileSize());
			for (size_t i = 0; i < image.GetWidth(); ++i, p += 4)
			{
				float x = static_cast<float>(i * tileSet->GetTileSize());

				std::map<u32, u32>::const_iterator e = colorToID.find(uvec3ToColor(glm::uvec3(p[0], p[1], p[2])));
				if (e != colorToID.end())
				{
					TileSetModel::TileData data = { e->second, glm::vec4(x, y, 1.0f, 1.0f) };
					tileData.push_back(data);
				}
			}
		}
	}

	std::shared_ptr< TileSetModel > model(new TileSetModel);
	if (!model->Init(static_cast<u32>(tileData.size()), tileData.data()))
		return nullptr;

	TileSetInstance* instance = new TileSetInstance;
	if (!instance->Init(tileSet, model, position, scale))
	{
		delete instance;
		return nullptr;
	}

	return instance;
}
