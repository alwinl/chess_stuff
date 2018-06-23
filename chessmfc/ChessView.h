// ChessView.h : interface of the CChessView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHESSVIEW_H__7B688DFF_F7CD_11D4_8C76_B04984000000__INCLUDED_)
#define AFX_CHESSVIEW_H__7B688DFF_F7CD_11D4_8C76_B04984000000__INCLUDED_

#include "WndInfo.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// Memory DC Avoid flickering

class CMemDC : public CDC
{
public:

    CMemDC(CDC* pDC) : CDC() {

        ASSERT(pDC != NULL);

        m_pDC = pDC;
        m_pOldBitmap = NULL;
        m_bMemDC = !pDC->IsPrinting();
              
        if( m_bMemDC ) {    // Create a Memory DC
            pDC->GetClipBox(&m_rect);
            CreateCompatibleDC(pDC);
            m_bitmap.CreateCompatibleBitmap(pDC, m_rect.Width(), m_rect.Height());
            m_pOldBitmap = SelectObject(&m_bitmap);
            SetWindowOrg(m_rect.left, m_rect.top);
        } else {
			// Make a copy of the relevent parts of the current DC for printing
            m_bPrinting = pDC->m_bPrinting;
            m_hDC       = pDC->m_hDC;
            m_hAttribDC = pDC->m_hAttribDC;
        }
    }
    
    // Destructor copies the contents of the mem DC to the original DC
    ~CMemDC() {
        if( m_bMemDC )  {    
            // Copy the offscreen bitmap onto the screen.
            m_pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(), this, m_rect.left, m_rect.top, SRCCOPY);

            //Swap back the original bitmap.
            SelectObject(m_pOldBitmap);
        } else {
            // All we need to do is replace the DC with an illegal value,
            // this keeps us from accidently deleting the handles associated with
            // the CDC that was passed to the constructor.
            m_hDC = m_hAttribDC = NULL;
        }
    }

    // Allow usage as a pointer
    CMemDC* operator->() { return this; }
        
    // Allow usage as a pointer
    operator CMemDC*() { return this; }

private:
    CBitmap  m_bitmap;      // Offscreen bitmap
    CBitmap* m_pOldBitmap;  // bitmap originally found in CMemDC
    CDC*     m_pDC;         // Saves CDC passed in constructor
    CRect    m_rect;        // Rectangle of drawing area.
    BOOL     m_bMemDC;      // TRUE if CDC really is a Memory DC.
};

class CChessView : public CView
{
protected: // create from serialization only
	CChessView();
	DECLARE_DYNCREATE(CChessView)

// Attributes
public:
	CChessDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChessView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	protected:
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CChessView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	CWndInfo m_wndInfo;
	CPoint   m_pntOrigin;
	bool     m_fReverse;
	CBitmap  m_bmpPieces;
	COLORREF m_clrBack;
	COLORREF m_clrBlack;
	COLORREF m_clrWhite;
	CPen     m_penGrey;
	CPen     m_penLtGrey;

	//{{AFX_MSG(CChessView)
	afx_msg void OnDisplayReverseboard();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDisplayShowbestline();
	//}}AFX_MSG
	afx_msg void OnDisplayColors( UINT id );
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ChessView.cpp
inline CChessDoc* CChessView::GetDocument()
   { return (CChessDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHESSVIEW_H__7B688DFF_F7CD_11D4_8C76_B04984000000__INCLUDED_)
