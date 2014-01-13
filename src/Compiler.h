// Compiler.h: interface for the CCompiler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMPILER_H__2E224708_A818_4813_8ACC_7C17C471C89A__INCLUDED_)
#define AFX_COMPILER_H__2E224708_A818_4813_8ACC_7C17C471C89A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_MEMF        256
#define MAX_CODE        512
#define MAX_LABELS      64
#define MAX_VARS        32
#define MAX_LINE_OPERANDS   10

/*
mov f1..255,f1..255|N


*/

//			IS_opcode_<destM_>source[M|C]		//dest is always mem
//			M=mem C=constant

struct S_ISTR
{
	unsigned char opcode;
	union
	{
		float *m_dst;           // mem float	dest
		float f_dst;            // const float	dest
		int ip;                 // next ip to jump over (used by loop opcodes)
	};
	union
	{
		float *m_src;       // mem	float	source
		float f_src;        // float	constant source
	};
};

struct S_LABEL
{
	CString name;
	int ip;
};

struct S_VAR
{
	CString name;
	int address;        // index of m_memf[]
};

class CCompiler
{
private:
	BOOL c_GetOpcode(CString op, int type, unsigned char & opcode);
	int     c_GetIp();
	void    c_Error(LPCTSTR msg, LPCTSTR where);

	float   c_GetFloatConstant(CString od);
	int     c_GetMemoryIndex(CString od);
	int     c_GetOperandType(CString od);

	BOOL    c_IsLabel(CString line);
	BOOL    c_IsReserved(LPCTSTR res);
	BOOL    c_IsVar(CString od);

	void    c_LabelAdd(LPCTSTR nm, int ip);
	int     c_LabelGetIp(LPCTSTR nm);
	BOOL    c_LabelsLinkIp();

	BOOL    c_Set(CString op);
	BOOL    c_Set(CString op, CString od);
	BOOL    c_Set(CString op, CString od1, CString od2);

	BOOL    c_Set(unsigned char opc);                       //zero-ry
	BOOL    c_Set(unsigned char opc, int dst);               //unary mem
	BOOL    c_Set(unsigned char opc, float dst);         // f_dst used as parameter (srand...)
	BOOL    c_Set(unsigned char opc, int dst, int src);       //binary mem,mem
	BOOL    c_Set(unsigned char opc, int dst, float src); //binary mem,const
	BOOL    c_Set(unsigned char opc, LPCTSTR lbl);

	int     c_VarAdd(LPCTSTR nm);       // return an unused mem address (index of m_memf[])
	S_VAR *c_VarGet(LPCTSTR nm);
	void    c_VarQuery(CString & od);   // translate od into a memory expression [#] if is a var

	BOOL    c_ExLine(CString& line, CString& src);
	BOOL    c_ExOpcode(CString& op, CString& line);
	BOOL    c_ExOperand(CString& od, CString& line);
	BOOL    c_ExLabel(CString& lbl, CString& line);

	BOOL    r_FlagTest(UINT flag);
	void    r_FlagSet(UINT flag);
	void    r_FlagsReset();
public:
	CFile m_logfile;
	BOOL  m_logEnabled;

	int m_numReserved;

	S_ISTR *GetISTR(int num);
	int     GetCodeSize();
	void *GetCodePtr();
	void    CodeLoad(void *code);
	void    CodeDump(LPCTSTR filename);
	void    SetMemoryAt(int index, float f);
	float   GetMemoryAt(int index);
	void    Log(LPCTSTR ls1, LPCTSTR ls2);
	void    Log(LPCTSTR ls, float f);
	void    Log(LPCTSTR ls);
	CCompiler();
	virtual ~CCompiler();

	UINT m_Flags;                       // [runtime]

	float m_memf[MAX_MEMF];             // [compile/runtime] memory (array of floats)

	int    m_IP;                        // [compile/runtime] instruction pointer
	S_ISTR m_Code[MAX_CODE];            // [compile/runtime] runtime code

	CString m_LabelsToLink[MAX_CODE];   // [compile] used to link labels to ip at end of compile process

	int     m_numLabels;                // [compile]
	S_LABEL m_Labels[MAX_LABELS];       // [compile]

	int   m_numVars;                    // [compile] also used to fill memory (m_memf[...])
	S_VAR m_Vars[MAX_VARS];             // [compile] they all are mapped to memory

	void Reset();

	void SourcePurge(CString & source);

	BOOL Compile(CString source);

	BOOL Exec();
protected:
};

#endif // !defined(AFX_COMPILER_H__2E224708_A818_4813_8ACC_7C17C471C89A__INCLUDED_)
