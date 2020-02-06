#pragma once

#include <memory>
#include <unordered_map>
#include <string>

#include "SvcLog.h"

class IResource
{
public:
	virtual bool Load(const char* name) = 0;
	//virtual void Unload() = 0;
	//virtual bool Save(const char* name) = 0;
};

template< typename T >
class ResourceFactory
{
private:
	struct Element
	{
		std::string name;
		std::shared_ptr< T > pRes;
	};

	typedef std::unordered_map< std::string, Element* >	MapByName;
	typedef std::unordered_map< T*, Element* > MapByAddr;

public:
	static void Create()
	{
		ms_instance = new ResourceFactory< T >;
	}
	static void Destroy()
	{
		for (typename MapByName::value_type& v : ms_instance->m_mapByName)
		{
			SvcLog::Printf(SvcLog::ELevel_Error, "resource %s is leaking", v.first);
			delete v.second;
		}
		ms_instance->m_mapByName.clear();
		ms_instance->m_mapByAddr.clear();

		delete ms_instance;
		ms_instance = nullptr;
	}

	static std::shared_ptr< T > GetResource(const std::string& name)
	{
		typename MapByName::iterator it = ms_instance->m_mapByName.find(name);
		if (it != ms_instance->m_mapByName.end())
		{
			return it->second->pRes;
		}

		T* res = new T;
		if (!res->Load(name.c_str()))
		{
			SvcLog::Printf(SvcLog::ELevel_Error, "failed to load resource %s", name.c_str());
			delete res;
			return nullptr;
		}

		Element* e = new Element;
		e->name = name;
		e->pRes = std::shared_ptr< T >(res);

		ms_instance->m_mapByName[e->name] = e;
		ms_instance->m_mapByAddr[res] = e;

		return e->pRes;
	}

	static void ReleaseResource(std::shared_ptr< T >& pRes)
	{
		T* res = pRes.get();

		typename MapByAddr::iterator it = ms_instance->m_mapByAddr.find(res);
		if (it != ms_instance->m_mapByAddr.end())
		{
			pRes = nullptr;

			Element* e = it->second;

			if (e->pRes.use_count() == 1)
			{
				ms_instance->m_mapByAddr.erase(it);
				ms_instance->m_mapByName.erase(e->name);

				delete e;
			}
		}
	}

private:
	ResourceFactory() {}
	~ResourceFactory() {}

private:
	static ResourceFactory* ms_instance;

	MapByName	m_mapByName;
	MapByAddr	m_mapByAddr;
};
