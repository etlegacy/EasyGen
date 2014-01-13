////////////////
// NMLIST.CPP //
////////////////
#include "NmList.h"

////////

#include <stdlib.h>

////////

CNmList::CNmList()
{
	m_Head = 0;
	m_Tail = 0;
}

CNmList::~CNmList()
{
	RemoveAll();
}

void CNmList::RemoveAll()
{
	while (m_Head)
		_Remove(m_Head);
}

void CNmList::ScanReset()
{
	m_Scan = m_Head;
}

bool CNmList::Scan(S_LIST *item)
{
	bool res = false;

	if (m_Scan)
	{
		res    = true;
		item   = m_Scan;
		m_Scan = m_Scan->next;
	}

	return res;
}

S_LIST *CNmList::_AddHead()
{
	S_LIST *nitem = new S_LIST;

	if (m_Head == 0)
	{
		nitem->next = 0;
		nitem->prev = 0;

		m_Head = nitem;
		m_Tail = nitem;

		return nitem;
	}

	nitem->next = m_Head;
	nitem->prev = 0;

	m_Head->prev = nitem;
	m_Head       = nitem;

	return nitem;
}

S_LIST *CNmList::_AddTail()
{
	if (m_Tail == 0)
	{
		return _AddHead();
	}

	S_LIST *nitem = new S_LIST;

	nitem->next = 0;
	nitem->prev = m_Tail;

	m_Tail->next = nitem;
	m_Tail       = nitem;

	return nitem;
}

S_LIST *CNmList::_AddAfter(S_LIST *item)
{
	if (item == m_Tail)
	{
		return _AddTail();
	}

	S_LIST *nitem = new S_LIST;

	nitem->next = item->next;
	nitem->prev = item;

	item->next->prev = nitem;
	item->next       = nitem;

	return nitem;
}

S_LIST *CNmList::_AddBefore(S_LIST *item)
{
	if (item == m_Head)
	{
		return _AddHead();
	}

	S_LIST *nitem = new S_LIST;

	nitem->next = item;
	nitem->prev = item->prev;

	item->prev->next = nitem;
	item->prev       = nitem;

	return nitem;
}

bool CNmList::_Remove(S_LIST *item)
{
	if (item == 0)
	{
		return false;
	}

	if ((m_Head == m_Tail) && (m_Head == item))
	{
		m_Head = 0;
		m_Tail = 0;
	}
	else if (item == m_Head)
	{
		m_Head       = m_Head->next;
		m_Head->prev = 0;
	}
	else if (item == m_Tail)
	{
		m_Tail       = m_Tail->prev;
		m_Tail->next = 0;
	}
	else    // generic
	{
		item->next->prev = item->prev;
		item->prev->next = item->next;
	}

	delete item;

	return true;
}

////////////////////
// LIST OF STRINGS
////////////////////

CNmListStr::CNmListStr()
{
	m_Num  = 0;
	m_Head = 0;
	m_Tail = 0;
}

CNmListStr::~CNmListStr()
{
	RemoveAll();
}

bool CNmListStr::Find(CNmStr& s1)
{
	return (_Find(s1) != 0);
}

void CNmListStr::FromString(const char *s, char separator)
{
	CNmStr     str;
	int        pos;
	S_LIST_STR *item;

	RemoveAll();

	if (s == NULL)
	{
		return;
	}

	str = s;

	if (str.GetAt(str.GetLength() - 1) != separator)
	{
		str = str + separator;
	}

	pos = str.Find(separator);

	while (pos != -1)
	{
		item = _AddTail();

		item->s = str.Left(pos);

		str.Delete(0, pos + 1);

		pos = str.Find(separator);
	}
}

int CNmListStr::GetNum()
{
	return m_Num;
}

CNmStr CNmListStr::GetAt(int pos)
{
	S_LIST_STR *item = m_Head;

	while (pos)
	{
		item = item->next;
		pos--;
	}

	return item->s;
}

void CNmListStr::SetAt(int pos, CNmStr& s1)
{
	S_LIST_STR *item = m_Head;

	while (pos)
	{
		item = item->next;
		pos--;
	}

	item->s = s1;
}

void CNmListStr::RemoveAll()
{
	while (m_Head)
		_Remove(m_Head);
}

void CNmListStr::ScanReset()
{
	m_Scan = m_Head;
}

bool CNmListStr::Scan(CNmStr& s1)
{
	bool res = false;

	if (m_Scan)
	{
		res    = true;
		s1     = m_Scan->s;
		m_Scan = m_Scan->next;
	}

	return res;
}

S_LIST_STR *CNmListStr::_AddHead()
{
	S_LIST_STR *nitem = new S_LIST_STR;

	m_Num++;

	if (m_Head == 0)
	{
		nitem->next = 0;
		nitem->prev = 0;

		m_Head = nitem;
		m_Tail = nitem;

		return nitem;
	}

	nitem->next = m_Head;
	nitem->prev = 0;

	m_Head->prev = nitem;
	m_Head       = nitem;

	return nitem;
}

S_LIST_STR *CNmListStr::_AddTail()
{
	if (m_Tail == 0)
	{
		return _AddHead();
	}

	S_LIST_STR *nitem = new S_LIST_STR;

	m_Num++;

	nitem->next = 0;
	nitem->prev = m_Tail;

	m_Tail->next = nitem;
	m_Tail       = nitem;

	return nitem;
}

S_LIST_STR *CNmListStr::_Find(CNmStr& s1)
{
	for (S_LIST_STR *p = m_Head; p; p = p->next)
	{
		if (p->s == s1)
		{
			return p;
		}
	}

	return 0;
}

S_LIST_STR *CNmListStr::_AddAfter(S_LIST_STR *item)
{
	if (item == m_Tail)
	{
		return _AddTail();
	}

	S_LIST_STR *nitem = new S_LIST_STR;

	m_Num++;

	nitem->next = item->next;
	nitem->prev = item;

	item->next->prev = nitem;
	item->next       = nitem;

	return nitem;
}

S_LIST_STR *CNmListStr::_AddBefore(S_LIST_STR *item)
{
	if (item == m_Head)
	{
		return _AddHead();
	}

	S_LIST_STR *nitem = new S_LIST_STR;

	m_Num++;

	nitem->next = item;
	nitem->prev = item->prev;

	item->prev->next = nitem;
	item->prev       = nitem;

	return nitem;
}

bool CNmListStr::_Remove(S_LIST_STR *item)
{
	if (item == 0)
	{
		return false;
	}

	if ((m_Head == m_Tail) && (m_Head == item))
	{
		m_Head = 0;
		m_Tail = 0;
	}
	else if (item == m_Head)
	{
		m_Head       = m_Head->next;
		m_Head->prev = 0;
	}
	else if (item == m_Tail)
	{
		m_Tail       = m_Tail->prev;
		m_Tail->next = 0;
	}
	else    // generic
	{
		item->next->prev = item->prev;
		item->prev->next = item->next;
	}

	delete item;

	m_Num--;

	return true;
}

bool CNmListStr::AddOrdered(CNmStr& s1)
{
	S_LIST_STR *nitem;

	if (m_Head == 0)
	{
		nitem    = _AddHead();
		nitem->s = s1;
		return true;
	}

	nitem = m_Head;

	while (s1 > nitem->s)
	{
		nitem = nitem->next;

		if (nitem == 0)
		{
			nitem    = _AddTail();
			nitem->s = s1;
			return true;
		}
	}

	if (nitem->s == s1)
	{
		return false;
	}

	nitem    = _AddBefore(nitem);
	nitem->s = s1;

	return true;
}

bool CNmListStr::Append(CNmStr& s1)
{
	if (_Find(s1))
	{
		return false;
	}

	S_LIST_STR *item = _AddTail();

	if (!item)
	{
		return false;
	}

	item->s = s1;

	return true;
}

bool CNmListStr::Remove(CNmStr& s1)
{
	S_LIST_STR *item = _Find(s1);

	if (!item)
	{
		return false;
	}

	return _Remove(item);
}

//////////////////
// LIST OF UINTS
//////////////////

CNmListUint::CNmListUint()
{
	m_Num  = 0;
	m_Head = 0;
	m_Tail = 0;
}

CNmListUint::~CNmListUint()
{
	RemoveAll();
}

bool CNmListUint::Find(unsigned int ui)
{
	return (_Find(ui) != 0);
}

int CNmListUint::GetNum()
{
	return m_Num;
}

void CNmListUint::RemoveAll()
{
	while (m_Head)
		_Remove(m_Head);
}

void CNmListUint::ScanReset()
{
	m_Scan = m_Head;
}

bool CNmListUint::Scan(unsigned int& ui)
{
	bool res = false;

	if (m_Scan)
	{
		res    = true;
		ui     = m_Scan->uint;
		m_Scan = m_Scan->next;
	}

	return res;
}

S_LIST_UINT *CNmListUint::_AddHead()
{
	S_LIST_UINT *nitem = new S_LIST_UINT;

	m_Num++;

	if (m_Head == 0)
	{
		nitem->next = 0;
		nitem->prev = 0;

		m_Head = nitem;
		m_Tail = nitem;

		return nitem;
	}

	nitem->next = m_Head;
	nitem->prev = 0;

	m_Head->prev = nitem;
	m_Head       = nitem;

	return nitem;
}

S_LIST_UINT *CNmListUint::_AddTail()
{
	if (m_Tail == 0)
	{
		return _AddHead();
	}

	S_LIST_UINT *nitem = new S_LIST_UINT;

	m_Num++;

	nitem->next = 0;
	nitem->prev = m_Tail;

	m_Tail->next = nitem;
	m_Tail       = nitem;

	return nitem;
}

S_LIST_UINT *CNmListUint::_Find(unsigned int ui)
{
	for (S_LIST_UINT *p = m_Head; p; p = p->next)
	{
		if (p->uint == ui)
		{
			return p;
		}
	}

	return 0;
}

S_LIST_UINT *CNmListUint::_AddAfter(S_LIST_UINT *item)
{
	if (item == m_Tail)
	{
		return _AddTail();
	}

	S_LIST_UINT *nitem = new S_LIST_UINT;

	m_Num++;

	nitem->next = item->next;
	nitem->prev = item;

	item->next->prev = nitem;
	item->next       = nitem;

	return nitem;
}

S_LIST_UINT *CNmListUint::_AddBefore(S_LIST_UINT *item)
{
	if (item == m_Head)
	{
		return _AddHead();
	}

	S_LIST_UINT *nitem = new S_LIST_UINT;

	m_Num++;

	nitem->next = item;
	nitem->prev = item->prev;

	item->prev->next = nitem;
	item->prev       = nitem;

	return nitem;
}

bool CNmListUint::_Remove(S_LIST_UINT *item)
{
	if (item == 0)
	{
		return false;
	}

	if ((m_Head == m_Tail) && (m_Head == item))
	{
		m_Head = 0;
		m_Tail = 0;
	}
	else if (item == m_Head)
	{
		m_Head       = m_Head->next;
		m_Head->prev = 0;
	}
	else if (item == m_Tail)
	{
		m_Tail       = m_Tail->prev;
		m_Tail->next = 0;
	}
	else    // generic
	{
		item->next->prev = item->prev;
		item->prev->next = item->next;
	}

	delete item;

	m_Num--;

	return true;
}

bool CNmListUint::Add(unsigned int ui)
{
	S_LIST_UINT *nitem;

	if (m_Head == 0)
	{
		nitem       = _AddHead();
		nitem->uint = ui;
		return true;
	}

	nitem = m_Head;

	while (ui > nitem->uint)
	{
		nitem = nitem->next;

		if (nitem == 0)
		{
			nitem       = _AddTail();
			nitem->uint = ui;
			return true;
		}
	}

	if (nitem->uint == ui)
	{
		return false;
	}

	nitem       = _AddBefore(nitem);
	nitem->uint = ui;

	return true;
}

unsigned int CNmListUint::PickOne()
{
	S_LIST_UINT  *item = m_Head;
	unsigned int i     = 1;

	while (item)
	{
		if (item->uint == i)
		{
			i++;
			item = item->next;
		}
		else if (item->uint > i)
		{
			return i;
		}
	}

	return i;
}

bool CNmListUint::Remove(unsigned int ui)
{
	S_LIST_UINT *item = _Find(ui);

	if (!item)
	{
		return false;
	}

	return _Remove(item);
}