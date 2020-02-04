#include "ResourceManager.h"

#include "TileSet.h"

ResourceFactory< TileSet >* ResourceFactory< TileSet >::ms_instance = nullptr;

void ResourceManager::CreateFactories()
{
	ResourceFactory< TileSet >::Create();
}

void ResourceManager::DestroyFactories()
{
	ResourceFactory< TileSet >::Destroy();
}