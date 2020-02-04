#pragma once

#include "XMLUtils.h"

class TileSetDesc : public xmlClass
{
	DECLARE_XML_CLASS();

public:
	TileSetDesc();

	u32			tileSize;
	u32			setSize;
	std::string	textureName;
};
