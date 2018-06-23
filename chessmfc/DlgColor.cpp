// DlgColor.cpp : implementation file
//

#include "stdafx.h"
#include "chessmfc.h"
#include "DlgColor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgColor dialog


CDlgColor::CDlgColor(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgColor::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgColor)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgColor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgColor)
	DDX_Control(pDX, IDC_COLOR_BACKCOLOR, m_BackButton);
	DDX_Control(pDX, IDC_COLOR_WHITESQUAREPIC, m_picWhiteSquare);
	DDX_Control(pDX, IDC_COLOR_BLACKSQUAREPIC, m_picBlackSquare);
	DDX_Control(pDX, IDC_COLOR_BACKCOLORPIC, m_picBackColor);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgColor, CDialog)
	//{{AFX_MSG_MAP(CDlgColor)
	ON_BN_CLICKED(IDC_COLOR_BACKCOLOR, OnColorBackcolor)
	ON_BN_CLICKED(IDC_COLOR_BLACKSQUARE, OnColorBlacksquare)
	ON_BN_CLICKED(IDC_COLOR_WHITESQUARE, OnColorWhitesquare)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgColor message handlers


int CDlgColor::DoColor(COLORREF *pColorRef)
{
	CColorDialog dlg( *pColorRef, 0, this );
	
	//dlg.SetCurrentColor( *pColorRef );

	if( dlg.DoModal() == IDOK ) {
		*pColorRef = dlg.GetColor();
		return( true );
	}

	return( false );
}

void CDlgColor::OnColorBackcolor() 
{
	DoColor( &m_clrBack );

}

void CDlgColor::OnColorBlacksquare() 
{
	DoColor( &m_clrBlack );
}

void CDlgColor::OnColorWhitesquare() 
{
	DoColor( &m_clrWhite );
}


