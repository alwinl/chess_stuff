#if !defined(AFX_DLGCOLOR_H__EC174B46_F87A_11D4_8C76_B04984000000__INCLUDED_)
#define AFX_DLGCOLOR_H__EC174B46_F87A_11D4_8C76_B04984000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgColor.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgColor dialog

class CDlgColor : public CDialog
{
// Construction
public:
	CDlgColor(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgColor)
	enum { IDD = IDD_COLOR };
	CButton	m_BackButton;
	CStatic	m_picWhiteSquare;
	CStatic	m_picBlackSquare;
	CStatic	m_picBackColor;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgColor)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	COLORREF m_clrBack;
	COLORREF m_clrBlack;
	COLORREF m_clrWhite;

// Implementation
protected:
	int DoColor( COLORREF * pColorRef );

	// Generated message map functions
	//{{AFX_MSG(CDlgColor)
	afx_msg void OnColorBackcolor();
	afx_msg void OnColorBlacksquare();
	afx_msg void OnColorWhitesquare();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCOLOR_H__EC174B46_F87A_11D4_8C76_B04984000000__INCLUDED_)
