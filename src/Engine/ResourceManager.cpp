#include "ResourceManager.h"

#include "TileSet.h"

ResourceFactory< TileSet >* ResourceFactory< TileSet >::ms_instance = nullptr;
ResourceFactory< TileSetModel >* ResourceFactory< TileSetModel >::ms_instance = nullptr;

void ResourceManager::CreateFactories()
{
	ResourceFactory< TileSet >::Create();
	ResourceFactory< TileSetModel >::Create();
}

void ResourceManager::DestroyFactories()
{
	ResourceFactory< TileSet >::Destroy();
	ResourceFactory< TileSetModel >::Destroy();
}