#ifndef SHADERFILE_H
#define SHADERFILE_H

#include "NmStr.h"
#include "NmParser.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CShaderFile
{
public:
	CShaderFile();
	virtual ~CShaderFile();

	bool    Open(const char *filename);
	bool    IsOpen();
	void    Close(bool andSave = false);

	bool    sh_GetNext();
	bool    sh_GetName(CNmStr& shadername);

	bool        stg_GetNext();
	const char *stg_GetPtr();
	bool        stg_GetValue(const char *szKey, CNmStr& value);

	void    Reset();

private:
	CNmStr m_sFilename;

	CNmParser m_Parser;         // whole file
	CNmStr    m_sCurShader;
	char      *m_pchrStartSh;
	char      *m_pchrEndSh;

	CNmParser m_ShaderParser;   // single shader
	CNmStr    m_sCurStage;
	char      *m_pchrStartStg;
	char      *m_pchrEndStg;

	int     stg_FindKey(const char *szKey);
};

#endif
