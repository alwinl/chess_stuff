// ChessFrm.h : interface of the CChessFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHESSFRM_H__7B688DFB_F7CD_11D4_8C76_B04984000000__INCLUDED_)
#define AFX_CHESSFRM_H__7B688DFB_F7CD_11D4_8C76_B04984000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CChessFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CChessFrame();
	DECLARE_DYNCREATE(CChessFrame)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChessFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CChessFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

// Generated message map functions
protected:
	CMenu m_mnuMain;
	CMenu m_mnuEdit;
	CMenu m_mnuThink;
	//{{AFX_MSG(CChessFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnEditArrange();
	afx_msg void OnEditdone();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHESSFRM_H__7B688DFB_F7CD_11D4_8C76_B04984000000__INCLUDED_)
