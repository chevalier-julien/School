#include "ConfigDesc.h"


DEFINE_XML_CLASS_BEGIN(ConfigDesc)
	REGISTER_XML_CLASS_MEMBER("dataPath", dataPath);
	REGISTER_XML_CLASS_MEMBER("windowSize", windowSize)
	REGISTER_XML_CLASS_MEMBER("windowName", windowName)
	REGISTER_XML_CLASS_MEMBER("fontTileSetName", fontTileSetName);
DEFINE_XML_CLASS_END()

ConfigDesc::ConfigDesc()
	: dataPath("data")
	, windowSize(800, 600)
	, windowName("School Engine")
	, fontTileSetName("")
{
}
