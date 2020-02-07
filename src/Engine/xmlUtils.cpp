#include "xmlUtils.h"


void xmlClassMemberContainer::PushBack(xmlClassMember* member)
{
	m_data.push_back(member);
}

void xmlClassMemberContainer::Clear()
{
	for (xmlClassMember* m : m_data)
	{
		delete m;
	}
	m_data.clear();
}

void xmlClassMemberContainer::Write(const xmlClass * object, xmlNodePtr node) const
{
	for (const xmlClassMember* m : m_data)
	{
		xmlNodePtr child = xmlNewChild(node, NULL, BAD_CAST m->m_type.c_str(), NULL);
		xmlNewProp(child, BAD_CAST "id", BAD_CAST m->m_id.c_str());

		m->writeContent(object, child);
	}
}

void xmlClassMemberContainer::Read(xmlNodePtr node, xmlClass* object) const
{
	for (const xmlClassMember* m : m_data)
	{
		for (xmlNodePtr child = xmlFirstElementChild(node); child; child = xmlNextElementSibling(child))
		{
			if (!xmlStrcmp(child->name, BAD_CAST m->m_type.c_str()))
			{
				if (!xmlStrcmp(xmlGetProp(child, BAD_CAST "id"), BAD_CAST m->m_id.c_str()))
				{
					m->readContent(child, object);
					break;
				}
			}
		}
	}
}


DEFINE_XML_CLASS_BEGIN( xmlClass )
DEFINE_XML_CLASS_END()

void xmlClass::xmlWrite(xmlNodePtr node) const
{
	GetMembers().Write(this, node);
}

void xmlClass::xmlRead(xmlNodePtr node)
{
	GetMembers().Read(node, this);
}
