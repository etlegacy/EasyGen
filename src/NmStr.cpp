/////////////
// NMSTR.CPP //
/////////////
#include "Nemo.h"
#include "NmStr.h"
////////
#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <stdarg.h>
/////////////

/*
Da tenere presente che il puntatore al m_Buf, buffer dei caratteri non DEVE MAI essere = 0.
Se la stringa è vuota m_Buf = ""
In nessuna condizione m_Buf = 0
*/
bool operator==(const CNmStr& s1, const CNmStr& s2)
{
	return strcmp(s1.m_Buf, s2.m_Buf) == 0;
}

bool operator==(const CNmStr& s1, const char *s2)
{
	return strcmp(s1.m_Buf, s2) == 0;
}

bool operator==(const char *s1, const CNmStr& s2)
{
	return strcmp(s1, s2.m_Buf) == 0;
}

bool operator!=(const CNmStr& s1, const CNmStr& s2)
{
	return strcmp(s1.m_Buf, s2.m_Buf) != 0;
}

bool operator!=(const CNmStr& s1, const char *s2)
{
	return strcmp(s1.m_Buf, s2) != 0;
}

bool operator!=(const char *s1, const CNmStr& s2)
{
	return strcmp(s1, s2.m_Buf) != 0;
}

bool operator<(const CNmStr& s1, const CNmStr& s2)
{
	return strcmp(s1.m_Buf, s2.m_Buf) < 0;
}

bool operator<(const CNmStr& s1, const char *s2)
{
	return strcmp(s1.m_Buf, s2) < 0;
}

bool operator<(const char *s1, const CNmStr& s2)
{
	return strcmp(s1, s2.m_Buf) < 0;
}

bool operator<=(const CNmStr& s1, const CNmStr& s2)
{
	return strcmp(s1.m_Buf, s2.m_Buf) <= 0;
}

bool operator<=(const CNmStr& s1, const char *s2)
{
	return strcmp(s1.m_Buf, s2) <= 0;
}

bool operator<=(const char *s1, const CNmStr& s2)
{
	return strcmp(s1, s2.m_Buf) <= 0;
}

bool operator>(const CNmStr& s1, const CNmStr& s2)
{
	return strcmp(s1.m_Buf, s2.m_Buf) > 0;
}

bool operator>(const CNmStr& s1, const char *s2)
{
	return strcmp(s1.m_Buf, s2) > 0;
}

bool operator>(const char *s1, const CNmStr& s2)
{
	return strcmp(s1, s2.m_Buf) > 0;
}

bool operator>=(const CNmStr& s1, const CNmStr& s2)
{
	return strcmp(s1.m_Buf, s2.m_Buf) >= 0;
}

bool operator>=(const CNmStr& s1, const char *s2)
{
	return strcmp(s1.m_Buf, s2) >= 0;
}

bool operator>=(const char *s1, const CNmStr& s2)
{
	return strcmp(s1, s2.m_Buf) >= 0;
}

CNmStr operator+(const CNmStr& s1, const CNmStr& s2)
{
	CNmStr s(s1);
	s += s2;
	return s;
}

CNmStr operator+(const CNmStr& s1, const char *s2)
{
	CNmStr s(s1);
	s += s2;
	return s;
}

CNmStr operator+(const char *s1, const CNmStr& s2)
{
	CNmStr s(s1);
	s += s2;
	return s;
}

CNmStr operator+(const CNmStr& s1, char c)
{
	CNmStr s(s1);
	s += c;
	return s;
}

CNmStr operator+(char c, const CNmStr& s2)
{
	CNmStr s;
	char   a[2];
	a[0] = c;
	a[1] = '\0';
	s    = a + s2;
	return s;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNmStr::CNmStr()
{
	m_Buf = _strdup("");
}

CNmStr::CNmStr(const char *s)
{
	m_Buf = _strdup(s);
}

CNmStr::CNmStr(const CNmStr& s)
{
	m_Buf = _strdup(s.m_Buf);
}

CNmStr::~CNmStr()
{
	free(m_Buf);
}

bool CNmStr::Alloc(int nLength)
{
	bool result;

	if (nLength < 0)
	{
		return false;
	}

	free(m_Buf);

	m_Buf = (char *)malloc(nLength + 1);

	result = (m_Buf != NULL);

	if (m_Buf == NULL)
	{
		m_Buf = _strdup("");
	}
	else
	{
		strcpy(m_Buf, "");
	}

	return result;
}

void CNmStr::Set(const char *src, int nLength)
{
	memcpy(m_Buf, src, nLength);
	m_Buf[nLength] = '\0';
}

bool CNmStr::Delete(int pos, int count)
{
	char *buf = m_Buf;
	char *newBuf;
	char *newBufPtr;
	int  lenOld = strlen(m_Buf);
	int  lenNew = lenOld - count;

	newBuf    = new char[lenNew + 1];
	newBufPtr = newBuf;

	memcpy(newBufPtr, buf, pos);

	buf += pos + count;

	newBufPtr += pos;

	memcpy(newBufPtr, buf, lenOld - pos - count);

	buf = m_Buf;

	m_Buf = newBuf;

	free(buf);

	m_Buf[lenNew] = '\0';

	return true;
}

int CNmStr::Find(char c)
{
	char *res = strchr(m_Buf, c);

	if (res != NULL)
	{
		return res - m_Buf;
	}

	return -1;
}

void CNmStr::Format(const char *buffer, ...)
{
	va_list argptr;
	char    *buf = new char[1024];

	va_start(argptr, buffer);
	vsprintf(buf, buffer, argptr);
	va_end(argptr);

	free(m_Buf);

	m_Buf = _strdup(buf);

	free(buf);
}

bool CNmStr::FromFile(const char *filename)
{
	free(m_Buf);

	if ((!Nm_fexist(filename)) || (Nm_LoadFile(filename, (void **)&m_Buf) == -1))
	{
		m_Buf = _strdup("");
		return false;
	}

	return true;
}

char CNmStr::GetAt(int pos)
{
	return m_Buf[pos];
}

void CNmStr::SetAt(int pos, char c)
{
	m_Buf[pos] = c;
}

int CNmStr::Find(char c, int start)
{
	char *res = m_Buf + start;

	res = strchr(res, c);

	if (res != NULL)
	{
		return res - m_Buf;
	}

	return -1;
}

int CNmStr::Find(const char *s)
{
	char *res = strstr(m_Buf, s);

	if (res != NULL)
	{
		return res - m_Buf;
	}

	return -1;
}

const char *CNmStr::GetBuffer()
{
	return (const char *)m_Buf;
}

int CNmStr::GetLength()
{
	return strlen(m_Buf);
}

int CNmStr::Insert(int pos, const char *szBuf)
{
	char *szNew;
	char *pchrPos;
	int  dwBufLen;
	int  dwLen;

	pchrPos = &m_Buf[pos];

	dwBufLen = strlen(szBuf);
	dwLen    = strlen(m_Buf);

	szNew = new char[dwLen + dwBufLen + 1];

	memcpy(szNew, m_Buf, pos);
	memcpy(szNew + pos, szBuf, dwBufLen);
	memcpy(szNew + pos + dwBufLen, pchrPos, dwLen - pos);

	szNew[dwLen + dwBufLen] = '\0';

	delete[] m_Buf;

	m_Buf = szNew;

	return dwLen + dwBufLen;
}

CNmStr CNmStr::Left(int count)
{
	CNmStr str;

	str.Alloc(count);
	str.Set(m_Buf, count);

//	str.m_Buf = new char[ count + 1 ];
//	memcpy( str.m_Buf, m_Buf, count );
//	str.m_Buf[ count ] = '\0';

	return str;
}

int CNmStr::ReverseFind(char c)
{
	char *res = strrchr(m_Buf, c);

	if (res != NULL)
	{
		return res - m_Buf;
	}

	return -1;
}

int CNmStr::ReverseFind(char c, int start)
{
	char *res = m_Buf + start;

	while (res >= m_Buf && *res != c)
	{
		res--;
	}

	if (res >= m_Buf)
	{
		return res - m_Buf;
	}

	return -1;
}

CNmStr CNmStr::Right(int count)
{
	CNmStr str;
	int    k  = strlen(m_Buf) - count;
	char   *c = (char *)(int(m_Buf) + k);

	str.Alloc(count);
	str.Set(c, count);

	return str;
}

CNmStr CNmStr::Mid(int start, int count)
{
	CNmStr str;
	char   *c = (char *)(int(m_Buf) + start);

	str.Alloc(count);
	str.Set(c, count);

	return str;
}

void CNmStr::MakeLower()
{
	_strlwr(m_Buf);
}

void CNmStr::MakeUpper()
{
	_strupr(m_Buf);
}

void CNmStr::MakeReverse()
{
	int  len  = strlen(m_Buf);
	int  last = len - 1;
	char c;

	for (int i = 0; i < len / 2; i++)
	{
		c           = m_Buf[i];
		m_Buf[i]    = m_Buf[last];
		m_Buf[last] = c;

		last--;
	}
}

CNmStr& CNmStr::operator=(const CNmStr& s)
{
	*this = s.m_Buf;

	return *this;
}

CNmStr& CNmStr::operator=(const char *s)
{
	free(m_Buf);

	m_Buf = _strdup(s);

	return *this;
}

CNmStr& CNmStr::operator=(char c)
{
	free(m_Buf);

	m_Buf = new char[2];

	m_Buf[0] = c;
	m_Buf[1] = '\0';

	return *this;
}

char CNmStr::operator[](int pos)
{
	return m_Buf[pos];
}

CNmStr& CNmStr::operator+=(const CNmStr& s)
{
	*this += s.m_Buf;

	return *this;
}

CNmStr& CNmStr::operator+=(const char *s)
{
	int  len = strlen(m_Buf) + strlen(s);
	char *buf;

	buf = new char[len + 1];

	strcpy(buf, m_Buf);
	strcat(buf, s);

	free(m_Buf);

	m_Buf = buf;

	return *this;
}

CNmStr& CNmStr::operator+=(char c)
{
	int  len = strlen(m_Buf);
	char *buf;

	buf = new char[len + 2];    // c '\0'

	memcpy(buf, m_Buf, len);
	buf[len]     = c;
	buf[len + 1] = '\0';

	free(m_Buf);

	m_Buf = buf;

	return *this;
}

int CNmStr::Replace(const char *find, const char *replace)
{
	bool done;
	int  occurences;
	int  lenNew;
	int  lenFind;
	int  lenReplace;
	char *buf;
	char *newBuf;
	char *newBufPtr;
	char *c;

	lenNew     = strlen(m_Buf);
	lenFind    = strlen(find);
	lenReplace = strlen(replace);

	buf  = m_Buf;
	done = false;

	occurences = 0;

	while (!done)
	{
		c = strstr(buf, find);      // Search for 'find'

		if (c != 0)   // occurence found
		{
			occurences++;
			lenNew = lenNew - lenFind + lenReplace;
			buf    = c + lenFind;
		}
		else
		{
			done = true;
		}
	}

	if (occurences == 0)
	{
		return 0;
	}

	newBuf    = new char[lenNew + 1];   // allocate mem for new m_Buf
	newBufPtr = newBuf;

	buf  = m_Buf;
	done = false;

	while (!done)
	{
		c = strstr(buf, find);      // Search for 'find'

		if (c != 0)   // occurence found
		{
			memcpy(newBufPtr, buf, c - buf);
			newBufPtr += c - buf;
			memcpy(newBufPtr, replace, lenReplace);
			buf        = c + lenFind;
			newBufPtr += lenReplace;
		}
		else
		{
			memcpy(newBufPtr, buf, strlen(buf));
			done = true;
		}
	}

	newBuf[lenNew] = '\0';

	c = m_Buf;

	m_Buf = newBuf;

	free(c);

	return occurences;
}

int CNmStr::Replace(char find, char replace)
{
	int  occ = 0;
	char *c;
	char *p = m_Buf;

	c = strchr(p, find);

	while (c != 0)
	{
		occ++;

		*c = replace;

		p = c;

		c = strchr(p, find);
	}

	return occ;
}

//
// Cuts away starting ' ','\r','\n','\t'
//
void CNmStr::TrimLeft()
{
	char *buf = m_Buf;

	while ((*buf != '\0') && ((*buf == ' ') || (*buf == '\r') || (*buf == '\n') || (*buf == '\t')))
	{
		buf++;
	}

	if (buf == m_Buf)
	{
		return;
	}

	char *newBuf = new char[strlen(buf) + 1];

	strcpy(newBuf, buf);

	free(m_Buf);

	m_Buf = newBuf;
}

void CNmStr::TrimLeft(char c)
{
	char *buf = m_Buf;

	while ((*buf != '\0') && (*buf == c))
	{
		buf++;
	}

	if (buf == m_Buf)
	{
		return;
	}

	char *newBuf = new char[strlen(buf) + 1];

	strcpy(newBuf, buf);

	free(m_Buf);

	m_Buf = newBuf;
}

void CNmStr::TrimLeft(const char *setc)
{
	char *buf = m_Buf;

	while ((*buf != '\0') && (strchr(setc, *buf) != NULL))
	{
		buf++;
	}

	if (buf == m_Buf)
	{
		return;
	}

	char *newBuf = new char[strlen(buf) + 1];

	strcpy(newBuf, buf);

	free(m_Buf);

	m_Buf = newBuf;
}

//
// Cuts away ending ' ','\r','\n','\t'
//
void CNmStr::TrimRight()
{
	char *buf = m_Buf;

	while (*buf != '\0')
		buf++;                  // position pointer to over ending \0
	buf--;          // one step back to position over last valid charachter (not \0)

	while ((buf >= m_Buf) && ((*buf == ' ') || (*buf == '\r') || (*buf == '\n') || (*buf == '\t')))
	{
		buf--;
	}

	if (*(buf + 1) == '\0')
	{
		return;
	}

	char *newBuf = new char[buf - m_Buf + 2];

	memcpy(newBuf, m_Buf, buf - m_Buf + 1);
	newBuf[buf - m_Buf + 1] = '\0';

	free(m_Buf);

	m_Buf = newBuf;
}

void CNmStr::TrimRight(char c)
{
	char *buf = m_Buf;

	while (*buf != '\0')
		buf++;                  // position pointer to over ending \0
	buf--;          // one step back to position over last valid charachter (not \0)

	while ((buf >= m_Buf) && (*buf == c))
	{
		buf--;
	}

	if (*(buf + 1) == '\0')
	{
		return;
	}

	char *newBuf = new char[buf - m_Buf + 2];

	memcpy(newBuf, m_Buf, buf - m_Buf + 1);
	newBuf[buf - m_Buf + 1] = '\0';

	free(m_Buf);

	m_Buf = newBuf;
}

void CNmStr::TrimRight(const char *setc)
{
	char *buf = m_Buf;

	while (*buf != '\0')
		buf++;                  // position pointer to over ending \0
	buf--;          // one step back to position over last valid charachter (not \0)

	while ((buf >= m_Buf) && (strchr(setc, *buf) != NULL))
	{
		buf--;
	}

	if (*(buf + 1) == '\0')
	{
		return;
	}

	char *newBuf = new char[buf - m_Buf + 2];

	memcpy(newBuf, m_Buf, buf - m_Buf + 1);
	newBuf[buf - m_Buf + 1] = '\0';

	free(m_Buf);

	m_Buf = newBuf;
}

void CNmStr::TestMsg()
{
//	MessageBox(0, m_Buf, ":-)", MB_OK);
}
