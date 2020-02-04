#include "Application.h"

#include "xmlClassRegistry.h"
#include "ResourceManager.h"
#include "SvcTime.h"
#include "SvcLog.h"
#include "SvcConfig.h"
#include "SvcWindow.h"
#include "SvcInput.h"
#include "SvcRender.h"


void Application::Execute(int arg, char* argv[])
{
	ParseCommandLine(arg, argv);

	if (Init())
	{
		while (Update());
	}

	Release();
}

bool Application::Init()
{
	std::string root = SvcConfig::ms_defaultRoot;
	std::map<std::string, std::string>::const_iterator rootArg = m_commandLine.find("ROOT");
	if (rootArg != m_commandLine.end())
		root = rootArg->second;

	std::string config = SvcConfig::ms_defaultConfig;
	std::map<std::string, std::string>::const_iterator configArg = m_commandLine.find("CONFIG");
	if (configArg != m_commandLine.end())
		config = configArg->second;

	//////

	xmlClassRegistry::RegisterClasses();
	ResourceManager::CreateFactories();

	if (!SvcTime::Create())
		return false;

	if (!SvcLog::Create())
		return false;

	if (!SvcConfig::Create(root.c_str(), config.c_str()))
		return false;

	if (!SvcWindow::Create())
		return false;

	if (!SvcInput::Create())
		return false;

	if (!SvcRender::Create())
		return false;

	return true;
}

void Application::Release()
{
	SvcRender::Destroy();
	SvcInput::Destroy();
	SvcWindow::Destroy();
	SvcLog::Destroy();
	SvcTime::Destroy();

	ResourceManager::DestroyFactories();
	xmlClassRegistry::UnregisterClasses();
}

bool Application::Update()
{
	if (SvcWindow::GetInstance()->WindowShouldClose())
		return false;

	if (!SvcTime::Update())
		return false;

	if (!SvcWindow::Update())
		return false;

	if (!SvcRender::Update())
		return false;

	return true;
}

const std::map<std::string, std::string>& Application::GetCommandLine() const { return m_commandLine; }

void Application::ParseCommandLine(int arg, char* argv[])
{
	std::map<std::string, std::string> cmds;

	std::string key;
	while (arg > 0)
	{
		char* next_token;
		char* token = strtok_s(argv[--arg], "=", &next_token);

		if (next_token != nullptr)
		{
			m_commandLine[token] = next_token;
		}
	}
}