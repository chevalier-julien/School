#include "Test_02.h"

#include "Engine/SvcLog.h"



Test_02::Test_02()
{
	Execute();
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
	SvcLog::Printf("test_02::Release()");

	Application::Release();
}
