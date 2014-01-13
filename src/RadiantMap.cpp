#include "easygen.h"
#include "RadiantMap.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRadiantMap::CRadiantMap()
{
	m_sFilename    = "";
	m_curEntity    = "";
	m_pchrStartEnt = NULL;
	m_pchrEndEnt   = NULL;
}

CRadiantMap::~CRadiantMap()
{

}

int CRadiantMap::ent_FindKey(const char *szKey)
{
	CNmStr sKey;
	int    dwPos = m_curEntity.Find('\"');
	int    dwEnd;

	while (dwPos != -1)
	{
		// we have a key here
		dwEnd = m_curEntity.Find('\"', dwPos + 1);

		if (dwEnd == -1)
		{
			break;
		}

		sKey = m_curEntity.Mid(dwPos + 1, dwEnd - dwPos - 1);

		if (sKey == szKey)
		{
			return dwPos + 1;
		}

		// skip value \" \"
		dwEnd = m_curEntity.Find('\"', dwEnd + 1);  if (dwEnd == -1)
		{
			break;
		}
		dwEnd = m_curEntity.Find('\"', dwEnd + 1);  if (dwEnd == -1)
		{
			break;
		}
		dwPos = m_curEntity.Find('\"', dwEnd + 1);
	}

	return -1;
}

int CRadiantMap::ent_FindValue(const char *szValue, int& dwKeyStart)
{
	CNmStr sValue;
	int    dwPos;
	int    dwEnd;

	dwKeyStart = -1;

	dwPos = m_curEntity.Find('\"');             if (dwPos == -1)
	{
		return -1;
	}
	dwKeyStart = dwPos + 1;
	dwPos      = m_curEntity.Find('\"', dwPos + 1);    if (dwPos == -1)
	{
		return -1;
	}
	dwPos = m_curEntity.Find('\"', dwPos + 1);

	while (dwPos != -1)
	{
		// we have a value here
		dwEnd = m_curEntity.Find('\"', dwPos + 1);

		if (dwEnd == -1)
		{
			break;
		}

		sValue = m_curEntity.Mid(dwPos + 1, dwEnd - dwPos - 1);

		if (sValue == szValue)
		{
			return dwPos + 1;
		}

		// skip key \" \"
		dwEnd = m_curEntity.Find('\"', dwEnd + 1);  if (dwEnd == -1)
		{
			break;
		}
		dwKeyStart = dwEnd + 1;
		dwEnd      = m_curEntity.Find('\"', dwEnd + 1);  if (dwEnd == -1)
		{
			break;
		}
		dwPos = m_curEntity.Find('\"', dwEnd + 1);
	}

	return -1;
}

bool CRadiantMap::Open(const char *filename)
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

bool CRadiantMap::IsOpen()
{
	return m_sFilename != "";
}

void CRadiantMap::Close(bool andSave)
{
	if (IsOpen())
	{
		if (andSave)
		{
			Nm_SaveFile((const char *)m_sFilename, m_Parser.GetBuffer());
		}

		m_Parser.BufferFree();

		m_sFilename    = "";
		m_curEntity    = "";
		m_pchrStartEnt = NULL;
		m_pchrEndEnt   = NULL;
	}
}

bool CRadiantMap::ent_GetNext()
{
	int  i_context;
	char *BrNext;

	m_curEntity    = "";
	m_pchrStartEnt = m_pchrEndEnt = NULL;

	if (m_Parser.EOP())
	{
		TRACE("EOP reached\n");
		return false;
	}

	if (!m_Parser.XGoOn('{'))
	{
		TRACE("quiet end, { not found\n");
		return false;
	}

	m_pchrStartEnt = m_Parser.GetCharPtr();

	m_Parser.XChar();   // eats '{'

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
	m_pchrEndEnt = BrNext + 1;

	m_curEntity.Alloc(m_pchrEndEnt - m_pchrStartEnt);
	m_curEntity.Set(m_pchrStartEnt, m_pchrEndEnt - m_pchrStartEnt);

	return true;
}

bool CRadiantMap::ent_Update()
{
	char *pchrPos;

	if (!IsOpen() || m_curEntity == "")
	{
		return false;
	}

	pchrPos = m_Parser.GetCharPtr();

	m_Parser.Seek(m_pchrStartEnt);
	m_Parser.wDelete(m_pchrEndEnt - m_pchrStartEnt);
	m_Parser.wInsert((const char *)m_curEntity);
	m_pchrEndEnt = m_pchrStartEnt + m_curEntity.GetLength();

	m_Parser.Seek(pchrPos);

	return true;
}

const char *CRadiantMap::ent_GetPtr()
{
	return m_curEntity.GetBuffer();
}

bool CRadiantMap::ent_GetValue(const char *szKey, CNmStr& value)
{
	int dwKey, dwValueStart, dwValueEnd;

	value = "";

	if (!IsOpen() || m_curEntity == "" || strlen(szKey) == 0)
	{
		return false;
	}

	dwKey = ent_FindKey(szKey);     if (dwKey == -1)
	{
		return false;
	}

	dwKey += strlen(szKey) + 2;

	dwValueStart = m_curEntity.Find('\"', dwKey);

	dwValueEnd = m_curEntity.Find('\"', dwValueStart + 1);

	if (dwValueStart == -1 || dwValueEnd == -1)
	{
		return false;
	}

	value = m_curEntity.Mid(dwValueStart + 1, dwValueEnd - dwValueStart - 1);

	return true;
}

bool CRadiantMap::ent_GetKey(const char *szValue, CNmStr& key)
{
	int dwValue, dwKeyStart, dwKeyEnd;

	key = "";

	if (!IsOpen() || m_curEntity == "" || strlen(szValue) == 0)
	{
		return false;
	}

	dwValue = ent_FindValue(szValue, dwKeyStart);   if (dwValue == -1)
	{
		return false;
	}

	dwKeyEnd = m_curEntity.Find('\"', dwKeyStart);

	key = m_curEntity.Mid(dwKeyStart, dwKeyEnd - dwKeyStart);

	return true;
}

bool CRadiantMap::ent_SetValue(const char *szKey, const char *szValue)
{
	int    dwExists, dwValue, dwValueEnd, dwNewPair;
	CNmStr pairnew, eol;

	if (!IsOpen() || m_curEntity == "" || strlen(szKey) == 0 || strlen(szValue) == 0)
	{
		return false;
	}

	dwExists = ent_FindKey(szKey);

	if (dwExists == -1)
	{
		// MakeNew
		dwNewPair = m_curEntity.Find('{');
		if (dwNewPair == -1)
		{
			return false;
		}
		dwNewPair++;
		dwNewPair = m_curEntity.Find('\n', dwNewPair);
		if (dwNewPair == -1)
		{
			return false;
		}

		m_curEntity.GetAt(dwNewPair - 1) == '\r' ?
		eol = "\r\n"
			  :
		      eol = "\n";

		dwNewPair++;
		pairnew.Format("\"%s\" \"%s\"%s", szKey, szValue, (const char *)eol);
		m_curEntity.Insert(dwNewPair, (const char *)pairnew);
		return true;
	}

	dwValue = m_curEntity.Find('\"', dwExists + strlen(szKey) + 2);

	if (dwValue == -1)
	{
		return false;
	}

	dwValue++;

	dwValueEnd = m_curEntity.Find('\"', dwValue);

	if (dwValueEnd == -1)   // mmm
	{
		return false;
	}

	m_curEntity.Delete(dwValue, dwValueEnd - dwValue);

	m_curEntity.Insert(dwValue, szValue);

	return true;
}
