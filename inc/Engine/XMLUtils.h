#pragma once

#include "Serializer.h"

#include "libxml/tree.h"
#include <vector>


template< typename T >
const char* xmlTypeName()
{
	return T::xmlTypeName();
}

template< typename T >
void xmlWrite(const T& v, xmlNodePtr node)
{
	v.xmlWrite(node);
}

template< typename T >
void xmlRead(xmlNodePtr node, T& v)
{
	v.xmlRead(node);
}

#define DEFINE_XML_TYPE( type, name )													\
template <> inline const char * xmlTypeName< type >() { return name; }					\
template <> inline void xmlWrite< type >(const type& v, xmlNodePtr node) {				\
	char content[128];																	\
	Serializer::Serialize(v, content, sizeof(content));									\
	xmlNodeSetContent(node, reinterpret_cast<const xmlChar *>(content));					\
}																						\
template <> inline void xmlRead< type >(xmlNodePtr node, type& v) {						\
	Serializer::Unserialize(reinterpret_cast<const char*>(xmlNodeGetContent(node)), v); \
}

DEFINE_XML_TYPE(i32, "i32")
DEFINE_XML_TYPE(u32, "u32")
DEFINE_XML_TYPE(u64, "u64")
DEFINE_XML_TYPE(f32, "f32")

DEFINE_XML_TYPE(glm::vec2, "vec2")
DEFINE_XML_TYPE(glm::ivec2, "ivec2")
DEFINE_XML_TYPE(glm::uvec2, "uvec2")
DEFINE_XML_TYPE(glm::vec3, "vec3")
DEFINE_XML_TYPE(glm::ivec3, "ivec3")
DEFINE_XML_TYPE(glm::uvec3, "uvec3")
DEFINE_XML_TYPE(glm::vec4, "vec4")
DEFINE_XML_TYPE(glm::ivec4, "ivec4")
DEFINE_XML_TYPE(glm::uvec4, "uvec4")

template <> inline const char* xmlTypeName< std::string >() { return "string"; }
template <> inline void xmlWrite< std::string >(const std::string& v, xmlNodePtr node) {
	xmlNodeSetContent(node, reinterpret_cast<const xmlChar*>(v.c_str()));
}
template <> inline void xmlRead< std::string >(xmlNodePtr node, std::string& v) {
	v = reinterpret_cast<const char*>(xmlNodeGetContent(node));
}

template < typename T >
void xmlWrite(const std::vector< T >& v, xmlNodePtr node)
{
	const char* type = xmlTypeName<T>();
	for (const T& e : v)
	{
		xmlWrite(e, xmlNewChild(node, NULL, BAD_CAST type, NULL));
	}
}
template< typename T >
void xmlRead(xmlNodePtr node, std::vector< T >& v)
{
	const char* type = xmlTypeName<T>();

	v.resize(xmlChildElementCount(node));

	xmlNodePtr child = xmlFirstElementChild(node);
	for (T& e : v)
	{
		xmlRead(child, e);
		child = xmlNextElementSibling(child);
	}
}

class xmlClass;

class xmlClassMember
{
public:
	virtual void writeContent(const xmlClass* object, xmlNodePtr node) const = 0;
	virtual void readContent(xmlNodePtr node, xmlClass* object) const = 0;

public:
	std::string m_type;
	std::string m_id;
};

template< typename CType, typename MType >
class xmlClassMemberGeneric : public xmlClassMember
{
public:
	virtual void writeContent(const xmlClass* object, xmlNodePtr node) const
	{
		xmlWrite(((const CType*)object)->*m_content, node);
	}

	virtual void readContent(xmlNodePtr node, xmlClass* object) const
	{
		xmlRead(node, ((CType*)object)->*m_content);
	}

public:
	MType CType::* m_content;
};

template < typename CType, typename MType >
xmlClassMember* xmlNewClassMember(const char* id, MType CType::* ptr)
{
	xmlClassMemberGeneric< CType, MType >* member = new xmlClassMemberGeneric< CType, MType >;
	member->m_type		= ::xmlTypeName<MType>();
	member->m_id			= id;
	member->m_content	= ptr;
	return member;
}

template < typename CType, typename VType >
xmlClassMember* xmlNewClassVectorMember(const char* id, VType CType::* ptr)
{
	xmlClassMemberGeneric< CType, VType >* member = new xmlClassMemberGeneric< CType, VType >;
	member->m_type = "vector";
	member->m_id = id;
	member->m_content = ptr;
	return member;
}

class xmlClassMemberContainer
{
public:
	void PushBack(xmlClassMember* member);
	void Clear();
	void Write(const xmlClass* object, xmlNodePtr node) const;
	void Read(xmlNodePtr node, xmlClass* object) const;

private:
	std::vector< xmlClassMember* > m_data;
};

#define DECLARE_XML_CLASS()														\
public:																			\
	static const char* xmlTypeName();											\
	static void UnregisterMembers();												\
	static void RegisterMembers();												\
protected:																		\
	virtual const xmlClassMemberContainer& GetMembers() const;					\
	static xmlClassMemberContainer ms_members;

#define DEFINE_XML_CLASS_BEGIN( CType )											\
xmlClassMemberContainer CType::ms_members;										\
const char * CType::xmlTypeName() { return #CType; }								\
const xmlClassMemberContainer& CType::GetMembers() const { return ms_members; }	\
void CType::UnregisterMembers() { ms_members.Clear(); }							\
void CType::RegisterMembers() {													\
	typedef CType ClassType;
#define DEFINE_XML_CLASS_END()													\
}

#define REGISTER_XML_CLASS_MEMBER( id, member )									\
	ms_members.PushBack(xmlNewClassMember(id, &ClassType::member));

#define REGISTER_XML_CLASS_VECTOR_MEMBER( id, member )							\
	ms_members.PushBack(xmlNewClassVectorMember(id, &ClassType::member));

class xmlClass
{
	DECLARE_XML_CLASS()

public:
	void xmlWrite(xmlNodePtr node) const;
	void xmlRead(xmlNodePtr node);
};
