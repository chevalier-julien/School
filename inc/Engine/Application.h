#pragma once

class Application
{
public:
	void Execute();

protected:
	virtual bool Init();
	virtual void Release();
	virtual bool Update();
};
