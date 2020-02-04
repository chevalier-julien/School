#include "TileSetDesc.h"

DEFINE_XML_CLASS_BEGIN(TileSetDesc)
	REGISTER_XML_CLASS_MEMBER("tileSize", tileSize)
	REGISTER_XML_CLASS_MEMBER("setSize", setSize)
	REGISTER_XML_CLASS_MEMBER("textureName", textureName);
DEFINE_XML_CLASS_END()

TileSetDesc::TileSetDesc()
	: tileSize(1)
	, setSize(1)
{
}
