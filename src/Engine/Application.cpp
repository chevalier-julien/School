#include "Application.h"

#include "SvcTime.h"
#include "SvcLog.h"
#include "ResourceManager.h"
#include "SvcWindow.h"
#include "SvcInput.h"
#include "SvcRender.h"

void Application::Execute()
{
	if (Init())
	{
		while (Update());
	}

	Release();
}

bool Application::Init()
{
	ResourceManager::CreateFactories();

	if (!SvcTime::Create())
		return false;

	if (!SvcLog::Create())
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
