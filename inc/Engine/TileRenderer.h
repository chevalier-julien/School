#pragma once

struct IndexBuffer;

class TileRenderer
{
public:
	TileRenderer();
	~TileRenderer();

	bool Init();
	void Release();
	void Draw();

private:
	IndexBuffer* m_indexBuffer;
};
