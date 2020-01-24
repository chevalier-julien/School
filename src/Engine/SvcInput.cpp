#include "SvcInput.h"

#include "SvcWindow.h"

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	for (IKeyCallbackHandler * handler : SvcInput::ms_instance->m_keyCallbackHandlers)
	{
		handler->Execute(key, scancode, action, mods);
	}
}



SvcInput * SvcInput::ms_instance = nullptr;

bool SvcInput::Create()
{
	ms_instance = new SvcInput();
	return ms_instance->Init();
}

void SvcInput::Destroy()
{
	delete ms_instance;
	ms_instance = nullptr;
}

SvcInput* SvcInput::GetInstance()
{
	return ms_instance;
}

bool SvcInput::Init()
{
	SvcWindow::GetInstance()->SetKeyCallback(KeyCallback);

	return true;
}

void SvcInput::Release()
{
	SvcWindow::GetInstance()->SetKeyCallback(nullptr);
}