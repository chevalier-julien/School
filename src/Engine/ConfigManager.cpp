#include "ConfigManager.h"

#include "ConfigDesc.h"

#include <filesystem>


SvcConfig* SvcConfig::ms_instance = nullptr;
const char* SvcConfig::ms_defaultRoot = "../../../";
const char* SvcConfig::ms_defaultConfig = "config.xml";

bool SvcConfig::Create(const char* root, const char* config)
{
	ms_instance = new SvcConfig();

	return ms_instance->Init(root, config);
}

void SvcConfig::Destroy()
{
	ms_instance->Release();

	delete ms_instance;
	ms_instance = nullptr;
}

SvcConfig* SvcConfig::GetInstance()
{
	return ms_instance;
}

const std::string& SvcConfig::GetRootPath() const	{ return m_rootPath; }
const std::string& SvcConfig::GetDataPath() const	{ return m_dataPath; }
const glm::uvec2& SvcConfig::GetWindowSize() const	{ return m_windowSize; }
const std::string& SvcConfig::GetWindowName() const	{ return m_windowName; }

bool SvcConfig::Init(const char* root, const char* config)
{
	std::filesystem::path rootPath = std::filesystem::absolute(root);

	m_rootPath = rootPath.string();

	std::filesystem::path configPath = rootPath;
	configPath += config;

	ConfigDesc desc;

	xmlDocPtr doc = xmlParseFile(configPath.string().c_str());
	if (!doc)
		return false;

	xmlNode* xmlRoot = xmlDocGetRootElement(doc);
	if (xmlStrcmp(xmlRoot->name, BAD_CAST desc.xmlTypeName()))
		return false;

	xmlRead(xmlRoot, desc);

	xmlFreeDoc(doc);
	xmlCleanupParser();

	//////

	std::filesystem::path dataPath = rootPath;
	dataPath += desc.dataPath;

	m_dataPath = dataPath.string();
	m_windowSize = desc.windowSize;
	m_windowName = desc.windowName;

	return true;
}

void SvcConfig::Release()
{
}
