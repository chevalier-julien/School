#pragma once

class Bitmap
{
public:
	Bitmap();
	~Bitmap();

	bool Load(const char* filename);
	bool Save(const char* filename);

	size_t GetWidth() const;
	size_t GetHeight() const;
	size_t GetChannelCount() const;
	unsigned char* GetData() const;

private:
	size_t m_width;
	size_t m_height;
	size_t m_channelCount;

	unsigned char* m_data;
};