#include "Test_01.h"

#include <iostream>


DEFINE_XML_CLASS_BEGIN( xmlClass_Test )
	REGISTER_XML_CLASS_MEMBER("value_s32", m_value_s32)
	REGISTER_XML_CLASS_MEMBER("value_u32", m_value_u32)
	REGISTER_XML_CLASS_MEMBER("value_u64", m_value_u64)
	REGISTER_XML_CLASS_MEMBER("value_f32", m_value_f32)
	REGISTER_XML_CLASS_MEMBER("value_string", m_value_string)
	REGISTER_XML_CLASS_VECTOR_MEMBER("values_string", m_values_string)
DEFINE_XML_CLASS_END()

xmlClass_Test::xmlClass_Test()
	: xmlClass()
	, m_value_s32(-1)
	, m_value_u32(1)
	, m_value_u64(-1)
	, m_value_f32(0.1f)
{
}

void InitXMLClasses()
{
	xmlClass::RegisterMembers();
	xmlClass_Test::RegisterMembers();
}

void ReleaseXMLClasses()
{
	xmlClass_Test::UnregisterMembers();
	xmlClass::UnregisterMembers();
}

const char* Test_01::ms_fileName = "..\\..\\data\\config.xml";

bool Test_01::Init()
{
	printf("TestApplication::Init()");

	InitXMLClasses();

	Load();

	return true;
}

bool Test_01::Update()
{
	printf("TestApplication::Update()");

	m_test.m_value_s32--;
	m_test.m_value_u32++;
	m_test.m_value_string = std::to_string(m_test.m_value_s32);
	m_test.m_values_string.push_back(m_test.m_value_string);

	return false;
}

void Test_01::Release()
{
	printf("TestApplication::Release()");

	Save();

	ReleaseXMLClasses();
}


void Test_01::Load()
{
	xmlDocPtr doc = xmlParseFile(ms_fileName);
	if (doc)
	{
		xmlNode* root = xmlDocGetRootElement(doc);
		if (!xmlStrcmp(root->name, BAD_CAST m_test.xmlTypeName()))
		{
			xmlRead(root, m_test);
		}
	}

	xmlFreeDoc(doc);
	xmlCleanupParser();
}

void Test_01::Save()
{
	xmlDocPtr doc = xmlNewDoc(BAD_CAST XML_DEFAULT_VERSION);
	xmlNodePtr root = xmlNewDocNode(doc, NULL, BAD_CAST m_test.xmlTypeName(), NULL);
	xmlDocSetRootElement(doc, root);

	xmlWrite(m_test, root);
		
	xmlSaveFormatFile(ms_fileName, doc, 1);
	xmlFreeDoc(doc);
}
