#pragma once

#include <map>
#include <string>

class Application
{
public:
	void Execute(int arg, char* argv[]);

protected:
	virtual bool Init();
	virtual void Release();
	virtual bool Update();

	const std::map<std::string, std::string>& GetCommandLine() const;

private:
	void ParseCommandLine(int arg, char* argv[]);

private:
	std::map<std::string, std::string> m_commandLine;
};
