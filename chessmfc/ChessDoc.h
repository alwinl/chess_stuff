// ChessDoc.h : interface of the CChessDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHESSDOC_H__7B688DFD_F7CD_11D4_8C76_B04984000000__INCLUDED_)
#define AFX_CHESSDOC_H__7B688DFD_F7CD_11D4_8C76_B04984000000__INCLUDED_

#include "Board.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CChessDoc : public CDocument
{
protected: // create from serialization only
	CChessDoc();
	DECLARE_DYNCREATE(CChessDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChessDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	CBoard ChessBoard;
	int Analyse();
	CString m_straInfoText[ IDC_INFOMESSAGE - IDC_INFOTURN + 1 ];
	bool Move( CPoint pntFrom, CPoint pntTo );
	virtual ~CChessDoc();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	void PaintArmys( CDC *pDC, CRect& rBoardRect, CDC* pMemDC, bool fReverse );
	void ChangeInfoText(int idxTxtInfo, CString strNewValue);

protected:
	bool m_fIsWhitesTurn;
	CArmy * m_pWhiteArmy;
	CArmy * m_pBlackArmy;

// Generated message map functions
protected:
	CTime m_tmChessClock;
	float m_fAverageTime;
	int   m_iMaxLevel;
	int   m_iLevel;

	bool InitBoard();

	//{{AFX_MSG(CChessDoc)
	//}}AFX_MSG
	afx_msg void OnLevel( UINT nID );
	afx_msg void OnUpdateLevel( CCmdUI* pCmdUI, UINT nID );

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHESSDOC_H__7B688DFD_F7CD_11D4_8C76_B04984000000__INCLUDED_)
