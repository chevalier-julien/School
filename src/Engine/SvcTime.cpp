#include "SvcTime.h"
#include <ctime>

using namespace std::chrono;

SvcTime* SvcTime::ms_instance = nullptr;

bool SvcTime::Create()
{
	ms_instance = new SvcTime();

	return ms_instance->Init();
}

void SvcTime::Destroy()
{
	ms_instance->Release();

	delete ms_instance;
	ms_instance = nullptr;
}

bool SvcTime::Update()
{
	high_resolution_clock::time_point now = high_resolution_clock::now();
	microseconds timeUs = duration_cast<microseconds>(now - high_resolution_clock::time_point());
	duration<float> elapsedTime = duration_cast<duration<float>>(now - ms_instance->m_timePoint);
	ms_instance->m_timePoint = now;
	ms_instance->m_timeUs = timeUs.count();
	ms_instance->m_elapsedTime = elapsedTime.count();

	system_clock::time_point sysTime = system_clock::now();
	time_t tt = system_clock::to_time_t(sysTime);
	localtime_s(&ms_instance->m_sysTimeInfos, &tt);
	strftime(ms_instance->m_timestamp, 32, "%T", &ms_instance->m_sysTimeInfos);

	return true;
}

u64 SvcTime::GetTimeUs()
{
	return ms_instance->m_timeUs;
}
f32 SvcTime::GetElapsedTime()
{
	return ms_instance->m_elapsedTime;
}
const char* SvcTime::GetTimestamp()
{
	return ms_instance->m_timestamp;
}
size_t SvcTime::SprintfSystemTime(char* str, size_t size, const char* format)
{
	return strftime(str, size, format, &ms_instance->m_sysTimeInfos);
}

bool SvcTime::Init()
{
	m_timePoint		= high_resolution_clock::now();
	m_timeUs			= duration_cast<microseconds>(m_timePoint - high_resolution_clock::time_point()).count();
	m_elapsedTime	= 0.0f;

	system_clock::time_point sysTime = system_clock::now();
	time_t tt = system_clock::to_time_t(sysTime);
	localtime_s(&m_sysTimeInfos, &tt);
	strftime(m_timestamp, 32, "%T", &m_sysTimeInfos);

	return true;
}

void SvcTime::Release()
{
}
