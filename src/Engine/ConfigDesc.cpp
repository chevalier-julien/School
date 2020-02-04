#include "ConfigDesc.h"


DEFINE_XML_CLASS_BEGIN(ConfigDesc)
	REGISTER_XML_CLASS_MEMBER("windowSize", windowSize)
	REGISTER_XML_CLASS_MEMBER("windowName", windowName)
	REGISTER_XML_CLASS_MEMBER("dataPath", dataPath);
DEFINE_XML_CLASS_END()

ConfigDesc::ConfigDesc()
	: windowSize(800, 600)
	, windowName("School Engine")
	, dataPath("data")
{
}
