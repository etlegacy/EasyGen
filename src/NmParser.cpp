////////////////
// NMPARSER.CPP //
////////////////
#include "NmParser.h"

////////

#include <stdlib.h>
#include <string.h>

////////

#include "Nemo.h"

CNmParser::CNmParser()
{
	m_Buf = NULL;
	m_Pos = NULL;
}

CNmParser::CNmParser(char *buf)
{
	BufferCopy(buf);
}

CNmParser::~CNmParser()
{
	if (m_Buf)
	{
		free(m_Buf);
	}
}

bool CNmParser::EOP()
{
	return *m_Pos == '\0';
}

char *CNmParser::Find(char c)
{
	return strchr(m_Pos, c);
}

char *CNmParser::Find(char *s)
{
	return strstr(m_Pos, s);
}

char *CNmParser::FindOneOf(const char *set)
{
	char *pchrResult = NULL;
	int  dwPos       = 0;

	while (set[dwPos] != '\0')
	{
		char *pchr = strchr(m_Pos, set[dwPos]);

		if (pchrResult == NULL || (pchr != NULL && pchr < pchrResult))
		{
			pchrResult = pchr;
		}

		dwPos++;
	}

	return pchrResult;
}

void CNmParser::BufferCopy(const char *buf)
{
	if (m_Buf)
	{
		free(m_Buf);
	}
	m_Buf = _strdup(buf);
	m_Pos = m_Buf;
}

bool CNmParser::BufferFromFile(const char *filename)
{
	if (m_Buf)
	{
		free(m_Buf);
	}

	if ((!Nm_fexist(filename)) || (Nm_LoadFile(filename, (void **)&m_Buf) == -1))
	{
		m_Buf = _strdup("");
		return false;
	}

	m_Pos = m_Buf;

	return true;
}

bool CNmParser::BufferToFile(const char *filename)
{
	if (!m_Buf)
	{
		return false;
	}

	return Nm_SaveFile(filename, m_Buf);
}

void CNmParser::BufferAppend(const char *buf, int buflen)
{
	if (buflen == 0)
	{
		return;
	}

	if (buflen == -1)
	{
		buflen = strlen(buf);
	}

	if (!m_Buf)
	{
		m_Buf = new char[buflen + 1];

		memcpy(m_Buf, buf, buflen);

		m_Buf[buflen] = '\0';

		m_Pos = m_Buf;  //	AUTO SEEK START
	}
	else
	{
		unsigned int oldBuflen;
		int          nbuflen; // new buf len
		char         *nbuf;
		unsigned int delta_buf;

		delta_buf = (unsigned int)m_Pos - (unsigned int)m_Buf;

		oldBuflen = strlen(m_Buf);

		nbuflen = oldBuflen + buflen;

		nbuf = new char[nbuflen + 1];

		memcpy(nbuf, m_Buf, oldBuflen);

		memcpy((void *)((unsigned int)nbuf + oldBuflen), buf, buflen);

		nbuf[nbuflen] = '\0';

		free(m_Buf);

		m_Buf = nbuf;

		m_Pos = (char *)((unsigned int)m_Buf + delta_buf);
	}
}

void CNmParser::BufferFree()
{
	if (m_Buf)
	{
		free(m_Buf);
		m_Buf = 0;
	}
}

char CNmParser::GetChar()
{
	return *m_Pos;
}

char *CNmParser::GetCharPtr()
{
	return m_Pos;
}

char *CNmParser::GetBuffer()
{
	return m_Buf;
}

void CNmParser::SeekStart()
{
	m_Pos = m_Buf;
}

void CNmParser::Seek(char *pos)
{
	m_Pos = pos;
}

void CNmParser::wDelete(int dwCount)
{
	char *szNew;
	int  dwNewLen;
	int  dwLen;
	int  dwHead;

	if (m_Buf == NULL)
	{
		return;
	}

	dwLen    = strlen(m_Buf);
	dwNewLen = dwLen - dwCount;

	szNew = new char[dwNewLen + 1];

	dwHead = m_Pos - m_Buf;

	memcpy(szNew, m_Buf, dwHead);

	memcpy(szNew + dwHead, m_Pos + dwCount, dwLen - dwCount - dwHead);

	szNew[dwNewLen] = '\0';

	m_Pos = szNew + dwHead;

	delete[] m_Buf;

	m_Buf = szNew;
}

void CNmParser::wInsert(const char *szBuf, bool bIncPos)
{
	char *szNew;
	int  dwBufLen;
	int  dwLen;
	int  dwHead;

	if (m_Buf == NULL)
	{
		return;
	}

	dwBufLen = strlen(szBuf);
	dwLen    = strlen(m_Buf);

	szNew = new char[dwLen + dwBufLen + 1];

	dwHead = m_Pos - m_Buf;

	memcpy(szNew, m_Buf, dwHead);
	memcpy(szNew + dwHead, szBuf, dwBufLen);
	memcpy(szNew + dwHead + dwBufLen, m_Pos, dwLen - dwHead);

	szNew[dwLen + dwBufLen] = '\0';

	m_Pos = szNew + dwHead;

	delete[] m_Buf;

	m_Buf = szNew;

	if (bIncPos)
	{
		m_Pos = m_Pos + dwBufLen;
	}
}

char CNmParser::XChar()
{
	char c = *m_Pos;
	m_Pos++;
	return c;
}

void CNmParser::XClear(char c)
{
	while ((GetChar() == c) && (GetChar() != '\0'))
		XChar();
}

void CNmParser::XClear(const char *setc)
{
	while ((strchr(setc, GetChar()) != NULL) && (GetChar() != '\0'))
		XChar();
}

float CNmParser::XFloat(bool trim)
{
	int  i = 0;
	char number[256];
	char c;

	if (trim)
	{
		XClear(' ');
	}

	c = GetChar();
	while (((c >= '0') && (c <= '9')) || (c == '.') || (c == '-') || (c == '+'))
	{
		XChar();
		number[i++] = c;
		c           = GetChar();
	}

	number[i] = '\0';

	return (float)atof(number);
}

bool CNmParser::XGo(char c)
{
	char *p = strchr(m_Pos, c);

	if (p == 0)
	{
		return false;
	}

	m_Pos = p + 1;

	return true;
}

bool CNmParser::XGo(const char *s)
{
	char *c = strstr(m_Pos, s);

	if (c == 0)
	{
		return false;
	}

	m_Pos = c + strlen(s);

	return true;
}

bool CNmParser::XGoOn(char c)
{
	char *p = strchr(m_Pos, c);

	if (p == 0)
	{
		return false;
	}

	m_Pos = p;

	return true;
}

bool CNmParser::XGoOn(const char *s)
{
	char *c = strstr(m_Pos, s);

	if (c == 0)
	{
		return false;
	}

	m_Pos = c;

	return true;
}

bool CNmParser::XGoOneOf(const char *set)
{
	char *pchr = FindOneOf(set);

	if (pchr == NULL)
	{
		return false;
	}

	m_Pos = pchr + 1;

	return true;
}

bool CNmParser::XGoOneOfOn(const char *set)
{
	char *pchr = FindOneOf(set);

	if (pchr == NULL)
	{
		return false;
	}

	m_Pos = pchr;

	return true;
}

bool CNmParser::XString(char **dst, char end)
{
	char         *cc;
	unsigned int len;

	cc = strchr(m_Pos, end);

	if (cc == NULL)
	{
		*dst = NULL;
		return false;
	}

	len = (unsigned int)cc - (unsigned int)m_Pos;

	*dst = new char[len + 1];

	memcpy(*dst, m_Pos, len);

	m_Pos = cc;

	(*dst)[len] = '\0';

	return true;
}

bool CNmParser::XString(CNmStr& str, char end)
{
	char         *cc;
	unsigned int len;

	cc = strchr(m_Pos, end);

	if (cc == NULL)
	{
		str = "";
		return false;
	}

	len = (unsigned int)cc - (unsigned int)m_Pos;

	str.Alloc(len);

	str.Set(m_Pos, len);

	m_Pos = cc;

	return true;
}