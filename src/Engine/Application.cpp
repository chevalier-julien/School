#include "Application.h"

void Application::Execute()
{
	if (Init())
	{
		while (Update());
	}

	Release();
}
