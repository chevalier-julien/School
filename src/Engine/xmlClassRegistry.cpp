#include "xmlClassRegistry.h"

#include "xmlUtils.h"
#include "ConfigDesc.h"
#include "TileSetDesc.h"
#include "TileSetMap.h"

void xmlClassRegistry::RegisterClasses()
{
	xmlClass::RegisterMembers();

	ConfigDesc::RegisterMembers();

	TileSetDesc::RegisterMembers();
	TileSetMapping_Element::RegisterMembers();
	TileSetMapping::RegisterMembers();
	TileSetMap::RegisterMembers();
}

void xmlClassRegistry::UnregisterClasses()
{
	TileSetMap::UnregisterMembers();
	TileSetMapping::UnregisterMembers();
	TileSetMapping_Element::UnregisterMembers();
	TileSetDesc::UnregisterMembers();

	ConfigDesc::UnregisterMembers();

	xmlClass::UnregisterMembers();
}