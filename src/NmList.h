//////////////
// NMLIST.H //
//////////////

#ifndef NMLIST_H
#define NMLIST_H

///////////////////
// LIST PROTOTYPE
///////////////////

#include "NmStr.h"

typedef struct S_LIST
{
	S_LIST *next;
	S_LIST *prev;
} S_LIST;

class CNmList
{
public:
	CNmList();
	virtual ~CNmList();

	void    RemoveAll();

	void    ScanReset();
	bool    Scan(S_LIST *item);

private:
	S_LIST *_AddHead();
	S_LIST *_AddTail();
	S_LIST *_AddAfter(S_LIST *item);
	S_LIST *_AddBefore(S_LIST *item);
	bool    _Remove(S_LIST *item);

	S_LIST *m_Head;
	S_LIST *m_Tail;
	S_LIST *m_Scan;
};

///////////////////
// LIST OF STRINGS
///////////////////

typedef struct S_LIST_STR
{
	CNmStr s;
	S_LIST_STR *next;
	S_LIST_STR *prev;
} S_LIST_STR;

class CNmListStr
{
public:
	CNmListStr();
	virtual ~CNmListStr();

	void    RemoveAll();

	bool    AddOrdered(CNmStr& s1);
	bool    Append(CNmStr& s1);
	bool    Remove(CNmStr& s1);

	int     GetNum();
	CNmStr  GetAt(int pos);
	void    SetAt(int pos, CNmStr& s1);

	bool    Find(CNmStr& s1);

	void    ScanReset();
	bool    Scan(CNmStr& s1);

	void    FromString(const char *s, char separator);

private:
	S_LIST_STR *_AddHead();
	S_LIST_STR *_AddTail();
	S_LIST_STR *_AddAfter(S_LIST_STR *item);
	S_LIST_STR *_AddBefore(S_LIST_STR *item);
	S_LIST_STR *_Find(CNmStr& s1);
	bool        _Remove(S_LIST_STR *item);

	S_LIST_STR *m_Head;
	S_LIST_STR *m_Tail;
	S_LIST_STR *m_Scan;
	int        m_Num;
};

//////////////////
// LIST OF UINTS
//////////////////

#define MAX_UNSIGNEDINT 0xffffffff

typedef struct S_LIST_UINT
{
	unsigned int uint;
	S_LIST_UINT *next;
	S_LIST_UINT *prev;
} S_LIST_UINT;

class CNmListUint
{
public:
	CNmListUint();
	virtual ~CNmListUint();

	void    RemoveAll();

	bool    Add(unsigned int ui);
	bool    Remove(unsigned int ui);

	int     GetNum();

	bool    Find(unsigned int ui);
	unsigned int PickOne();

	void    ScanReset();
	bool    Scan(unsigned int& ui);

private:
	S_LIST_UINT *_AddHead();
	S_LIST_UINT *_AddTail();
	S_LIST_UINT *_AddAfter(S_LIST_UINT *item);
	S_LIST_UINT *_AddBefore(S_LIST_UINT *item);
	S_LIST_UINT *_Find(unsigned int ui);
	bool        _Remove(S_LIST_UINT *item);

	S_LIST_UINT *m_Head;
	S_LIST_UINT *m_Tail;
	S_LIST_UINT *m_Scan;
	int         m_Num;
};

#endif