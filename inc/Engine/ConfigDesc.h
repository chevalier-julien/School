#pragma once

#include "XMLUtils.h"

class ConfigDesc : public xmlClass
{
	DECLARE_XML_CLASS();

public:
	ConfigDesc();

	glm::uvec2	windowSize;
	std::string	windowName;
	std::string	dataPath;
};
