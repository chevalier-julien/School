#pragma once

#include "xmlUtils.h"

class ConfigDesc : public xmlClass
{
	DECLARE_XML_CLASS();

public:
	ConfigDesc();

	std::string	dataPath;

	glm::uvec2	windowSize;
	std::string	windowName;

	std::string fontTileSetName;
};
