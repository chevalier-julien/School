#pragma once

#include "Types.h"
#include <string>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

namespace Serializer
{
	template<typename T> const char* GetFormat() { return 0; }
	template<> inline const char* GetFormat< i32 >() { return "%li"; }
	template<> inline const char* GetFormat< u32 >() { return "%lu"; }
	template<> inline const char* GetFormat< u64 >() { return "%llu"; }
	template<> inline const char* GetFormat< f32 >() { return "%f"; }

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

	template <> inline const char* GetFormat< glm::vec2 >() { return "%f,%f"; }
	template <>	inline void Serialize(const glm::vec2& v, char* buffer, size_t bufferCount) {
		sprintf_s(buffer, bufferCount, GetFormat< glm::vec2 >(), v.x, v.y);
	}
	template <>	inline void Unserialize(const char* buffer, glm::vec2& v) {
		sscanf_s(buffer, GetFormat< glm::vec2 >(), &v.x, &v.y);
	}

	template <> inline const char* GetFormat< glm::ivec2 >() { return "%i,%i"; }
	template <>	inline void Serialize(const glm::ivec2& v, char* buffer, size_t bufferCount) {
		sprintf_s(buffer, bufferCount, GetFormat< glm::ivec2 >(), v.x, v.y);
	}
	template <>	inline void Unserialize(const char* buffer, glm::ivec2& v) {
		sscanf_s(buffer, GetFormat< glm::ivec2 >(), &v.x, &v.y);
	}

	template <> inline const char* GetFormat< glm::uvec2 >() { return "%u,%u"; }
	template <> inline	void Serialize(const glm::uvec2& v, char* buffer, size_t bufferCount) {
		sprintf_s(buffer, bufferCount, GetFormat< glm::uvec2 >(), v.x, v.y);
	}
	template <> inline	void Unserialize(const char* buffer, glm::uvec2& v) {
		sscanf_s(buffer, GetFormat< glm::uvec2 >(), &v.x, &v.y);
	}

	template <> inline const char* GetFormat< glm::vec3 >() { return "%f,%f,%f"; }
	template <> inline	void Serialize(const glm::vec3& v, char* buffer, size_t bufferCount) {
		sprintf_s(buffer, bufferCount, GetFormat< glm::vec3 >(), v.x, v.y, v.z);
	}
	template <> inline	void Unserialize(const char* buffer, glm::vec3& v) {
		sscanf_s(buffer, GetFormat< glm::vec3 >(), &v.x, &v.y, &v.z);
	}

	template <> inline const char* GetFormat< glm::ivec3 >() { return "%i,%i,%i"; }
	template <> inline	void Serialize(const glm::ivec3& v, char* buffer, size_t bufferCount) {
		sprintf_s(buffer, bufferCount, GetFormat< glm::ivec3 >(), v.x, v.y, v.z);
	}
	template <> inline	void Unserialize(const char* buffer, glm::ivec3& v) {
		sscanf_s(buffer, GetFormat< glm::ivec3 >(), &v.x, &v.y, &v.z);
	}

	template <> inline const char* GetFormat< glm::uvec3 >() { return "%u,%u,%u"; }
	template <> inline	void Serialize(const glm::uvec3& v, char* buffer, size_t bufferCount) {
		sprintf_s(buffer, bufferCount, GetFormat< glm::uvec3 >(), v.x, v.y, v.z);
	}
	template <> inline	void Unserialize(const char* buffer, glm::uvec3& v) {
		sscanf_s(buffer, GetFormat< glm::uvec3 >(), &v.x, &v.y, &v.z);
	}

	template <> inline const char* GetFormat< glm::vec4 >() { return "%f,%f,%f,%f"; }
	template <> inline	void Serialize(const glm::vec4& v, char* buffer, size_t bufferCount) 	{
		sprintf_s(buffer, bufferCount, GetFormat< glm::vec4 >(), v.x, v.y, v.z, v.w);
	}
	template <> inline	void Unserialize(const char* buffer, glm::vec4& v) {
		sscanf_s(buffer, GetFormat< glm::vec4 >(), &v.x, &v.y, &v.z, &v.w);
	}

	template <> inline const char* GetFormat< glm::ivec4 >() { return "%i,%i,%i,%i"; }
	template <> inline	void Serialize(const glm::ivec4& v, char* buffer, size_t bufferCount) {
		sprintf_s(buffer, bufferCount, GetFormat< glm::ivec4 >(), v.x, v.y, v.z, v.w);
	}
	template <> inline	void Unserialize(const char* buffer, glm::ivec4& v) {
		sscanf_s(buffer, GetFormat< glm::ivec4 >(), &v.x, &v.y, &v.z, &v.w);
	}

	template <> inline const char* GetFormat< glm::uvec4 >() { return "%u,%u,%u,%u"; }
	template <> inline	void Serialize(const glm::uvec4& v, char* buffer, size_t bufferCount) {
		sprintf_s(buffer, bufferCount, GetFormat< glm::uvec4 >(), v.x, v.y, v.z, v.w);
	}
	template <> inline	void Unserialize(const char* buffer, glm::uvec4& v) {
		sscanf_s(buffer, GetFormat< glm::uvec4 >(), &v.x, &v.y, &v.z, &v.w);
	}
}
