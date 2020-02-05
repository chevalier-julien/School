#include "Test_02.h"

#include "Engine/TileSetInstance.h"
#include "Engine/TileSetInstanceHelper.h"

#include "Engine/SvcLog.h"

Test_02::Test_02()
	: m_tileSetInstance(nullptr)
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

	m_tileSetInstance = CreateTileSetInstance_Map("maps/map01.xml");
	if (m_tileSetInstance == nullptr)
		return false;

	return true;
}

void Test_02::Release()
{
	SvcLog::Printf("Test_02::Release()");

	m_tileSetInstance->Release();
	delete m_tileSetInstance;
	m_tileSetInstance = nullptr;

	Application::Release();
}
