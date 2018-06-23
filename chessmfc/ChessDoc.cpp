// ChessDoc.cpp : implementation of the CChessDoc class
//

#include "stdafx.h"
#include "chessmfc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChessDoc

IMPLEMENT_DYNCREATE(CChessDoc, CDocument)

BEGIN_MESSAGE_MAP(CChessDoc, CDocument)
	//{{AFX_MSG_MAP(CChessDoc)
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE( ID_LEVEL_EASY, ID_LEVEL_MATESEARCH, OnLevel )
	ON_UPDATE_COMMAND_UI_RANGE( ID_LEVEL_EASY, ID_LEVEL_MATESEARCH, OnUpdateLevel )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChessDoc construction/destruction

CChessDoc::CChessDoc()
{
	// TODO: add one-time construction code here
	m_pBlackArmy = new CArmy( &ChessBoard, true );
	m_pWhiteArmy = new CArmy( &ChessBoard, false );

	m_fIsWhitesTurn = true;
	m_iLevel = ID_LEVEL_EASY;

	m_fAverageTime = 5.0;
	m_iMaxLevel    = 23;
}

CChessDoc::~CChessDoc()
{
	delete m_pBlackArmy;
	delete m_pWhiteArmy;
}

BOOL CChessDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())										
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	InitBoard();

	UpdateAllViews( NULL, 1 );

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CChessDoc serialization

void CChessDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
		InitBoard();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CChessDoc diagnostics

#ifdef _DEBUG
void CChessDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CChessDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChessDoc commands


bool CChessDoc::InitBoard()
{
	delete m_pBlackArmy;
	delete m_pWhiteArmy;

	m_pBlackArmy = new CArmy( &ChessBoard, true );
	m_pWhiteArmy = new CArmy( &ChessBoard, false );

	m_fIsWhitesTurn = true;

    m_straInfoText[ IDC_INFOTURN     - IDC_INFOTURN ] = "White";
    m_straInfoText[ IDC_INFOWHITE    - IDC_INFOTURN ] = "";
    m_straInfoText[ IDC_INFOBLACK    - IDC_INFOTURN ] = "";
    m_straInfoText[ IDC_INFOTIME     - IDC_INFOTURN ] = "";
    m_straInfoText[ IDC_INFOLEVEL    - IDC_INFOTURN ] = "Easy";
    m_straInfoText[ IDC_INFOVALUE    - IDC_INFOTURN ] = "";
    m_straInfoText[ IDC_INFONODES    - IDC_INFOTURN ] = "";
    m_straInfoText[ IDC_INFONSEC     - IDC_INFOTURN ] = "";
    m_straInfoText[ IDC_INFODEPTH    - IDC_INFOTURN ] = "";
    m_straInfoText[ IDC_INFOBESTLINE - IDC_INFOTURN ] = "";
    m_straInfoText[ IDC_INFOMESSAGE  - IDC_INFOTURN ] = "";


	return true;
}

bool CChessDoc::Move(CPoint pntFrom, CPoint pntTo)
{
	if( m_fIsWhitesTurn ) {
		if( !m_pWhiteArmy->MovePiece( m_pBlackArmy, pntFrom, pntTo ) )
			return false;
		ChangeInfoText( IDC_INFOWHITE, m_pWhiteArmy->m_strLastMove );
	} else {
		if( !m_pBlackArmy->MovePiece( m_pWhiteArmy, pntFrom, pntTo ) )
			return false;
		ChangeInfoText( IDC_INFOBLACK, m_pBlackArmy->m_strLastMove );
	}

	m_fIsWhitesTurn = !m_fIsWhitesTurn;
	ChangeInfoText( IDC_INFOTURN, m_fIsWhitesTurn ? _T("White") : _T("Black") );

	return true;
}

void CChessDoc::PaintArmys( CDC *pDC, CRect& rBoardRect, CDC* pMemDC, bool fReverse )
{
	m_pWhiteArmy->DoPaint( pDC, rBoardRect, pMemDC, fReverse );
	m_pBlackArmy->DoPaint( pDC, rBoardRect, pMemDC, fReverse );
}

void CChessDoc::OnUpdateLevel(CCmdUI* pCmdUI, UINT nID) 
{
	pCmdUI->SetCheck( ( m_iLevel == pCmdUI->m_nID ) ? 1 : 0 );
}

void CChessDoc::OnLevel( UINT nID ) 
{
	CDlgInput dlg;
	CString strInfo;

	m_iLevel = nID;

	switch( m_iLevel ) {
    case ID_LEVEL_EASY        :
		m_fAverageTime = 5.0;
		m_iMaxLevel    = 23;
		ChangeInfoText( IDC_INFOLEVEL, "Easy" );
		break;

    case ID_LEVEL_INFINITE    :
		m_iMaxLevel    = 23;
		ChangeInfoText( IDC_INFOLEVEL, "Infinite" );
		break;

    case ID_LEVEL_TIMEPERMOVE :
		dlg.m_strTitle  = "Set seconds per move";
		dlg.m_strPrompt = "Seconds per move:";
		dlg.m_fValue    = m_fAverageTime;

		if( dlg.DoModal() != IDOK )
			return;

		m_fAverageTime = dlg.m_fValue;

		//m_ChessTime[ white ].totaltime = ( m_MoveNo / 2 ) * m_fAverageTime;
		//m_ChessTime[ black ].totaltime = ( m_MoveNo / 2 ) * m_fAverageTime;
		m_iMaxLevel = 23;

		strInfo.Format(	"%1.0f sec / move", m_fAverageTime );
		ChangeInfoText( IDC_INFOLEVEL, strInfo );
		break;

    case ID_LEVEL_TOTALTIME   :

		dlg.m_strTitle  = "Set minutes per game";
		dlg.m_strPrompt = "Minutes per game:";
		dlg.m_fValue    = m_fAverageTime;

		if( dlg.DoModal() != IDOK )
			return;

		m_fAverageTime = dlg.m_fValue;

		//m_ChessTime[ white ].totaltime = ( m_MoveNo / 2 ) * m_fAverageTime;
		//m_ChessTime[ black ].totaltime = ( m_MoveNo / 2 ) * m_fAverageTime;
		m_iMaxLevel = 23;

		strInfo.Format(	"%2.2f min / game", m_fAverageTime );
		ChangeInfoText( IDC_INFOLEVEL, strInfo );
		break;

    case ID_LEVEL_PLAYSEARCH  :
		dlg.m_strTitle  = "Set play depth";
		dlg.m_strPrompt = "Number of plays:";
		dlg.m_fValue    = (float)m_iMaxLevel;

		if( dlg.DoModal() != IDOK )
			return;

		m_iMaxLevel = ( ((int)dlg.m_fValue > 23) ? 23 : (int)dlg.m_fValue );

		strInfo.Format(	"Analyse %d plays deep", m_iMaxLevel );
		ChangeInfoText( IDC_INFOLEVEL, strInfo );
		break;

	case ID_LEVEL_MATCHING     :
		ChangeInfoText( IDC_INFOLEVEL, "Match users time" );
		break;

	case ID_LEVEL_MATESEARCH   :
		ChangeInfoText( IDC_INFOLEVEL, "Mate search" );
	//	ComputersTurn();
		break;
	}

	Analyse();
}


void CChessDoc::ChangeInfoText(int idxTxtInfo, CString strNewValue)
{
    m_straInfoText[ idxTxtInfo  - IDC_INFOTURN ] = strNewValue;
	UpdateAllViews( NULL );
}

int CChessDoc::Analyse()
{


	int whitematerial     = m_pWhiteArmy->CalcTotalMaterial();
	int blackmaterial     = m_pBlackArmy->CalcTotalMaterial();
	CPoint pntWhiteKing   = m_pWhiteArmy->GetKingPos();
	CPoint pntBlackKing   = m_pBlackArmy->GetKingPos();
	int pawntotalmaterial = m_pWhiteArmy->CalcPawnMaterial() + m_pBlackArmy->CalcPawnMaterial();

	m_pWhiteArmy->CalcAttackTable( whitematerial, blackmaterial, pntBlackKing, pawntotalmaterial );
	m_pBlackArmy->CalcAttackTable( blackmaterial, whitematerial, pntWhiteKing, pawntotalmaterial );

#if 0
	char distan[8] = { 3, 2, 1, 0, 0, 1, 2, 3 };
	const unsigned char squarerankvalue[8]     = { 0, 0, 0, 0, 1, 2, 4, 4 };
	CPoint apntSurround[] = { (-1, -1), (-1, 0), (-1, 1), (0, -1), (0, 1), (1, -1), (1, 0), (1, 1) };

	int whitematerial     = m_pWhiteArmy->CalcTotalMaterial();
	int blackmaterial     = m_pBlackArmy->CalcTotalMaterial();
	int totalmaterial     = whitematerial + blackmaterial;
	int material          = whitematerial - blackmaterial;

	int pawntotalmaterial = m_pWhiteArmy->CalcPawnMaterial() + m_pBlackArmy->CalcPawnMaterial();
	bool mating           = m_pWhiteArmy->IsMating( totalmaterial, material );

    int materiallevel     = max( 0, totalmaterial - 0x2000 ) / 0x100;

    //losingcolor = ( ( material < 0 ) ? white : black );

    /*  Calculate attack values of each square (importance of each square)  */
	CPoint pntSquare;
	
    for( pntSquare.x = 'A'; pntSquare.x <= 'H'; pntSquare.x++ ) {
        for( pntSquare.y = 1; pntSquare.y <= 8; pntSquare.y++ ) {

            m_pWhiteArmy->AttackValue[ ( pntSquare.x - 'A' ) * 8 + pntSquare.y - 1 ] =
            m_pBlackArmy->AttackValue[ ( 'H' - pntSquare.x ) * 8 + pntSquare.y - 1 ] =
			           ( ( squarerankvalue[ pntSquare.x - 'A' ] * 3 * ( materiallevel + 8 ) ) >> 5 )	/*  Rank importrance  */ 
			           + max( 0, 8 - 3 * ( distan[ pntSquare.x - 'A' ] + distan[ pntSquare.y - 1 ] ) ); /*  center importance */


        }
	}

	int idx;

	/*  Add importance of the 8 squares around the opponent's King  */
    if( materiallevel > 0 ) {
		CPoint pntBlackKing = m_pBlackArmy->GetKingPos();
		CPoint pntWhiteKing = m_pWhiteArmy->GetKingPos();

		for( int dir = 0; dir < 8; dir++ ) {

			pntSquare = pntBlackKing + apntSurround[ dir ];
			idx = ( pntSquare.x - 'A' ) * 8 + pntSquare.y - 1;

			if( idx >= 0 && idx < 64 )
	            m_pWhiteArmy->AttackValue[ idx ] += ((12 * ( materiallevel + 8 ) ) >> 5 );

			pntSquare = pntWhiteKing + apntSurround[ dir ];
			idx = ( pntSquare.x - 'A' ) * 8 + pntSquare.y - 1;

			if( idx >= 0 && idx < 64 )
	            m_pBlackArmy->AttackValue[ idx ] += ((12 * ( materiallevel + 8 ) ) >> 5 );
		}
	}
#endif
	return 0;
}
