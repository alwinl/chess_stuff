// WndInfo.cpp : implementation file
//

#include "stdafx.h"
#include "chessmfc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWndInfo dialog


CWndInfo::CWndInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CWndInfo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWndInfo)
	//}}AFX_DATA_INIT
}


void CWndInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWndInfo)
	//}}AFX_DATA_MAP
	for( int iCount = IDC_INFOTURN; iCount <= IDC_INFOMESSAGE; iCount++ )
		DDX_Control( pDX, iCount, m_txtInfo[ iCount - IDC_INFOTURN ] );
}


BEGIN_MESSAGE_MAP(CWndInfo, CDialog)
	//{{AFX_MSG_MAP(CWndInfo)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWndInfo message handlers

void CWndInfo::SetInfo(int idxTxtInfo, CString strNewValue)
{

	m_txtInfo[ idxTxtInfo - IDC_INFOTURN ].SetWindowText( strNewValue );
}
