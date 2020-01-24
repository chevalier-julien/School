#include "SvcLog.h"

#include <cstdarg>
#include <iostream>

#include "SvcTime.h"

SvcLog* SvcLog::ms_instance = nullptr;

bool SvcLog::Create()
{
	ms_instance = new SvcLog();

	return ms_instance->Init();
}

void SvcLog::Destroy()
{
	ms_instance->Release();

	delete ms_instance;
	ms_instance = nullptr;
}

bool SvcLog::Init()
{
	char header[64];
	SvcTime::SprintfSystemTime(header, sizeof(header), "Start at %D %T");

	char filename[64];
	SvcTime::SprintfSystemTime(filename, sizeof(filename), "log_%Y_%m_%d.txt");

	std::cout << header << std::endl;

	m_file.open(filename);
	m_file << header << std::endl;
	m_file.flush();

	return m_file.good();
}

void SvcLog::Release()
{
	char tail[64];
	SvcTime::SprintfSystemTime(tail, sizeof(tail), "End at %D %T");

	std::cout << tail << std::endl;

	m_file << tail << std::endl;
	m_file.flush();
	m_file.close();
}

void SvcLog::VPrintf(ELevel lvl, const char* format, va_list args)
{
	vsprintf_s(ms_instance->m_msg, format, args);

	const char* lvlSymbols[] = { " ", " Warning ! ", " Error !!! " };

	std::cout << lvlSymbols[lvl];
	std::cout << ms_instance->m_msg;
	std::cout << std::endl;

	ms_instance->m_file << SvcTime::GetTimestamp();
	ms_instance->m_file << lvlSymbols[lvl];
	ms_instance->m_file << ms_instance->m_msg;
	ms_instance->m_file << std::endl;
	ms_instance->m_file.flush();
}

void SvcLog::Printf(ELevel lvl, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	VPrintf(lvl, format, args);
	va_end(args);
}

void SvcLog::Printf(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	VPrintf(ELevel_Message, format, args);
	va_end(args);
}
