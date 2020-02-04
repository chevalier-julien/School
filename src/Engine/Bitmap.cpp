#include "Bitmap.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "SvcLog.h"

Bitmap::Bitmap()
	: m_width(0)
	, m_height(0)
	, m_channelCount(0)
	, m_data(nullptr)
{
}

Bitmap::~Bitmap()
{
	if (m_data)
		stbi_image_free(m_data);
}

bool Bitmap::Load(const char* filename)
{
	int width, height, channels;
	m_data = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);

	if (!m_data)
	{
		SvcLog::Printf(SvcLog::ELevel_Error, "failed to load image %s!", filename);
		return false;
	}

	m_width = width;
	m_height = height;
	m_channelCount = channels;

	return true;
}
bool Bitmap::Save(const char* filename)
{
	// TODO
	return false;
}

size_t Bitmap::GetWidth() const { return m_width; }
size_t Bitmap::GetHeight() const { return m_height; }
size_t Bitmap::GetChannelCount() const { return m_channelCount; }
unsigned char* Bitmap::GetData() const { return m_data; }