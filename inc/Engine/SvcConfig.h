#pragma once

#include "XMLUtils.h"

#include <glm/vec2.hpp>
#include <string>

class SvcConfig
{
public:
	static const char* ms_defaultRoot;
	static const char* ms_defaultConfig;

public:
	static bool Create(const char* root, const char* config);
	static void Destroy();

	static SvcConfig* GetInstance();

	const std::string& GetRootPath() const;
	const std::string& GetDataPath() const;
	const glm::uvec2& GetWindowSize() const;
	const std::string& GetWindowName() const;

private:
	SvcConfig() {}
	~SvcConfig() {}

	bool Init(const char* root, const char* config);
	void Release();

private:
	static SvcConfig*	ms_instance;

	std::string	m_rootPath;
	std::string	m_dataPath;
	glm::uvec2	m_windowSize;
	std::string	m_windowName;
};
