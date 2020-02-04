#include "TileSetMap.h"

DEFINE_XML_CLASS_BEGIN(TileSetMapping_Element)
	REGISTER_XML_CLASS_MEMBER("id", m_id)
	REGISTER_XML_CLASS_MEMBER("color", m_color)
DEFINE_XML_CLASS_END()

TileSetMapping_Element::TileSetMapping_Element()
	: m_id(0)
	, m_color(0, 0, 0)
{
}

DEFINE_XML_CLASS_BEGIN(TileSetMapping)
	REGISTER_XML_CLASS_VECTOR_MEMBER("elements", m_elements)
DEFINE_XML_CLASS_END()

DEFINE_XML_CLASS_BEGIN(TileSetMap)
	REGISTER_XML_CLASS_MEMBER("tileSetName", m_tileSetName)
	REGISTER_XML_CLASS_MEMBER("mapping", m_mappingName)
	REGISTER_XML_CLASS_MEMBER("imageName", m_imageName)
DEFINE_XML_CLASS_END()
