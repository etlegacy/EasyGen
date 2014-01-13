// Compiler.cpp: implementation of the CCompiler class.
//
//////////////////////////////////////////////////////////////////////

#include "EasyGen.h"
#include "Compiler.h"

#define     TYPE_ERR        -1
#define     TYPE_M          0
#define     TYPE_C          1

//internal use
#define     REG_AX          MAX_MEMF - 10
#define     REG_BX          MAX_MEMF - 9

#define     IS_UNDEFINED    0       // undefined
// solo
#define     IS_NOP          1       // nop
#define     IS_HLT          2       // hlt		(Exec() returns TRUE )
#define     IS_ABORT        3       // abort	(Exec() returns FALSE)

// unary	(opcode dst)
#define     IS_ABS_M        10      // fabs (only memory)
#define     IS_SQR_M        11      // sqr	(only memory)
#define     IS_SQRT_M       12      // sqrt	(only memory)
#define     IS_SRAND_C      13      // srand(const)		// init rand
#define     IS_SRAND_M      14      // srand(memory)	// init rand
#define     IS_RAND_M       15      // rand	(only memory)
#define     IS_INV_M        16      // inv	(only memory)
#define     IS_OPP_M        17      // opp	(only memory)
#define     IS_SIN_M        18      // sin
#define     IS_COS_M        19      // cos
#define     IS_ACOS_M       20      // acos

// binary	(opcode dst,src)
#define     IS_MOV_C        101     // mov	(dst,src)
#define     IS_MOV_M        102     //
#define     IS_ADD_C        103     // add	(dst,src)
#define     IS_ADD_M        104     //
#define     IS_SUB_C        105     // sub	(dst,src)
#define     IS_SUB_M        106     //
#define     IS_MUL_C        107     // mul	(dst,src)
#define     IS_MUL_M        108     //
#define     IS_DIV_C        109     // div	(dst,src)
#define     IS_DIV_M        110     //
#define     IS_CMP_C        111     // cmp	(dst,src)
#define     IS_CMP_M        112     //
#define     IS_SUSQ_C       113     // susq (dst,src) : dst = sqr(dst-src)
#define     IS_SUSQ_M       114     //
#define     IS_MAX_C        115     // max (a,b)	:	a = max(a,b)
#define     IS_MAX_M        116     //
#define     IS_MIN_C        117     // min (a,b)	:	a = min(a,b)
#define     IS_MIN_M        118     //

// jumps
#define     ISSTART_JMP     180
#define     ISEND_JMP       199
#define     IS_JMP          180     // jmp	(label)
#define     IS_JE           181     // je	(label)
#define     IS_JL           182     // jl	(label)
#define     IS_JG           183     // jg	(label)
#define     IS_JLE          184     // jle	(label)
#define     IS_JGE          185     // jge	(label)

// extended
#define     ISEX_DIST_M     200     // dist (dst,p1x,p1y,p2x,p2y)

#define     FLAG_ZERO       0x00000001
#define     FLAG_NEGATIVE   0x00000002
#define     FLAG_POSITIVE   0x00000004

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

/*
static CString c_mfcreserved[] =
{
    "nop","hlt",

    "mov","add","sub","mul","div","cmp",

    "jmp","je","jl","jg","jle","jge",

    "dist",

    ""
};
*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCompiler::CCompiler()
{
	m_numReserved = 0;

	int i = 0;

/*	while (c_mfcreserved[i++] != "")
    {
        m_numReserved++;
    }*/

	m_logEnabled = m_logfile.Open("modifiers\\compiler_log.txt", CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive | CFile::typeBinary);
}

CCompiler::~CCompiler()
{
	if (m_logEnabled)
	{
		m_logfile.Close();
	}
}

void CCompiler::Reset()
{
	m_IP = 0;

	while (m_IP < MAX_CODE)
	{
		c_Set(IS_HLT);
	}

	m_IP        = 0;
	m_numLabels = 0;
	m_numVars   = 0;
}

BOOL CCompiler::c_Set(unsigned char opc)
{
	if (m_IP == MAX_CODE)
	{
		return FALSE;
	}

	S_ISTR *is = GetISTR(m_IP++);

	is->opcode = opc;

	return TRUE;
}

BOOL CCompiler::c_Set(unsigned char opc, int dst)
{
	if (m_IP == MAX_CODE)
	{
		return FALSE;
	}

	S_ISTR *is = GetISTR(m_IP++);

	is->opcode = opc;
	is->m_dst  = &m_memf[dst];

	return TRUE;
}

BOOL CCompiler::c_Set(unsigned char opc, int dst, int src)
{
	if (m_IP == MAX_CODE)
	{
		return FALSE;
	}

	S_ISTR *is = GetISTR(m_IP++);

	is->opcode = opc;
	is->m_dst  = &m_memf[dst];
	is->m_src  = &m_memf[src];

	return TRUE;
}

BOOL CCompiler::c_Set(unsigned char opc, int dst, float src)
{
	if (m_IP == MAX_CODE)
	{
		return FALSE;
	}

	S_ISTR *is = GetISTR(m_IP++);

	is->opcode = opc;
	is->m_dst  = &m_memf[dst];
	is->f_src  = src;

	return TRUE;
}


BOOL CCompiler::c_Set(unsigned char opc, LPCTSTR lbl)
{
	if (m_IP == MAX_CODE)
	{
		return FALSE;
	}

	S_ISTR *is = GetISTR(m_IP);

	is->opcode = opc;

	m_LabelsToLink[m_IP++] = lbl;

	return TRUE;
}

BOOL CCompiler::Compile(CString source)
{
	BOOL    done, set_result;
	CString line, od, s, label;

	// cl = line to compile
	CString cl_operand[MAX_LINE_OPERANDS];
	CString cl_opcode;
	int     cl_num_operands;

	// VERY IMPORTANT
	Reset();

	// VERY IMPORTANT
	SourcePurge(source);

	done = FALSE;
	while (!done)
	{
		done = !c_ExLine(line, source);

		if (done)
		{
			continue;
		}

		// is "label:\r" ?
		if (c_IsLabel(line))
		{
			c_ExLabel(label, line);
			c_LabelAdd((LPCTSTR)label, c_GetIp());
			continue;
		}

		// is "opcode ..." ?
		if (!c_ExOpcode(cl_opcode, line))
		{
			c_Error("Opcode expected : ", (LPCTSTR)line);
			return FALSE;
		}

		cl_num_operands = 0;

		while (c_ExOperand(od, line))
		{
			cl_operand[cl_num_operands] = od;

			cl_num_operands++;
		}

		switch (cl_num_operands)
		{
		case (0):
			set_result = c_Set(cl_opcode);
			break;
		case (1):
			set_result = c_Set(cl_opcode, cl_operand[0]);
			break;
		case (2):
			set_result = c_Set(cl_opcode, cl_operand[0], cl_operand[1]);
			break;
		case (5):
			if (cl_opcode == "ex_dist")
			{
				set_result  = c_Set("mov", "huhudx", cl_operand[1]);
				set_result &= c_Set("susq", "huhudx", cl_operand[3]);
				set_result &= c_Set("mov", "huhudy", cl_operand[2]);
				set_result &= c_Set("susq", "huhudy", cl_operand[4]);
				set_result &= c_Set("mov", cl_operand[0], "huhudx");
				set_result &= c_Set("add", cl_operand[0], "huhudy");
				set_result &= c_Set("sqrt", cl_operand[0]);
			}
			else if (cl_opcode == "ex_dist2")
			{
				set_result  = c_Set("mov", "huhudx", cl_operand[1]);
				set_result &= c_Set("susq", "huhudx", cl_operand[3]);
				set_result &= c_Set("mov", "huhudy", cl_operand[2]);
				set_result &= c_Set("susq", "huhudy", cl_operand[4]);
				set_result &= c_Set("mov", cl_operand[0], "huhudx");
				set_result &= c_Set("add", cl_operand[0], "huhudy");
			}
			break;
		}

		if (!set_result)
		{
			c_Error("Error writing code ", "");
			return FALSE;
		}

	} //while

	// uses m_labeltolink[]
	if (!c_LabelsLinkIp())
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CCompiler::Exec()
{
	CString s;

	S_ISTR *is;

	float f;

	m_IP = 0;

	while (m_IP < MAX_CODE)
	{
		is = GetISTR(m_IP++);

		switch (is->opcode)
		{
		case (IS_NOP):
			break;
		case (IS_HLT):
			return TRUE;
			break;
		case (IS_ABORT):
			return FALSE;
			break;
		case (IS_JMP):
			m_IP = is->ip;
			break;
		case (IS_JE):
			if (r_FlagTest(FLAG_ZERO))
			{
				m_IP = is->ip;
			}
			break;
		case (IS_JL):
			if (r_FlagTest(FLAG_NEGATIVE))
			{
				m_IP = is->ip;
			}
			break;
		case (IS_JG):
			if (r_FlagTest(FLAG_POSITIVE))
			{
				m_IP = is->ip;
			}
			break;
		case (IS_JLE):
			if (r_FlagTest(FLAG_NEGATIVE) || r_FlagTest(FLAG_ZERO))
			{
				m_IP = is->ip;
			}
			break;
		case (IS_JGE):
			if (r_FlagTest(FLAG_POSITIVE) || r_FlagTest(FLAG_ZERO))
			{
				m_IP = is->ip;
			}
			break;
		case (IS_ABS_M):
			*is->m_dst = (float)fabs(*is->m_dst);
			break;
		case (IS_SQR_M):
			*is->m_dst = *is->m_dst * *is->m_dst;
			break;
		case (IS_SQRT_M):
			*is->m_dst = (float)sqrt(*is->m_dst);
			break;
		case (IS_SRAND_C):
			srand((int)is->f_dst);
			break;
		case (IS_SRAND_M):
			srand((int)*is->m_dst);
			break;
		case (IS_RAND_M):
			*is->m_dst = float(rand()) / float(RAND_MAX);
			break;
		case (IS_INV_M):
			*is->m_dst = 1.f / *is->m_dst;
			break;
		case (IS_OPP_M):
			*is->m_dst = -*is->m_dst;
			break;
		case (IS_MOV_C):
			*is->m_dst = is->f_src;
			break;
		case (IS_MOV_M):
			*is->m_dst = *is->m_src;
			break;
		case (IS_ADD_C):
			*is->m_dst += is->f_src;
			break;
		case (IS_ADD_M):
			*is->m_dst += *is->m_src;
			break;
		case (IS_SUB_C):
			*is->m_dst -= is->f_src;
			break;
		case (IS_SUB_M):
			*is->m_dst -= *is->m_src;
			break;
		case (IS_MUL_C):
			*is->m_dst *= is->f_src;
			break;
		case (IS_MUL_M):
			*is->m_dst *= *is->m_src;
			break;
		case (IS_DIV_C):
			*is->m_dst /= is->f_src;
			break;
		case (IS_DIV_M):
			*is->m_dst /= *is->m_src;
			break;
		case (IS_SIN_M):
			*is->m_dst = (float)sin(*is->m_dst);
			break;
		case (IS_COS_M):
			*is->m_dst = (float)cos(*is->m_dst);
			break;
		case (IS_ACOS_M):
			*is->m_dst = (float)acos(*is->m_dst);
			break;
		case (IS_SUSQ_C):
			f          = *is->m_dst - is->f_src;
			*is->m_dst = f * f;
			break;
		case (IS_SUSQ_M):
			f          = *is->m_dst - *is->m_src;
			*is->m_dst = f * f;
			break;
		case (IS_MAX_C):
			if (is->f_src > *is->m_dst)
			{
				*is->m_dst = is->f_src;
			}
			break;
		case (IS_MAX_M):
			if (*is->m_src > *is->m_dst)
			{
				*is->m_dst = *is->m_src;
			}
			break;
		case (IS_MIN_C):
			if (is->f_src < *is->m_dst)
			{
				*is->m_dst = is->f_src;
			}
			break;
		case (IS_MIN_M):
			if (*is->m_src < *is->m_dst)
			{
				*is->m_dst = *is->m_src;
			}
			break;
		case (IS_CMP_C):
			f = *is->m_dst - is->f_src;

			r_FlagsReset();
			if (f == 0)
			{
				r_FlagSet(FLAG_ZERO);
			}
			if (f < 0)
			{
				r_FlagSet(FLAG_NEGATIVE);
			}
			if (f > 0)
			{
				r_FlagSet(FLAG_POSITIVE);
			}
			break;
		case (IS_CMP_M):
			f = *is->m_dst - *is->m_src;

			r_FlagsReset();
			if (f == 0)
			{
				r_FlagSet(FLAG_ZERO);
			}
			if (f < 0)
			{
				r_FlagSet(FLAG_NEGATIVE);
			}
			if (f > 0)
			{
				r_FlagSet(FLAG_POSITIVE);
			}
			break;
		}
	}

	return TRUE;
}

/*
//we
  mov  [1],  34 \r\n
mul\t[1] ,[5]\r\n

mov [1],34\r
mul [1],[5]\r

*/
void CCompiler::SourcePurge(CString &source)
{
	BOOL done;
	int  i, j;

	source += "\r\n";

	source.Replace("\t", "");    // tab cut
	source.Replace("\n", "");    // newline cut

	//cut away commented lines (start with //)
	done = FALSE;
	while (!done)
	{
		i = source.Find("//");

		if (i != -1)
		{
			j = source.Find('\r', i);
			source.Delete(i, j - i);
		}
		else
		{
			done = TRUE;
		}
	}
	// comment


	while (source.Replace("  ", " "))
		;                               // redundant spaces cut
	while (source.Replace("\r\r", "\r"))
		;

	// '\r' ',' ':' "//" are attractive
	while (source.Replace(" ,", ","))
		;
	while (source.Replace(", ", ","))
		;
	while (source.Replace(" \r", "\r"))
		;
	while (source.Replace("\r ", "\r"))
		;
	while (source.Replace(" :", ":"))
		;
	while (source.Replace(": ", ":"))
		;
	while (source.Replace(" //", "//"))
		;
	while (source.Replace("// ", "//"))
		;

	source.TrimLeft();
}

//	"nop\r"
//	"mov M1,0\r"
BOOL CCompiler::c_ExOpcode(CString &op, CString &line)
{
	int i = line.Find(' ');
	int j = line.Find('\r');

	if (i == -1)
	{
		if (j == -1)
		{
			return FALSE;
		}
		else
		{
			i = j;
		}
	}

	op = line.Left(i);
	line.Delete(0, i + 1);

	return TRUE;
}

//	"M1,0\r"
//	could be src/dst
BOOL CCompiler::c_ExOperand(CString &od, CString &line)
{
	int i = line.Find(',');

	if (i != -1)
	{
		od = line.Left(i);
		line.Delete(0, i + 1);
		return TRUE;
	}

	int j = line.Find('\r');

	if (j != -1)
	{
		od = line.Left(j);
		line.Delete(0, j + 1);
		return TRUE;
	}

	return FALSE;
}

//	"label:\r"
BOOL CCompiler::c_ExLabel(CString &lbl, CString &line)
{
	int i = line.Find(':');

	if (i == -1)
	{
		return FALSE;
	}

	lbl = line.Left(i);
	line.Delete(0, i + 1);

	return TRUE;
}

BOOL CCompiler::c_ExLine(CString &line, CString &src)
{
	int i = src.Find('\r');

	if (i != -1)
	{
		line = src.Left(i + 1);   // '\r' remains at end
		src.Delete(0, i + 1);
		return TRUE;
	}

	return FALSE;
}

//
//	should check more accurately if the type is TYPE_C
//
int CCompiler::c_GetOperandType(CString od)
{
	if (od.GetLength() == 0)
	{
		return TYPE_ERR;
	}

	if (od.GetAt(0) == '[')
	{
		if (od.GetAt(od.GetLength() - 1) == ']')
		{
			return TYPE_M;
		}
		else
		{
			return TYPE_ERR;
		}
	}

	if (((od.GetAt(0) >= 'a') && (od.GetAt(0) <= 'z')) ||
	    ((od.GetAt(0) >= 'A') && (od.GetAt(0) <= 'Z')))
	{
		return TYPE_ERR;
	}

	// 0..9 or - +
	return TYPE_C;
}

int CCompiler::c_GetMemoryIndex(CString od)
{
	od.Delete(0, 1);
	od.Delete(od.GetLength() - 1, 1);

	return atoi((LPCTSTR)od);
}

float CCompiler::c_GetFloatConstant(CString od)
{
	return (float)atof((LPCTSTR) od);
}

void CCompiler::c_Error(LPCTSTR msg, LPCTSTR where)
{
	CString s;

	s  = msg;
	s += " \"";
	s += where;
	s += "\"";

	Log((LPCTSTR)s);
}

float CCompiler::GetMemoryAt(int index)
{
	return m_memf[index];
}

void CCompiler::CodeDump(LPCTSTR filename)
{
	CString s;
	S_ISTR  *is;
	HANDLE  hFile = CreateFile(filename,
	                           GENERIC_WRITE,
	                           FILE_SHARE_READ,
	                           NULL,
	                           CREATE_ALWAYS,
	                           FILE_ATTRIBUTE_NORMAL,
	                           NULL);

	CFile myFile(hFile);

	CArchive ar(&myFile, CArchive::store, 8192);

	for (int i = 0; i < MAX_CODE; i++)
	{
		is = GetISTR(i);

		switch (is->opcode)
		{
		case (IS_NOP):
			ar.WriteString("nop\r\n");
			break;
		case (IS_MOV_C):
			ar.WriteString("movc\r\n");
			break;
		case (IS_MOV_M):
			ar.WriteString("movm\r\n");
			break;
		case (IS_ADD_C):
			ar.WriteString("addc\r\n");
			break;
		case (IS_ADD_M):
			ar.WriteString("addm\r\n");
			break;
		case (IS_SUB_C):
			ar.WriteString("subc\r\n");
			break;
		case (IS_SUB_M):
			ar.WriteString("subm\r\n");
			break;
		case (IS_SQR_M):
			ar.WriteString("sqrm\r\n");
			break;
		case (IS_SQRT_M):
			ar.WriteString("sqrtm\r\n");
			break;
		case (IS_HLT):
			ar.WriteString("hlt\r\n");
			break;
		}
	}

	ar.Close();
	myFile.Close();
	CloseHandle(hFile);
}

void CCompiler::SetMemoryAt(int index, float f)
{
	m_memf[index] = f;
}

void CCompiler::CodeLoad(void *code)
{
	memcpy(m_Code, code, GetCodeSize());
}

void *CCompiler::GetCodePtr()
{
	return (void *)m_Code;
}

int CCompiler::GetCodeSize()
{
	return MAX_CODE * sizeof(S_ISTR);
}

void CCompiler::c_LabelAdd(LPCTSTR nm, int ip)
{
	S_LABEL *l = &m_Labels[m_numLabels++];

	l->ip   = ip;
	l->name = nm;
}

int CCompiler::c_LabelGetIp(LPCTSTR nm)
{
	S_LABEL *l;

	for (int i = 0; i < m_numLabels; i++)
	{
		l = &m_Labels[i];
		if (l->name == nm)
		{
			return l->ip;
		}
	}

	return -1;
}

//
// "label:\r"
//
BOOL CCompiler::c_IsLabel(CString line)
{
	return (line.Find(":\r") != -1);
}

int CCompiler::c_GetIp()
{
	return m_IP;
}

BOOL CCompiler::c_LabelsLinkIp()
{
	S_ISTR *is;
	int    ip;

	for (int i = 0; i < m_IP; i++)
	{
		is = GetISTR(i);

		if ((is->opcode >= ISSTART_JMP) && (is->opcode <= ISEND_JMP))
		{
			ip = c_LabelGetIp(m_LabelsToLink[i]);
			if (ip == -1)
			{
				c_Error("Label not found :", (LPCTSTR)m_LabelsToLink[i]);
				return FALSE;
			}

			is->ip = ip;
		}
	}

	return TRUE;
}

S_ISTR *CCompiler::GetISTR(int num)
{
	return &m_Code[num];
}

void CCompiler::r_FlagsReset()
{
	m_Flags = 0;
}

void CCompiler::r_FlagSet(UINT flag)
{
	m_Flags |= flag;
}

BOOL CCompiler::r_FlagTest(UINT flag)
{
	return m_Flags & flag;
}

int CCompiler::c_VarAdd(LPCTSTR nm)
{
	S_VAR *var = c_VarGet(nm);

	if (var != NULL)
	{
		return var->address;
	}

	var = &m_Vars[m_numVars];

	var->name    = nm;
	var->address = m_numVars;

	m_numVars++;

	return var->address;
}

S_VAR *CCompiler::c_VarGet(LPCTSTR nm)
{
	S_VAR *var;

	for (int i = 0; i < m_numVars; i++)
	{
		var = &m_Vars[i];

		if (var->name == nm)
		{
			return var;
		}
	}

	return NULL;
}

BOOL CCompiler::c_IsReserved(LPCTSTR res)
{
/*	for(int i=0;i<m_numReserved;i++)
    {
        if (c_mfcreserved[i] == res)
        {
            return TRUE;
        }
    }*/

	return TRUE;
}

BOOL CCompiler::c_IsVar(CString od)
{
	return (
	    ((od.GetAt(0) >= 'a') && (od.GetAt(0) <= 'z')) ||
	    ((od.GetAt(0) >= 'A') && (od.GetAt(0) <= 'Z'))
	    );
}

void CCompiler::c_VarQuery(CString &od)
{
	if (!c_IsVar(od))
	{
		return;
	}

	int address = c_VarAdd((LPCTSTR)od);

	od.Format("[%d]", address);
}

BOOL CCompiler::c_Set(CString op)
{
	unsigned char opcode;

	if (!c_GetOpcode(op, TYPE_ERR, opcode))
	{
		c_Error("Opcode unknown or opcode requires more parameters : ", (LPCTSTR)op);
		return FALSE;
	}

	return c_Set(opcode);
}

BOOL CCompiler::c_Set(CString op, CString od)
{
	unsigned char opcode;
	int           type;

	opcode = IS_UNDEFINED;

	// start jmp detect (opcode operand, could be a jmp statement)
	c_GetOpcode(op, TYPE_ERR, opcode);

	if ((opcode >= ISSTART_JMP) && (opcode <= ISEND_JMP))
	{
		c_Set(opcode, (LPCTSTR)od);
		return TRUE;
	}
	// end jmp process

	c_VarQuery(od);

	type = c_GetOperandType(od);

	c_GetOpcode(op, type, opcode);

	switch (type)
	{
	case (TYPE_ERR):
		c_Error("Type unknown : ", od);
		return FALSE;
		break;
	case (TYPE_C):
		c_Set(opcode, c_GetFloatConstant(od));
		break;
	case (TYPE_M):
		c_Set(opcode, c_GetMemoryIndex(od));
		break;
	}

	return TRUE;
}

BOOL CCompiler::c_Set(CString op, CString od1, CString od2)
{
	unsigned char opcode;
	int           type;
	CString       dst, src;

	dst = od1;
	src = od2;

	c_VarQuery(dst);
	c_VarQuery(src);

	if (c_GetOperandType(dst) != TYPE_M)
	{
		c_Error("Destination must be memory : ", od1);
		return FALSE;
	}

	type = c_GetOperandType(src);

	if (!c_GetOpcode(op, type, opcode))
	{
		c_Error("Opcode unknow or opcode does not require 2 parameters : ", (LPCTSTR)(op + " " + od1 + "," + od2));
		return FALSE;
	}

	switch (type)
	{
	case (TYPE_ERR):
		c_Error("Type unknown : ", src);
		return FALSE;
		break;
	case (TYPE_C):
		c_Set(opcode, c_GetMemoryIndex(dst), c_GetFloatConstant(src));
		break;
	case (TYPE_M):
		c_Set(opcode, c_GetMemoryIndex(dst), c_GetMemoryIndex(src));
		break;
	}

	return TRUE;
}

BOOL CCompiler::c_GetOpcode(CString op, int type, unsigned char &opcode)
{
	opcode = IS_UNDEFINED;

	// type independent

	if (op == "nop")
	{
		opcode = IS_NOP;
	}
	if (op == "hlt")
	{
		opcode = IS_HLT;
	}
	if (op == "abort")
	{
		opcode = IS_ABORT;
	}

	if (op == "jmp")
	{
		opcode = IS_JMP;
	}
	if (op == "je")
	{
		opcode = IS_JE;
	}
	if (op == "jl")
	{
		opcode = IS_JL;
	}
	if (op == "jg")
	{
		opcode = IS_JG;
	}
	if (op == "jle")
	{
		opcode = IS_JLE;
	}
	if (op == "jge")
	{
		opcode = IS_JGE;
	}

	if (opcode != IS_UNDEFINED)
	{
		return TRUE;
	}

	switch (type)
	{
	case (TYPE_ERR):
		return FALSE;
		break;
	case (TYPE_C):
		if (op == "mov")
		{
			opcode = IS_MOV_C;
		}
		if (op == "add")
		{
			opcode = IS_ADD_C;
		}
		if (op == "sub")
		{
			opcode = IS_SUB_C;
		}
		if (op == "mul")
		{
			opcode = IS_MUL_C;
		}
		if (op == "div")
		{
			opcode = IS_DIV_C;
		}
		if (op == "cmp")
		{
			opcode = IS_CMP_C;
		}
		if (op == "susq")
		{
			opcode = IS_SUSQ_C;
		}
		if (op == "max")
		{
			opcode = IS_MAX_C;
		}
		if (op == "min")
		{
			opcode = IS_MIN_C;
		}

		if (op == "srand")
		{
			opcode = IS_SRAND_C;
		}
		break;
	case (TYPE_M):
		if (op == "mov")
		{
			opcode = IS_MOV_M;
		}
		if (op == "add")
		{
			opcode = IS_ADD_M;
		}
		if (op == "sub")
		{
			opcode = IS_SUB_M;
		}
		if (op == "mul")
		{
			opcode = IS_MUL_M;
		}
		if (op == "div")
		{
			opcode = IS_DIV_M;
		}
		if (op == "cmp")
		{
			opcode = IS_CMP_M;
		}
		if (op == "sin")
		{
			opcode = IS_SIN_M;
		}
		if (op == "cos")
		{
			opcode = IS_COS_M;
		}
		if (op == "acos")
		{
			opcode = IS_ACOS_M;
		}
		if (op == "susq")
		{
			opcode = IS_SUSQ_M;
		}
		if (op == "max")
		{
			opcode = IS_MAX_M;
		}
		if (op == "min")
		{
			opcode = IS_MIN_M;
		}

		if (op == "srand")
		{
			opcode = IS_SRAND_M;
		}

		if (op == "abs")
		{
			opcode = IS_ABS_M;
		}
		if (op == "inv")
		{
			opcode = IS_INV_M;
		}
		if (op == "opp")
		{
			opcode = IS_OPP_M;
		}
		if (op == "sqr")
		{
			opcode = IS_SQR_M;
		}
		if (op == "sqrt")
		{
			opcode = IS_SQRT_M;
		}
		if (op == "rand")
		{
			opcode = IS_RAND_M;
		}
		break;
	}

	if (opcode != IS_UNDEFINED)
	{
		return TRUE;
	}

	return FALSE;
}


void CCompiler::Log(LPCTSTR ls)
{
	if (!m_logEnabled)
	{
		return;
	}

	CArchive ar(&m_logfile, CArchive::store, 8192);
	CString  s;

	s  = ls;
	s += "\r\n";

	ar.WriteString((LPCTSTR)s);

	ar.Close();
}

void CCompiler::Log(LPCTSTR ls, float f)
{
	if (!m_logEnabled)
	{
		return;
	}

	CArchive ar(&m_logfile, CArchive::store, 8192);
	CString  s;

	s.Format(" %f", f);
	s = ls + s;

	Log((LPCTSTR)s);

	ar.Close();
}

void CCompiler::Log(LPCTSTR ls1, LPCTSTR ls2)
{
	if (!m_logEnabled)
	{
		return;
	}

	CArchive ar(&m_logfile, CArchive::store, 8192);
	CString  s;

	s  = ls1;
	s += " ";
	s += ls2;

	Log((LPCTSTR)s);

	ar.Close();
}

BOOL CCompiler::c_Set(unsigned char opc, float dst)
{
	if (m_IP == MAX_CODE)
	{
		return FALSE;
	}

	S_ISTR *is = GetISTR(m_IP++);

	is->opcode = opc;
	is->f_dst  = dst;

	return TRUE;
}
