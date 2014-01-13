// TextureMan.cpp : implementation file
//

#include "EasyGen.h"
#include "TextureMan.h"

#include "EasyGenDoc.h"
#include "EasyGenView.h"

#include "Texture.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTextureMan

IMPLEMENT_DYNCREATE(CTextureMan, CScrollView)

CTextureMan::CTextureMan()
{
	m_TextureSelected = NULL;

	LOGFONT logFont;
	logFont.lfHeight         = 12;
	logFont.lfWidth          = 0;
	logFont.lfEscapement     = 0;
	logFont.lfOrientation    = 0;
	logFont.lfWeight         = FW_NORMAL;
	logFont.lfItalic         = 0;
	logFont.lfUnderline      = 0;
	logFont.lfStrikeOut      = 0;
	logFont.lfCharSet        = ANSI_CHARSET;
	logFont.lfOutPrecision   = OUT_DEFAULT_PRECIS;
	logFont.lfClipPrecision  = CLIP_DEFAULT_PRECIS;
	logFont.lfQuality        = PROOF_QUALITY;
	logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_ROMAN;
	strcpy(logFont.lfFaceName, "Tahoma");

	m_Font.CreateFontIndirect(&logFont);
}

CTextureMan::~CTextureMan()
{
}


BEGIN_MESSAGE_MAP(CTextureMan, CScrollView)
//{{AFX_MSG_MAP(CTextureMan)
ON_WM_LBUTTONDOWN()
ON_WM_ERASEBKGND()
ON_WM_SIZE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTextureMan drawing

void CTextureMan::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: calculate the total size of this view
	sizeTotal.cx = sizeTotal.cy = 200;
	SetScrollSizes(MM_TEXT, sizeTotal);
}

void CTextureMan::OnDraw(CDC *pDC)
{
	CDocument *pDoc = GetDocument();

	CTexture *tex;
	CBrush   brSelRect;
	SIZE     sz;
	int      y     = 20;
	int      x     = 8;
	int      ystep = 20;
	CRect    scrRect, rect;

	COLORREF colBk      = RGB(128, 128, 128);
	COLORREF colTextSel = RGB(255, 255, 255);
	COLORREF colText    = RGB(0, 0, 0);
	COLORREF colRectSel = RGB(212, 32, 32);

	CFont *oldFont = pDC->SelectObject(&m_Font);

	GetClientRect(&scrRect);

	m_Textures.ScanReset();

	while (tex = m_Textures.ScanNext())
	{
		tex->m_ThumbPos.SetRect(
		    x,
		    y,
		    x + tex->m_Thumb.GetWidth(),
		    y + tex->m_Thumb.GetHeight()
		    );

		y += tex->m_Thumb.GetHeight() + ystep;
	}

	if (y > scrRect.bottom)
	{
		scrRect.bottom = y;
	}

	pDC->FillRect(scrRect, &CBrush(colBk));
	pDC->SetBkColor(colBk);

	m_Textures.ScanReset();

	while (tex = m_Textures.ScanNext())
	{
		tex->m_Thumb.DrawImage(pDC->m_hDC, tex->m_ThumbPos.left, tex->m_ThumbPos.top, tex->m_Thumb.GetWidth(), tex->m_Thumb.GetHeight());

		if (m_TextureSelected == tex)
		{
			brSelRect.CreateSolidBrush(colRectSel);
			rect = tex->m_ThumbPos;
			rect.InflateRect(2, 2);
			pDC->FrameRect((LPRECT)rect, &brSelRect);
			rect.InflateRect(1, 1);
			pDC->FrameRect((LPRECT)rect, &brSelRect);

			pDC->SetTextColor(colTextSel);
		}
		else
		{
			pDC->SetTextColor(colText);
		}

		pDC->TextOut(tex->m_ThumbPos.left, tex->m_ThumbPos.top - 13, tex->m_sInfo);
	}

	sz.cx = scrRect.right;
	sz.cy = scrRect.bottom;

	SetScrollSizes(MM_TEXT, sz);

	pDC->SelectObject(oldFont);
}

/////////////////////////////////////////////////////////////////////////////
// CTextureMan diagnostics

#ifdef _DEBUG
void CTextureMan::AssertValid() const
{
	CScrollView::AssertValid();
}

void CTextureMan::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTextureMan message handlers

CTexture *CTextureMan::AddTexture(LPCTSTR filename)
{
	return m_Textures.Add(filename);
}

void CTextureMan::OnLButtonDown(UINT nFlags, CPoint point)
{
	CEasyGenDoc *pDoc = GetDocument();

	CTexture *tex;
	CPoint   delta = GetScrollPosition();
	BOOL     found = FALSE;

	BOOL kCTRL = nFlags & MK_CONTROL;

	point += delta;

	m_Textures.ScanReset();

	while ((tex = m_Textures.ScanNext()) && (!found))
	{
		if (tex->m_ThumbPos.PtInRect(point))
		{
			found = TRUE;

			m_TextureSelected = tex;

			pDoc->SetCurrentTexture(tex);

			pDoc->SetAlphaTexture(tex);

//			Lock(m_TextureSelected);
			RefreshLocking();
		}
	}

	InvalidateRect(NULL, FALSE);

	CScrollView::OnLButtonDown(nFlags, point);
}

CEasyGenDoc *CTextureMan::GetDocument()
{
	CEasyGenApp  *pApp       = (CEasyGenApp *)AfxGetApp();
	CMainFrame   *pMainFrame = (CMainFrame *)pApp->m_pMainWnd;
	CEasyGenView *pView      = (CEasyGenView *)pMainFrame->m_wndSplitter.GetPane(0, 1);
	return pView->GetDocument();
}

BOOL CTextureMan::OnEraseBkgnd(CDC *pDC)
{
//	CBrush br( GetSysColor( RGB(100,100,100) ) );
//	FillOutsideRect( pDC, &br );
//	return TRUE;                   // Erased
	return FALSE;
//	return CScrollView::OnEraseBkgnd(pDC);
}

void CTextureMan::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);

	Invalidate();
}

void CTextureMan::Clear()
{
	CEasyGenDoc *pDoc = GetDocument();

	m_Textures.RemoveAll();
	m_TextureSelected = m_Textures.Get(m_TextureSelected);

	// should be locked, != NULL
	if (m_TextureSelected == NULL)
	{
		pDoc->SetCurrentTexture(NULL);
	}

	InvalidateRect(NULL, FALSE);
}

void CTextureMan::Lock(CTexture *tex)
{
	CTexture *texx;

	m_Textures.ScanReset();

	while (texx = m_Textures.ScanNext())
	{
		texx->m_Locked = tex == texx;
	}
}

BOOL CTextureMan::IsTextureValid(CTexture *tex)
{
	return m_Textures.Get(tex) != NULL;
}

void CTextureMan::RefreshLocking()
{
	CEasyGenDoc *pDoc = GetDocument();

	CTexture *tex;

	m_Textures.ScanReset();
	while (tex = m_Textures.ScanNext())
	{
		tex->Lock(pDoc->IsTextureInUse(tex));
	}
}

void CTextureMan::SelectTexture(CTexture *tex)
{
	m_TextureSelected = tex;
}
