#pragma once

#include <string>
#include <glm/vec2.hpp>

class TileSetInstance;

TileSetInstance* CreateTileSetInstance_Text(const std::string& text, const glm::vec2& position = glm::vec2(0.0f), const glm::vec2& scale = glm::vec2(1.0f));
TileSetInstance* CreateTileSetInstance_Map(const std::string& filename, const glm::vec2& position = glm::vec2(0.0f), const glm::vec2& scale = glm::vec2(1.0f));
