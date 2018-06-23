// chessmfc.h : main header file for the CHESSMFC application
//

#if !defined(AFX_CHESSMFC_H__7B688DF7_F7CD_11D4_8C76_B04984000000__INCLUDED_)
#define AFX_CHESSMFC_H__7B688DF7_F7CD_11D4_8C76_B04984000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

#include <list>

using namespace std;

#include "Board.h"
#include "Piece.h"

#include "ChessDoc.h"
#include "ChessFrm.h"
#include "ChessDoc.h"

#include "WndInfo.h"
#include "ChessView.h"

#include "DlgInput.h"

/////////////////////////////////////////////////////////////////////////////
// CChessApp:
// See chessmfc.cpp for the implementation of this class
//

class CChessApp : public CWinApp
{
public:
	CChessApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChessApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CChessApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHESSMFC_H__7B688DF7_F7CD_11D4_8C76_B04984000000__INCLUDED_)
