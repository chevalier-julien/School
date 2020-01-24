#pragma once

#include <chrono>

#include "Types.h"

class SvcTime
{
public:
	static bool Create();
	static void Destroy();
	static bool Update();

	static u64 GetTimeUs();
	static f32 GetElapsedTime();
	static const char* GetTimestamp();
	static size_t SprintfSystemTime(char* str, size_t count, const char* format);

private:
	SvcTime() {}
	~SvcTime() {}

	bool Init();
	void Release();

private:
	static SvcTime* ms_instance;

	std::chrono::high_resolution_clock::time_point	m_timePoint;
	u64												m_timeUs;
	f32												m_elapsedTime;
	tm												m_sysTimeInfos;
	char												m_timestamp[32];
};
