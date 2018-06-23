// ChessFrm.cpp : implementation of the CChessFrame class
//

#include "stdafx.h"
#include "chessmfc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChessFrame

IMPLEMENT_DYNCREATE(CChessFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CChessFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CChessFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_EDIT_ARRANGE, OnEditArrange)
	ON_COMMAND(ID_EDITDONE, OnEditdone)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CChessFrame construction/destruction

CChessFrame::CChessFrame()
{
	// TODO: add member initialization code here
	m_mnuThink.LoadMenu( IDR_THINKING );
	m_mnuEdit.LoadMenu(IDR_EDIT );
	m_mnuMain.LoadMenu(IDR_MAINFRAME );

}

CChessFrame::~CChessFrame()
{
}

int CChessFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
}

BOOL CChessFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CChessFrame diagnostics

#ifdef _DEBUG
void CChessFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CChessFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChessFrame message handlers


void CChessFrame::OnEditArrange() 
{
	// TODO: Add your command handler code here
	BOOL fResult = SetMenu( &m_mnuEdit );
}

void CChessFrame::OnEditdone() 
{
	// TODO: Add your command handler code here
	BOOL fResult = SetMenu( &m_mnuMain );
}
