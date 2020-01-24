#pragma once

#include <fstream>

class SvcLog
{
public:
	enum ELevel
	{
		ELevel_Message,
		ELevel_Warning,
		ELevel_Error
	};

public:
	static bool Create();
	static void Destroy();

	static void VPrintf(ELevel lvl, const char* format, va_list args);
	static void Printf(ELevel lvl, const char* format, ...);
	static void Printf(const char* format, ...);

private:
	SvcLog() {}
	~SvcLog() {}

	bool Init();
	void Release();

private:
	static SvcLog*	ms_instance;
	char				m_msg[2048];
	std::ofstream	m_file;
};

