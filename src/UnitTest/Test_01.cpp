#include "Test_01.h"

#include <iostream>

#include "Engine/SvcLog.h"

DEFINE_XML_CLASS_BEGIN( xmlClass_Test )
	REGISTER_XML_CLASS_MEMBER("value_i32", m_value_i32)
	REGISTER_XML_CLASS_MEMBER("value_u32", m_value_u32)
	REGISTER_XML_CLASS_MEMBER("value_u64", m_value_u64)
	REGISTER_XML_CLASS_MEMBER("value_f32", m_value_f32)
	REGISTER_XML_CLASS_MEMBER("value_string", m_value_string)
	REGISTER_XML_CLASS_VECTOR_MEMBER("values_string", m_values_string)
	REGISTER_XML_CLASS_MEMBER("value_vec4", m_value_vec4)
	REGISTER_XML_CLASS_MEMBER("value_ivec3", m_value_ivec3)
	REGISTER_XML_CLASS_MEMBER("value_uvec2", m_value_uvec2)
DEFINE_XML_CLASS_END()

xmlClass_Test::xmlClass_Test()
	: xmlClass()
	, m_value_i32(-1)
	, m_value_u32(1)
	, m_value_u64(-1)
	, m_value_f32(0.1f)
	, m_value_vec4(glm::vec4(0.0f))
	, m_value_ivec3(glm::ivec3(0))
	, m_value_uvec2(glm::uvec2(0))
{
}

void InitXMLClasses()
{
	xmlClass_Test::RegisterMembers();
}

void ReleaseXMLClasses()
{
	xmlClass::UnregisterMembers();
}

const char* Test_01::ms_fileName = "test.xml";

Test_01::Test_01()
{
	// command line
	int arg = 1;
	char root[] = "ROOT=../../";
	char* argv[] = { root };

	Execute(arg, argv);
}

bool Test_01::Init()
{
	if (!Application::Init())
		return false;

	SvcLog::Printf("Test_01::Init()");

	InitXMLClasses();

	Load();

	return true;
}

void Test_01::Release()
{
	SvcLog::Printf("Test_01::Release()");

	Save();

	ReleaseXMLClasses();

	Application::Release();
}

bool Test_01::Update()
{
	if (!Application::Update())
		return false;

	SvcLog::Printf("Test_01::Update()");

	m_test.m_value_i32--;
	m_test.m_value_u32++;
	m_test.m_value_string = std::to_string(m_test.m_value_i32);
	m_test.m_values_string.push_back(m_test.m_value_string);
	m_test.m_value_vec4[m_test.m_value_u32 % 4]++;
	m_test.m_value_ivec3[m_test.m_value_u32 % 3]--;
	m_test.m_value_uvec2[m_test.m_value_u32 % 2]++;

	SvcLog::Printf("%i %u %f,%f,%f,%f",
		m_test.m_value_i32,
		m_test.m_value_u32,
		m_test.m_value_vec4[0], m_test.m_value_vec4[1], m_test.m_value_vec4[2], m_test.m_value_vec4[3]);

	return false;
}

void Test_01::Load()
{
	LoadFromFile(ms_fileName, m_test);
}

void Test_01::Save()
{
	SaveToFile(ms_fileName, m_test);
}
