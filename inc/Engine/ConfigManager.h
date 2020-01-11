#pragma once

#include <string>




class ConfigManager
{
public:
	static const char* ms_configFileName;

public:
	ConfigManager();

private:
	std::string	m_dataPath;
};
