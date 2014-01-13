/////////////
// NMSTR.H //
/////////////

#ifndef NMSTR_H
#define NMSTR_H

class CNmStr
{
public:
	CNmStr();
	CNmStr(const char *);
	CNmStr(const CNmStr&);
	virtual ~CNmStr();

	bool    FromFile(const char *filename);

	// conversion operators
	operator const char *()
	{
		return (const char *) m_Buf;
	}

	// various operators
	friend bool operator==(const CNmStr& s1, const CNmStr& s2);
	friend bool operator==(const CNmStr& s1, const char *s2);
	friend bool operator==(const char *s1, const CNmStr& s2);
	friend bool operator!=(const CNmStr& s1, const CNmStr& s2);
	friend bool operator!=(const CNmStr& s1, const char *s2);
	friend bool operator!=(const char *s1, const CNmStr& s2);
	friend bool operator<(const CNmStr& s1, const CNmStr& s2);
	friend bool operator<(const CNmStr& s1, const char *s2);
	friend bool operator<(const char *s1, const CNmStr& s2);
	friend bool operator<=(const CNmStr& s1, const CNmStr& s2);
	friend bool operator<=(const CNmStr& s1, const char *s2);
	friend bool operator<=(const char *s1, const CNmStr& s2);
	friend bool operator>(const CNmStr& s1, const CNmStr& s2);
	friend bool operator>(const CNmStr& s1, const char *s2);
	friend bool operator>(const char *s1, const CNmStr& s2);
	friend bool operator>=(const CNmStr& s1, const char *s2);
	friend bool operator>=(const CNmStr& s1, const CNmStr& s2);
	friend bool operator>=(const char *s1, const CNmStr& s2);

	CNmStr& operator=(const CNmStr& s);
	CNmStr& operator=(const char *s);
	CNmStr& operator=(char c);
	CNmStr& operator+=(const CNmStr& s);
	CNmStr& operator+=(const char *s);
	CNmStr& operator+=(char c);
	char operator[](int pos);

	// MFC CNmString like functions
	const char *GetBuffer();
	int     GetLength();
	int     Find(char);
	int     Find(char, int);
	int     Find(const char *);
	int     ReverseFind(char);
	void    Format(const char *buffer, ...);
	int     Replace(const char *find, const char *replace);
	int     Replace(char find, char replace);
	bool    Delete(int pos, int count);
	int     Insert(int pos, const char *szBuf);
	CNmStr  Mid(int start, int count);
	CNmStr  Left(int count);
	CNmStr  Right(int count);
	void    TrimLeft();
	void    TrimLeft(char c);
	void    TrimLeft(const char *setc);
	void    TrimRight();
	void    TrimRight(char c);
	void    TrimRight(const char *setc);
	char    GetAt(int pos);
	void    SetAt(int pos, char c);
	void    MakeLower();
	void    MakeUpper();
	void    MakeReverse();

	// My functions
	bool    Alloc(int nLength);
	void    Set(const char *, int nLength); // no length control is performed
	int     ReverseFind(char c, int start);

	// Test functions
	void    TestMsg();
private:
	char *m_Buf;
};

bool operator==(const CNmStr& s1, const CNmStr& s2);
bool operator==(const CNmStr& s1, const char *s2);
bool operator==(const char *s1, const CNmStr& s2);
bool operator!=(const CNmStr& s1, const CNmStr& s2);
bool operator!=(const CNmStr& s1, const char *s2);
bool operator!=(const char *s1, const CNmStr& s2);
bool operator<(const CNmStr& s1, const CNmStr& s2);
bool operator<(const CNmStr& s1, const char *s2);
bool operator<(const char *s1, const CNmStr& s2);
bool operator<=(const CNmStr& s1, const CNmStr& s2);
bool operator<=(const CNmStr& s1, const char *s2);
bool operator<=(const char *s1, const CNmStr& s2);
bool operator>(const CNmStr& s1, const CNmStr& s2);
bool operator>(const CNmStr& s1, const char *s2);
bool operator>(const char *s1, const CNmStr& s2);
bool operator>=(const CNmStr& s1, const CNmStr& s2);
bool operator>=(const CNmStr& s1, const char *s2);
bool operator>=(const char *s1, const CNmStr& s2);
CNmStr operator+(const CNmStr& s1, const CNmStr& s2);
CNmStr operator+(const CNmStr& s1, const char *s2);
CNmStr operator+(const char *s1, const CNmStr& s2);
CNmStr operator+(const CNmStr& s1, char c);
CNmStr operator+(char c, const CNmStr& s2);

#endif
