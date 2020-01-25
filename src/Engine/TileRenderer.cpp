#include "TileRenderer.h"

#include "SvcRender.h"

TileRenderer::TileRenderer()
	: m_indexBuffer(nullptr)
{
	
}

TileRenderer::~TileRenderer()
{
}

bool TileRenderer::Init()
{
	if (!SvcRender::GetInstance()->CreateGraphicsPipeline("tile"))
		return false;

	uint16_t indices[] = {
		0, 1, 2,
		2, 1, 3
	};

	m_indexBuffer = SvcRender::GetInstance()->CreateIndexBuffer(indices, sizeof(indices));

	return true;
}

void TileRenderer::Release()
{
	SvcRender::GetInstance()->DestroyIndexBuffer(m_indexBuffer);
	SvcRender::GetInstance()->DestroyGraphicsPipeline("tile");
}

void TileRenderer::Draw()
{
	SvcRender* svcRender = SvcRender::GetInstance();

	svcRender->BindGrapchicsPipeline("tile");
	svcRender->BindIndexBuffer(m_indexBuffer, 0);
	svcRender->DrawIndexed(6, 1, 0, 0, 0);
}