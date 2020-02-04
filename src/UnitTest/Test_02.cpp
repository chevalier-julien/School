#include "Test_02.h"

#include "Engine/SvcLog.h"

Test_02::Test_02()
{
	// command line
	int arg = 1;
	char root[] = "ROOT=../../";
	char* argv[] = { root };

	Execute(arg, argv);
}

bool Test_02::Init()
{
	if (!Application::Init())
		return false;

	SvcLog::Printf("Test_02::Init()");

	return true;
}

void Test_02::Release()
{
	SvcLog::Printf("Test_02::Release()");

	Application::Release();
}
