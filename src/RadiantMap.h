#ifndef RADIANTMAP_H
#define RADIANTMAP_H

#include "Nemo.h"
#include "NmParser.h"
#include "NmStr.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRadiantMap
{
public:
	CRadiantMap();
	virtual ~CRadiantMap();

	bool        Open(const char *filename);
	bool        IsOpen();
	void        Close(bool andSave = false);

	bool        ent_GetNext();
	bool        ent_GetValue(const char *szKey, CNmStr& value);
	bool        ent_GetKey(const char *szValue, CNmStr& key);
	bool        ent_SetValue(const char *szKey, const char *szValue);
	bool        ent_Update();

	const char *ent_GetPtr();

private:
	CNmParser m_Parser;
	CNmStr    m_sFilename;
	CNmStr    m_curEntity;

	char *m_pchrStartEnt;
	char *m_pchrEndEnt;

	int         ent_FindKey(const char *szKey);
	int         ent_FindValue(const char *szValue, int& dwKeyStart);
};

#endif
