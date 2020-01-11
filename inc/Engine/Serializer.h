#pragma once

#include "Types.h"
#include <string>


namespace Serializer
{
	template<typename T> const char* GetFormat() { return 0; }
	template<> inline const char* GetFormat< S32 >() { return "%ld"; }
	template<> inline const char* GetFormat< U32 >() { return "%lu"; }
	template<> inline const char* GetFormat< U64 >() { return "%llu"; }
	template<> inline const char* GetFormat< F32 >() { return "%f"; }

	template < typename T >
	void Serialize(const T& v, char* buffer, size_t bufferCount)
	{
		sprintf_s(buffer, bufferCount, GetFormat<T>(), v);
	}

	template < typename T >
	void Unserialize(const char* buffer, T& v)
	{
		sscanf_s(buffer, GetFormat<T>(), &v);
	}
}
