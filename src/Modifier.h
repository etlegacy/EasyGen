// Modifier.h: interface for the CModifier class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MODIFIER_H__CECAF32C_D59E_4737_8A03_030F1548C79D__INCLUDED_)
#define AFX_MODIFIER_H__CECAF32C_D59E_4737_8A03_030F1548C79D__INCLUDED_

#include "Compiler.h"   // Added by ClassView

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGrid;

#define MODIFIERS_MAX   128

struct S_MODIFIER
{
	CString name;
	CString description;
	void *code;
	UINT flags;
};

class CModifier
{
public:
	BOOL SetupGetbValue(CString setup_body, CString key);
	UINT GetFlags();
	float GetHeight();
	float GetRadius();
	void PositionSet(float x, float y);
	BOOL SetupModifier(S_MODIFIER *m, CString setup_body);
	void SetupBodyPurge(CString & setup);
	BOOL GetSetupBody(CString&buffer);
	S_MODIFIER *GetCurrentModifier();
	S_MODIFIER *GetModifier(int num);
	void DrawGL();
	void GetModifierName(int num, CString&name);
	int GetNumModifiers();
	void RadiusDelta(float dr);
	void RadiusSet(float r);
	float m_r;
	CModifier();
//	CModifier(LPCTSTR folder);
	virtual ~CModifier();

	int        m_CurrentModifier;
	int        m_NumModifiers;
	S_MODIFIER m_Modifiers[MODIFIERS_MAX];

	float m_x;
	float m_y;

	float m_rx;
	float m_ry;

	float m_h;

	CCompiler m_Compiler;
	void PreProcessBody(CString & body);
	BOOL GetSourceBody(CString& buffer);

	void SelectModifier(int mod);
	void Modify(CGrid *Grid, int mode);
	S_MODIFIER *Add(CString nm, void *cd, int size);

	void HeightSet(float h);
	void HeightDelta(float dh);

	void MoveDelta(float dx, float dy);
	void SetMode(int mode);

	bool LoadFolder(LPCTSTR folder);
};

#endif // !defined(AFX_MODIFIER_H__CECAF32C_D59E_4737_8A03_030F1548C79D__INCLUDED_)
