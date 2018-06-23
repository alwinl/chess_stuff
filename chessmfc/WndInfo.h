#if !defined(AFX_WNDINFO_H__281FB894_05FF_11D5_8C89_C25214000000__INCLUDED_)
#define AFX_WNDINFO_H__281FB894_05FF_11D5_8C89_C25214000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WndInfo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWndInfo dialog

class CWndInfo : public CDialog
{
// Construction
public:
	void SetInfo( int idxTxtInfo, CString strNewValue );
	CWndInfo(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWndInfo)
	enum { IDD = IDD_INFO };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWndInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CStatic m_txtInfo[ IDC_INFOMESSAGE - IDC_INFOTURN + 1 ];

	// Generated message map functions
	//{{AFX_MSG(CWndInfo)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDINFO_H__281FB894_05FF_11D5_8C89_C25214000000__INCLUDED_)
