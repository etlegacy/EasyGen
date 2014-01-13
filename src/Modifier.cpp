// Modifier.cpp: implementation of the CModifier class.
//
//////////////////////////////////////////////////////////////////////

#include "EasyGen.h"
#include "Modifier.h"

#include "Grid.h"
#include "Nemo.h"

// 0..119 reserved to vars

#define     I_PX    120
#define     I_PY    121
#define     I_PZ    122
#define     I_NEWZ  123
#define     I_MX    124
#define     I_MY    125
#define     I_MH    126
#define     I_MR    127
#define     I_MAXZ  128
#define     I_MINZ  129
#define     I_PI    130


#define pigreco 3.1415926535897932384626433832795f

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CModifier::CModifier()
{
	S_MODIFIER *m;

	m_x  = 0;
	m_y  = 0;
	m_r  = 0.1f;
	m_rx = 0;
	m_ry = 0;
	m_h  = 0;

	m_NumModifiers = 0;

	m_CurrentModifier = -1;

	for (int i = 0; i < MODIFIERS_MAX; i++)
	{
		m = GetModifier(i);

		m->code = NULL;
	}
}

/*
CModifier::CModifier(LPCTSTR folder)
{
    CModifier();

    LoadFolder(folder);
}
*/

CModifier::~CModifier()
{
	S_MODIFIER *m;

	for (int i = 0; i < MODIFIERS_MAX; i++)
	{
		m = GetModifier(i);

		if (m->code != NULL)
		{
			delete m->code;
		}
	}
}

S_MODIFIER *CModifier::Add(CString nm, void *cd, int size)
{
	if (m_NumModifiers == MODIFIERS_MAX)
	{
		AfxMessageBox("Modifiers exceeded");
		return NULL;
	}

	S_MODIFIER *m = GetModifier(m_NumModifiers++);

	m->name  = nm;
	m->flags = 0;
	m->code  = (unsigned char *)malloc(size);
	memcpy(m->code, cd, size);

	return m;
}

void CModifier::Modify(CGrid *Grid, int mode)
{
	CNmVec2 pA, pB;
	int     i_start, i_end, j_start, j_end;

	int   i, j, k;
	float *z;
	float newz;
	float maxh = Grid->GetMaxHeight();
	float minh = Grid->GetMinHeight();

	srand(UINT((m_x - (float)floor(m_x) + m_y - (float)floor(m_y)) * 100));

	S_MODIFIER *m = GetCurrentModifier();

	if ((m->flags & MOD_FLAG_SQUARERADIUS) || (m->flags & MOD_FLAG_RADIUS))
	{
		pB.Set(m_x + m_r, m_y + m_r);
		pA.Set(m_x - m_r, m_y - m_r);

		Grid->GetIndexBounds(pA, pB, j_start, j_end, i_start, i_end);

		Grid->ScanRectSetup(i_start, j_start, i_end, j_end);

		while (Grid->ScanRect(i, j, k, z))
		{
			if ((m->flags & MOD_FLAG_RADIUS) &&
			    (distance(m_x, m_y, Grid->GetX(i), Grid->GetY(j)) > m_r)
			    )
			{
				continue;
			}

			m_Compiler.SetMemoryAt(I_PX, Grid->GetX(i));
			m_Compiler.SetMemoryAt(I_PY, Grid->GetY(j));
			m_Compiler.SetMemoryAt(I_PZ, *z);
			m_Compiler.SetMemoryAt(I_MX, m_x);
			m_Compiler.SetMemoryAt(I_MY, m_y);
			m_Compiler.SetMemoryAt(I_MH, m_h);
			m_Compiler.SetMemoryAt(I_MR, m_r);
			m_Compiler.SetMemoryAt(I_MAXZ, maxh);
			m_Compiler.SetMemoryAt(I_MINZ, minh);
			m_Compiler.SetMemoryAt(I_PI, pigreco);

			if (!m_Compiler.Exec())
			{
				continue;
			}

			newz = m_Compiler.GetMemoryAt(I_NEWZ);

			switch (mode)
			{
			case (MOD_MODE_MAX):
				*z > newz ? *z = *z : *z = newz;
				break;
			case (MOD_MODE_ADD):
				*z += newz;
				break;
			case (MOD_MODE_IDENT):
				*z = newz;
				break;
			}

			if (*z < ASSUME_ZERO)
			{
				*z = 0.0f;
			}
		}
	}
	else
	{
		Grid->ScanFullSetup();

		while (Grid->ScanFull(i, j, k, z))
		{
			m_Compiler.SetMemoryAt(I_PX, Grid->GetX(i));
			m_Compiler.SetMemoryAt(I_PY, Grid->GetY(j));
			m_Compiler.SetMemoryAt(I_PZ, *z);
			m_Compiler.SetMemoryAt(I_MX, m_x);
			m_Compiler.SetMemoryAt(I_MY, m_y);
			m_Compiler.SetMemoryAt(I_MH, m_h);
			m_Compiler.SetMemoryAt(I_MR, m_r);
			m_Compiler.SetMemoryAt(I_MAXZ, maxh);
			m_Compiler.SetMemoryAt(I_MINZ, minh);
			m_Compiler.SetMemoryAt(I_PI, pigreco);

			if (!m_Compiler.Exec())
			{
				continue;
			}

			newz = m_Compiler.GetMemoryAt(I_NEWZ);

			switch (mode)
			{
			case (MOD_MODE_MAX):
				*z > newz ? *z = *z : *z = newz;
				break;
			case (MOD_MODE_ADD):
				*z += newz;
				break;
			case (MOD_MODE_IDENT):
				*z = newz;
				break;
			}

			if (*z < ASSUME_ZERO)
			{
				*z = 0.0f;
			}
		}
	}
}

void CModifier::SelectModifier(int mod)
{
	if (mod >= m_NumModifiers)
	{
		AfxMessageBox("mod select error");
		return;
	}

	S_MODIFIER *m = GetModifier(mod);

	m_Compiler.CodeLoad(m->code);

	m_CurrentModifier = mod;
}

BOOL CModifier::GetSourceBody(CString &buffer)
{
	int i = buffer.Find("beginCODE");

	if (i == -1)
	{
		return FALSE;
	}

	buffer.Delete(0, i + strlen("beginCODE"));

	i = buffer.Find("endCODE");

	if (i == -1)
	{
		return FALSE;
	}

	buffer.Delete(i, strlen("endCODE"));

	return TRUE;
}

//	mx,my	: modifier pos
//	mh		: modifier height
//	mr		: modifier radius
//	x,y,z	: coordinate of the point to modify
//	newz	: new calculated z
//
//
void CModifier::PreProcessBody(CString &body)
{
	CString s;

	s.Format("[%d]", I_MX);
	body.Replace("MX", s);

	s.Format("[%d]", I_MY);
	body.Replace("MY", s);

	s.Format("[%d]", I_MH);
	body.Replace("MH", s);

	s.Format("[%d]", I_MR);
	body.Replace("MR", s);

	s.Format("[%d]", I_NEWZ);
	body.Replace("NEWZ", s);

	s.Format("[%d]", I_PX);
	body.Replace("PX", s);

	s.Format("[%d]", I_PY);
	body.Replace("PY", s);

	s.Format("[%d]", I_PZ);
	body.Replace("PZ", s);

	s.Format("[%d]", I_MAXZ);
	body.Replace("MAXZ", s);

	s.Format("[%d]", I_MINZ);
	body.Replace("MINZ", s);

	s.Format("[%d]", I_PI);
	body.Replace("PI", s);
}

void CModifier::SetMode(int mode)
{
//	m_mode = mode;
}

void CModifier::MoveDelta(float dx, float dy)
{
	m_x += dx;
	m_y += dy;
}

void CModifier::HeightSet(float h)
{
	m_h = h;
}

void CModifier::HeightDelta(float dh)
{
	m_h += dh;
}

bool CModifier::LoadFolder(LPCTSTR folder)
{
	CString mask, s;
	CString sfolder;
	CString filename;
	CString name;
	CString code;

	sfolder = folder;

	mask  = folder;
	mask += "*.mdf";

	WIN32_FIND_DATA data;
	HANDLE          hFind = FindFirstFile(_T((LPCTSTR)mask), &data);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (lstrcmp(data.cFileName, _T(".")) == 0 ||
			    lstrcmp(data.cFileName, _T("..")) == 0 ||
			    (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				continue;
			}

			filename  = sfolder;
			filename += data.cFileName;

			name = data.cFileName;
			name.Delete(name.ReverseFind('.'), name.GetLength() - name.ReverseFind('.'));

			// process filename here
			char *buf;

			CString src_body, setup_body;

			if (Nm_LoadFile(filename, (void **)&buf) > 0)
			{
				setup_body = buf;
				src_body   = buf;
				delete[] buf;
			}
			else
			{
				TRACE("Error reading %s", filename);
				continue;
			}

			if (!GetSourceBody(src_body))
			{
				m_Compiler.Log("Source body not found in : ", (LPCTSTR)filename);
			}
			else
			{

				//
				// substitute rx -> M0 , ry -> M1 etc etc
				//
				PreProcessBody(src_body);

				if (m_Compiler.Compile(src_body))
				{

					//debug
/*
                    m_Compiler.CodeDump("c:\\dump.txt");
                    m_Compiler.Exec();
                    s = name + "\tMemory At 0: ";
                    m_Compiler.Log(s, m_Compiler.GetMemoryAt(0));
                    s = name + "\tMemory At 1: ";
                    m_Compiler.Log(s, m_Compiler.GetMemoryAt(1));
*/
					//debug

					S_MODIFIER *added;

					added = Add(name, m_Compiler.GetCodePtr(), m_Compiler.GetCodeSize());

					if (!GetSetupBody(setup_body))
					{
						m_Compiler.Log("Setup body not found in : ", (LPCTSTR)filename);
					}
					else if (added != NULL)
					{
						SetupModifier(added, setup_body);
					}

				}
			}

			//
		}
		while (FindNextFile(hFind, &data));
		FindClose(hFind);
	}

	return true;
}

void CModifier::RadiusSet(float r)
{
	m_r = r;
}

void CModifier::RadiusDelta(float dr)
{
	m_r += dr;

	if (m_r < 0)
	{
		m_r = 0;
	}
}

int CModifier::GetNumModifiers()
{
	return m_NumModifiers;
}

void CModifier::GetModifierName(int num, CString &name)
{
	if (num >= m_NumModifiers)
	{
		name = "mod out of range";
		return;
	}

	S_MODIFIER *m = GetModifier(num);

	name = m->name;
}

void CModifier::DrawGL()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3f(1.f, 0.f, 0.f);

	glCube(
	    m_x,
	    m_y,
	    0.f,
	    m_r * 2,
	    m_r * 2,
	    m_h
	    );

}


S_MODIFIER *CModifier::GetModifier(int num)
{
	return &m_Modifiers[num];
}

S_MODIFIER *CModifier::GetCurrentModifier()
{
	return &m_Modifiers[m_CurrentModifier];
}

BOOL CModifier::GetSetupBody(CString &buffer)
{
	int i = buffer.Find("beginSETUP");

	if (i == -1)
	{
		return FALSE;
	}

	buffer.Delete(0, i + strlen("beginSETUP"));

	i = buffer.Find("endSETUP");

	if (i == -1)
	{
		return FALSE;
	}

	buffer.Delete(i, strlen("endSETUP"));

	return TRUE;
}

void CModifier::SetupBodyPurge(CString &setup)
{
	BOOL done;
	int  i, j;

	setup += "\r\n";

	setup.Replace("\t", ""); // tab cut
	while (setup.Replace("  ", " "))
		;                               // redundant spaces cut
	setup.Replace("\n", "");     // newline cut
	while (setup.Replace("\r\r", "\r"))
		;

	// '=' are attractive
	while (setup.Replace(" =", "="))
		;
	while (setup.Replace("= ", "="))
		;

	setup.TrimLeft();

	// cut away commented lines (start with //)
	done = FALSE;
	while (!done)
	{
		i = setup.Find("//");

		if (i != -1)
		{
			j = setup.Find('\r', i);
			setup.Delete(i, j - i + 1);
		}
		else
		{
			done = TRUE;
		}
	}
}

// setup_body : line1\rline2\r
//
//
BOOL CModifier::SetupModifier(S_MODIFIER *m, CString setup_body)
{
	CString s = setup_body;
	CString q;

	int i, j;

	m->flags = 0;

	if (SetupGetbValue(setup_body, "fl_modeAdd"))
	{
		m->flags |= MOD_FLAG_ADD;
	}
	if (SetupGetbValue(setup_body, "fl_modeCompare"))
	{
		m->flags |= MOD_FLAG_MAX;
	}
	if (SetupGetbValue(setup_body, "fl_modeForce"))
	{
		m->flags |= MOD_FLAG_IDENT;
	}
	if (SetupGetbValue(setup_body, "fl_squareRadius"))
	{
		m->flags |= MOD_FLAG_SQUARERADIUS;
	}
	if (SetupGetbValue(setup_body, "fl_Radius"))
	{
		m->flags |= MOD_FLAG_RADIUS;
	}
	if (SetupGetbValue(setup_body, "fl_positiveHeight"))
	{
		m->flags |= MOD_FLAG_POSITIVEHEIGHT;
	}

	i = s.Find("description");

	if (i != -1)
	{
		j = s.Find('=', i);

		i = s.Find('\r', j);

		q = s.Mid(j + 1, i - j - 1);

		q.TrimLeft();
		q.TrimRight();

		q.Replace("\\n", "\r\n");

		m->description = q;
	}
	else
	{
		m->description = "";
	}

	return TRUE;
}

void CModifier::PositionSet(float x, float y)
{
	m_x = x;
	m_y = y;
}

float CModifier::GetRadius()
{
	return m_r;
}

float CModifier::GetHeight()
{
	return m_h;
}

UINT CModifier::GetFlags()
{
	S_MODIFIER *m = GetCurrentModifier();

	return m->flags;
}

BOOL CModifier::SetupGetbValue(CString s, CString key)
{
	CString line;
	int     i, j;
	char    nums[3] = "01";

	i = s.Find(key);

	if (i != -1)
	{
		j = s.Find('=', i);

		i = s.Find('\r', j);

		line = s.Mid(j + 1, i - j - 1);

		i = line.FindOneOf(nums);

		if (i != -1)
		{
			return line.GetAt(i) == '1';
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}
