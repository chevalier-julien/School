#pragma once

#include "Engine\Application.h"
#include "Engine\XMLUtils.h"

class xmlClass_Test : public xmlClass
{
	DECLARE_XML_CLASS()

public:
	xmlClass_Test();

public:
	S32 m_value_s32;
	U32 m_value_u32;
	U64 m_value_u64;
	F32 m_value_f32;
	std::string	m_value_string;
	std::vector< std::string >	m_values_string;
};

void InitXMLClasses();
void ReleaseXMLClasses();

class Test_01 : public Application
{
public:
	virtual bool Init();
	virtual bool Update();
	virtual void Release();

private:
	static const char* ms_fileName;

private:
	void Load();
	void Save();

private:
	xmlClass_Test 	m_test;
};


