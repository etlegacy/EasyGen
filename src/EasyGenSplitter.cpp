// EasyGenSplitter.cpp : implementation file
//

#include "easygen.h"
#include "EasyGenSplitter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEasyGenSplitter

CEasyGenSplitter::CEasyGenSplitter()
{
	m_cxSplitter    = m_cySplitter = 5;
	m_cxBorderShare = m_cyBorderShare = 0;
	m_cxSplitterGap = m_cySplitterGap = 5;
	m_cxBorder      = m_cyBorder = 2;
}

CEasyGenSplitter::~CEasyGenSplitter()
{
}


BEGIN_MESSAGE_MAP(CEasyGenSplitter, CSplitterWnd)
//{{AFX_MSG_MAP(CEasyGenSplitter)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CEasyGenSplitter message handlers

void CEasyGenSplitter::OnDrawSplitter(CDC *pDC, ESplitType nType, const CRect& rect)
{
	// Let CSplitterWnd handle everything but the border-drawing
	if ((nType != splitBorder) || (pDC == NULL))
	{
		CSplitterWnd::OnDrawSplitter(pDC, nType, rect);
		return;
	}

	ASSERT_VALID(pDC);

	// Draw border
	CRect  r;
	CBrush brblk(RGB(0, 0, 0));
	CBrush brface(GetSysColor(COLOR_BTNFACE));

/*
    r = rect;
    pDC->Draw3dRect(r, GetSysColor(COLOR_BTNSHADOW), GetSysColor(COLOR_BTNHIGHLIGHT));
    r.DeflateRect(1,1);
    pDC->Draw3dRect(r, GetSysColor(COLOR_BTNSHADOW), GetSysColor(COLOR_BTNHIGHLIGHT));
*/
/*
    r = rect;
    pDC->Draw3dRect(r, GetSysColor(COLOR_BTNFACE), GetSysColor(COLOR_BTNFACE));
    r.DeflateRect(1,1);
    pDC->Draw3dRect(r, GetSysColor(COLOR_BTNFACE), GetSysColor(COLOR_BTNFACE));
*/
	r = rect;
	pDC->FrameRect(&r, &brface);
	r.DeflateRect(1, 1);
//	pDC->FrameRect(&r,&brblk);
	pDC->Draw3dRect(r, GetSysColor(COLOR_BTNSHADOW), GetSysColor(COLOR_BTNHIGHLIGHT));
//	pDC->Draw3dRect(r, GetSysColor(COLOR_BTNHIGHLIGHT), GetSysColor(COLOR_BTNSHADOW));
}
