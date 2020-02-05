#pragma once

#include "Engine\Application.h"

class TileSetInstance;

class Test_02 : public Application
{
public:
	Test_02();

public:
	virtual bool Init();
	virtual void Release();

private:
	TileSetInstance* m_tileSetInstance;
};


