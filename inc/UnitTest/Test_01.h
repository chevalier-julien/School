#pragma once

#include "Engine\Application.h"
#include "Engine\xmlUtils.h"

class xmlClass_Test : public xmlClass
{
	DECLARE_XML_CLASS()

public:
	xmlClass_Test();

public:
	i32 m_value_i32;
	u32 m_value_u32;
	u64 m_value_u64;
	f32 m_value_f32;
	std::string	m_value_string;
	std::vector< std::string >	m_values_string;
	glm::vec4 m_value_vec4;
	glm::ivec3 m_value_ivec3;
	glm::uvec2 m_value_uvec2;
};

void InitXMLClasses();
void ReleaseXMLClasses();

class Test_01 : public Application
{
public:
	Test_01();

protected:
	virtual bool Init();
	virtual void Release();
	virtual bool Update();

private:
	static const char* ms_fileName;

private:
	void Load();
	void Save();

private:
	xmlClass_Test 	m_test;
};


