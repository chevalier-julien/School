#include "DeferredFunc.h"

RenderDeferredHandler_Destroy* RenderDeferredHandler_Destroy::ms_instance = nullptr;

void RenderDeferredHandler_Destroy::Create(size_t size)
{
	ms_instance = new RenderDeferredHandler_Destroy(size);
}
void RenderDeferredHandler_Destroy::Destroy()
{
	delete ms_instance;
	ms_instance = nullptr;
}
RenderDeferredHandler_Destroy* RenderDeferredHandler_Destroy::GetInstance()
{
	return ms_instance;
}

void RenderDeferredHandler_Destroy::BeginFrame()
{
	m_currentContainer = new Container;
	m_currentContainer->mask = m_size - 1;
	m_containers.push_back(m_currentContainer);
}

void RenderDeferredHandler_Destroy::DestroyItems(size_t mask)
{
	std::vector< Container* >::iterator it = m_containers.begin();
	while (it != m_containers.end())
	{
		Container* container = *(it++);

		container->mask &= ~mask;
		if (container->mask == 0)
		{
			for (IRenderDeferredItem_Destroy* item : container->items)
			{
				item->Destroy();
				delete item;
			}

			if (m_currentContainer == container)
			{
				m_currentContainer = nullptr;
			}
			delete container;

			it = m_containers.erase(it);
		}
	}
}

RenderDeferredHandler_Destroy::RenderDeferredHandler_Destroy(size_t size)
	: m_size(size)
	, m_currentContainer(nullptr)
{
}

RenderDeferredHandler_Destroy::~RenderDeferredHandler_Destroy()
{
	DestroyItems(-1);
}
