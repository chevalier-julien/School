#pragma once

class Application
{
public:
	void Execute();

protected:
	virtual bool Init()		= 0;
	virtual bool Update()	= 0;
	virtual void Release()	= 0;
};
