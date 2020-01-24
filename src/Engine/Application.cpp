#include "Application.h"

#include "SvcTime.h"
#include "SvcLog.h"
#include "SvcWindow.h"
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
	if (!SvcTime::Create())
		return false;

	if (!SvcLog::Create())
		return false;

	if (!SvcWindow::Create())
		return false;

	if (!SvcRender::Create())
		return false;

	return true;
}

void Application::Release()
{
	SvcRender::Destroy();
	SvcWindow::Destroy();
	SvcLog::Destroy();
	SvcTime::Destroy();
}

bool Application::Update()
{
	if (SvcWindow::WindowShouldClose())
		return false;

	if (!SvcTime::Update())
		return false;

	if (!SvcWindow::Update())
		return false;

	if (!SvcRender::Update())
		return false;

	return true;
}
