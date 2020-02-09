#pragma once

#include "RenderDevice.h"

class IRenderDeferredItem_Destroy
{
public:
	virtual void Destroy() = 0;
};

template< typename T>
class RenderDeferredItem_Destroy : public IRenderDeferredItem_Destroy
{
public:
	typedef void (RenderDevice::* Func)(T);

public:
	RenderDeferredItem_Destroy(T o, Func f) : object(o), pFunc(f) {}
	void Destroy()
	{
		(RenderDevice::GetInstance()->*pFunc)(object);
	}

public:
	T object;
	Func pFunc;
};

class RenderDeferredHandler_Destroy
{
private:
	struct Container
	{
		size_t mask;
		std::vector< IRenderDeferredItem_Destroy* > items;
	};

public:
	static void Create(size_t size);
	static void Destroy();
	static RenderDeferredHandler_Destroy* GetInstance();

	void BeginFrame();
	void DestroyItems(size_t mask);

	template < typename T >
	void DestroyItem(T object, void (RenderDevice::* pFunc)(T))
	{
		if (m_currentContainer != nullptr)
		{
			m_currentContainer->items.push_back(new RenderDeferredItem_Destroy(object, pFunc));
		}
		else
		{
			(RenderDevice::GetInstance()->*pFunc)(object);
		}
	}

private:
	RenderDeferredHandler_Destroy(size_t size);
	~RenderDeferredHandler_Destroy();

	static RenderDeferredHandler_Destroy* ms_instance;

	size_t m_size;
	Container* m_currentContainer;
	std::vector< Container* > m_containers;
};

template < typename T >
void RenderDeferred_Destroy(T object, void (RenderDevice::* pFunc)(T))
{
	RenderDeferredHandler_Destroy::GetInstance()->DestroyItem(object, pFunc);
}
