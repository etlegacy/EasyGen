#include "easygen.h"
#include "ShaderFile.h"
////////
#include "Nemo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShaderFile::CShaderFile()
{
	m_sFilename = "";

	m_sCurShader  = "";
	m_pchrStartSh = NULL;
	m_pchrEndSh   = NULL;

	m_sCurStage    = "";
	m_pchrStartStg = NULL;
	m_pchrEndStg   = NULL;
}

CShaderFile::~CShaderFile()
{

}

void CShaderFile::Reset()
{
	m_sFilename = "";

	m_Parser.BufferFree();
	m_sCurShader  = "";
	m_pchrStartSh = NULL;
	m_pchrEndSh   = NULL;

	m_ShaderParser.BufferFree();
	m_sCurStage    = "";
	m_pchrStartStg = NULL;
	m_pchrEndStg   = NULL;
}

bool CShaderFile::Open(const char *filename)
{
	bool bResult;

	if (IsOpen())
	{
		Close();
	}

	bResult = m_Parser.BufferFromFile(filename);

	if (bResult)
	{
		m_sFilename = filename;
	}

	return bResult;
}

bool CShaderFile::IsOpen()
{
	return m_sFilename != "";
}

void CShaderFile::Close(bool andSave)
{
	if (IsOpen())
	{
		if (andSave)
		{
			Nm_SaveFile((const char *)m_sFilename, m_Parser.GetBuffer());
		}

		Reset();
	}
}

bool CShaderFile::sh_GetNext()
{
	int  i_context;
	char *BrNext;

	m_sCurShader  = "";
	m_pchrStartSh = NULL;
	m_pchrEndSh   = NULL;

	m_ShaderParser.BufferFree();
	m_sCurStage    = "";
	m_pchrStartStg = NULL;
	m_pchrEndStg   = NULL;

	if (m_Parser.EOP())
	{
		TRACE("EOP reached\n");
		return false;
	}

	if (!m_Parser.XGoOn("textures/"))
	{
		TRACE("quiet end, textures/ not found\n");
		return false;
	}

	m_pchrStartSh = m_Parser.GetCharPtr();

	m_Parser.XGo('{');  // eats 'textures/..\n'

	i_context = 0;

	while (i_context >= 0)
	{
		BrNext = m_Parser.FindOneOf("{}");

		if (BrNext == NULL)
		{
			i_context = -2;
			break;
		}

		*BrNext == '}' ?
		i_context--
		:
		i_context++;

		m_Parser.Seek(BrNext + 1);
	}

	if (i_context == -2)
	{
		TRACE("} not found\n");
		return false;
	}

	// BrNext, l'ultimo, è stato sicuramente un '}'
	m_pchrEndSh = BrNext + 1;

	m_sCurShader.Alloc(m_pchrEndSh - m_pchrStartSh);
	m_sCurShader.Set(m_pchrStartSh, m_pchrEndSh - m_pchrStartSh);

	m_ShaderParser.BufferCopy(m_sCurShader.GetBuffer());
	m_ShaderParser.XGo('{');

	return true;
}

bool CShaderFile::sh_GetName(CNmStr& shadername)
{
	int i_tex, i_r, i_n, i;

	if (!IsOpen() || m_sCurShader == "")
	{
		return false;
	}

	i_tex = m_sCurShader.Find("textures/");
	i_r   = m_sCurShader.Find('\r', i_tex);
	i_n   = m_sCurShader.Find('\n', i_tex);

	i_r < i_n ? i = i_r : i = i_n;

	shadername = m_sCurShader.Mid(i_tex, i - i_tex);

	return shadername != "";
}

int CShaderFile::stg_FindKey(const char *szKey)
{
	CNmStr sKey;
	int    dwPos;

	dwPos = m_sCurStage.Find(szKey);

	return dwPos;
}

bool CShaderFile::stg_GetNext()
{
	int  i_context;
	char *BrNext;

	m_sCurStage    = "";
	m_pchrStartStg = NULL;
	m_pchrEndStg   = NULL;

	if (m_ShaderParser.EOP())
	{
		TRACE("EOP reached\n");
		return false;
	}

	if (!m_ShaderParser.XGoOn('{'))
	{
		TRACE("quiet end, {-stage not found\n");
		return false;
	}

	m_pchrStartStg = m_ShaderParser.GetCharPtr();

	m_ShaderParser.XChar(); // eats '{'

	i_context = 0;

	while (i_context >= 0)
	{
		BrNext = m_ShaderParser.FindOneOf("{}");

		if (BrNext == NULL)
		{
			i_context = -2;
			break;
		}

		*BrNext == '}' ?
		i_context--
		:
		i_context++;

		m_ShaderParser.Seek(BrNext + 1);
	}

	if (i_context == -2)
	{
		TRACE("} not found\n");
		return false;
	}

	// BrNext, l'ultimo, è stato sicuramente un '}'
	m_pchrEndStg = BrNext + 1;

	m_sCurStage.Alloc(m_pchrEndStg - m_pchrStartStg);
	m_sCurStage.Set(m_pchrStartStg, m_pchrEndStg - m_pchrStartStg);

	return true;
}

const char *CShaderFile::stg_GetPtr()
{
	return m_sCurStage.GetBuffer();
}

/*
    map textures/....../bla.jpg
*/
bool CShaderFile::stg_GetValue(const char *szKey, CNmStr& value)
{
	int i_r, i_n;
	int dwKey, dwValueStart, dwValueEnd;

	value = "";

	if (!IsOpen() || m_sCurStage == "" || strlen(szKey) == 0)
	{
		return false;
	}

	dwKey = stg_FindKey(szKey);     if (dwKey == -1)
	{
		return false;
	}

	dwValueStart = dwKey + strlen(szKey) + 1;

	i_r = m_sCurStage.Find('\r', dwValueStart);
	i_n = m_sCurStage.Find('\n', dwValueStart);

	i_r < i_n ? dwValueEnd = i_r : dwValueEnd = i_n;

	if (dwValueStart == -1 || dwValueEnd == -1)
	{
		return false;
	}

	value = m_sCurStage.Mid(dwValueStart, dwValueEnd - dwValueStart);
	value.TrimLeft();

	return true;
}
