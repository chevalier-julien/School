#pragma once

#include "XMLUtils.h"

class TileSetInstance;

class TileSetMapping_Element : public xmlClass
{
	DECLARE_XML_CLASS();

public:
	TileSetMapping_Element();

	u32			m_id;
	glm::uvec3	m_color;
};

class TileSetMapping : public xmlClass
{
	DECLARE_XML_CLASS();

public:
	TileSetMapping() {}

	std::vector< TileSetMapping_Element >	m_elements;
};

class TileSetMap : public xmlClass
{
	DECLARE_XML_CLASS();

public:
	TileSetMap() {}

	std::string m_tileSetName;
	std::string m_mappingName;
	std::string m_imageName;
};
