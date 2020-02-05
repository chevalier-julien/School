#pragma once

#include <unordered_set>

template < typename T >
class RenderObject
{
public:
	RenderObject()
	{
		ms_collection.insert((T*)this);
	}
	virtual ~RenderObject()
	{
		ms_collection.erase((T*)this);
	}

	static const std::unordered_set<T*>& GetCollection() { return ms_collection; }

private:
	static std::unordered_set<T*>	ms_collection;
};

#define DEFINE_RENDER_OBJECT( T ) std::unordered_set< T* > RenderObject< T >::ms_collection
