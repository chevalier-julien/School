#include "xmlClassRegistry.h"

#include "XMLUtils.h"
#include "ConfigDesc.h"
#include "TileSetDesc.h"

void xmlClassRegistry::RegisterClasses()
{
	xmlClass::RegisterMembers();

	ConfigDesc::RegisterMembers();

	TileSetDesc::RegisterMembers();
}

void xmlClassRegistry::UnregisterClasses()
{
	TileSetDesc::UnregisterMembers();

	ConfigDesc::UnregisterMembers();

	xmlClass::UnregisterMembers();
}