// Piece.cpp: implementation of the CPiece class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "chessmfc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define BORDERWIDTH 3
#define SQUAREHEIGHT 36
#define SQUAREWIDTH  36

#define MASKOFFSET  0
#define BLACKOFFSET SQUAREHEIGHT
#define WHITEOFFSET (2 * SQUAREHEIGHT)

int aPieceValues[] = { 256, 1216, 768, 768, 2304, 4096, 0 };

char aPieceShorthand[] = { ' ', 'R', 'K', 'B', 'Q', 'K', 0 };

//////////////////////////////////////////////////////////////////////
// CPiece
//////////////////////////////////////////////////////////////////////
CPiece::CPiece( CArmy * pArmy, CPoint curPos ) : m_pArmy( pArmy), m_curPos( curPos ) 
{ 
	m_pArmy->m_pBoard->SetPieceOnSquare( m_curPos, this ); 
}

CPiece::~CPiece() 
{
	m_pArmy->m_pBoard->TakePieceFromSquare( m_curPos ); 
};

//////////////////////////////////////////////////////////////////////
// Can this piece do this move

bool CPiece::ValidMove( CPoint pntTo )
{
	if( (pntTo.x < 'A') || (pntTo.x > 'H') || (pntTo.y < 1) || (pntTo.y > 8) )
		return false;

	if( m_pArmy->PieceOnSquare( pntTo ) )
		return false;

	return true ;
}

//////////////////////////////////////////////////////////////////////
// Move this piece to this square

bool CPiece::MoveTo(CPoint pntMoveTo)
{
	m_pArmy->m_pBoard->TakePieceFromSquare( m_curPos ); 
	m_curPos = pntMoveTo;
	m_pArmy->m_pBoard->SetPieceOnSquare( m_curPos, this ); 
	return true;
}

//////////////////////////////////////////////////////////////////////
// Is the other piece on the same square as this piece

bool CPiece::operator ==(CPiece* other)
{
	if( m_curPos == other->m_curPos )
		return true;

	return false;
}

//////////////////////////////////////////////////////////////////////
// CPawn
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Can this pawn do this move

bool CPawn::ValidMove( CPoint pntTo)
{
	if( !CPiece::ValidMove( pntTo ) )
		return( false );

	list<CPiece *>::iterator PieceIt;
	int dy = pntTo.y - m_curPos.y;
	int dx = pntTo.x - m_curPos.x;

	int iMul = ( m_pArmy->IsBlack() ? -1 : 1 );
	int pos = ( m_pArmy->IsBlack() ? 7 : 2 );

	if( !dx ) {
		// normal move
		if( dy == iMul ) {
			if( !m_pArmy->m_pBoard->GetPieceFromSquare( pntTo ) )
				return true;

		} else if( (dy == 2*iMul) && (m_curPos.y == pos) )
			if( ( !m_pArmy->m_pBoard->GetPieceFromSquare( pntTo ) ) &&  ( !m_pArmy->m_pBoard->GetPieceFromSquare( CPoint( m_curPos.x, m_curPos.y + iMul ) ) ) )
				return true;

	} else if( (abs( dx ) == 1 ) && (dy == iMul) ) {
		if( m_pArmy->m_pBoard->GetPieceFromSquare( pntTo ) )	// Can't be our own army since CPiece::ValidMove would have failed
			return true;
	}

	return( false );	// invalid move
}

//////////////////////////////////////////////////////////////////////
// CRook
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Can this rook do this move

bool CRook::ValidMove( CPoint pntTo) 
{ 
	if( !CPiece::ValidMove(  pntTo ) )
		return( false );

	int dy = pntTo.y - m_curPos.y;
	int dx = pntTo.x - m_curPos.x;

	if( (!dx && dy)|| (dx && !dy) ) {
		// Direction is ok, see if we are not bumping into something
		int dxLoop = ((!dx) ? 0 : ((dx>0) ? 1 : -1 ) );
		int dyLoop = ((!dy) ? 0 : ((dy>0) ? 1 : -1 ) );

		int x = m_curPos.x + dxLoop;
		int y = m_curPos.y + dyLoop;

		while( ( y != pntTo.y ) || ( x != pntTo.x ) ) {
			if( m_pArmy->m_pBoard->GetPieceFromSquare( CPoint( x, y ) ) )
				return false;

			y += dyLoop;
			x += dxLoop;
		}
		m_HasMoved = true;
		return true;
	}

	return( false ); 
}

//////////////////////////////////////////////////////////////////////
// Move this rook to this square

bool CRook::MoveTo(CPoint pntTo)
{
	if( CPiece::MoveTo( pntTo ) ) {
		m_HasMoved = true;
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////
// CKnight
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Can this knight do this move

bool CKnight::ValidMove( CPoint pntTo) 
{ 
	if( !CPiece::ValidMove(  pntTo ) )
		return( false );

	int dy = pntTo.y - m_curPos.y;
	int dx = pntTo.x - m_curPos.x;

	if( ( abs( dy ) == 2 && abs( dx ) == 1 ) || ( abs( dy ) == 1 && abs( dx ) == 2 ) )
		return true;

	return( false ); 
}

//////////////////////////////////////////////////////////////////////
// CBishop
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Can this bishop do this move

bool CBishop::ValidMove( CPoint pntTo) 
{ 
	if( !CPiece::ValidMove( pntTo ) )
		return( false );

	int dy = pntTo.y - m_curPos.y;
	int dx = pntTo.x - m_curPos.x;

	if( abs(dx) == abs(dy) ) {
		// Direction is ok, see if we are not bumping into something
		int dxLoop = ((!dx) ? 0 : ((dx>0) ? 1 : -1 ) );
		int dyLoop = ((!dy) ? 0 : ((dy>0) ? 1 : -1 ) );

		int x = m_curPos.x + dxLoop;
		int y = m_curPos.y + dyLoop;

		while( ( y != pntTo.y ) || ( x != pntTo.x ) ) {
			if( m_pArmy->m_pBoard->GetPieceFromSquare( CPoint( x, y ) ) )
				return false;

			y += dyLoop;
			x += dxLoop;
		}

		return true;
	}

	return( false ); 
}

//////////////////////////////////////////////////////////////////////
// CQueen
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Can this Queen do this move

bool CQueen::ValidMove( CPoint pntTo) 
{ 
	if( !CPiece::ValidMove( pntTo ) )
		return( false );

	int dy = pntTo.y - m_curPos.y;
	int dx = pntTo.x - m_curPos.x;

	if( (!dx && dy)|| (dx && !dy) || (abs(dx) == abs(dy)) ) {

		// Direction is ok, see if we are not bumping into something
		int dxLoop = ((!dx) ? 0 : ((dx>0) ? 1 : -1 ) );
		int dyLoop = ((!dy) ? 0 : ((dy>0) ? 1 : -1 ) );

		int x = m_curPos.x + dxLoop;
		int y = m_curPos.y + dyLoop;

		while( ( y != pntTo.y ) || ( x != pntTo.x ) ) {
			if( m_pArmy->m_pBoard->GetPieceFromSquare( CPoint( x, y ) ) )
				return false;

			y += dyLoop;
			x += dxLoop;
		}

		return true;
	}

	return( false ); 
}

//////////////////////////////////////////////////////////////////////
// CKing
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Can this king do this move

bool CKing::ValidMove( CPoint pntTo) 
{ 
	if( !CPiece::ValidMove( pntTo ) )
		return( false );

	m_pRook = NULL;

	int dy = pntTo.y - m_curPos.y;
	int dx = pntTo.x - m_curPos.x;

	if( (abs(dy) == 1) || (abs(dx) == 1) )
		return true;

	if( !dy && ( abs(dx) == 2 ) && !m_HasMoved ) {
		// possible rochade

		m_pRook = dynamic_cast<CRook *> ( m_pArmy->PieceOnSquare( CPoint( ((dx>0) ? 'H' : 'A'), m_curPos.y ) ) );

		if( m_pRook ) {
			if( m_pRook->m_HasMoved ) {
				m_pRook = NULL;
				return false;
			}
			m_pntRook = CPoint( ((dx>0) ? 'F' : 'D' ), m_curPos.y );

			return true;
		}
	}

	return( false ); 
}

//////////////////////////////////////////////////////////////////////
// Move this king to this square

bool CKing::MoveTo(CPoint pntTo)
{
	if( CPiece::MoveTo( pntTo ) ) {
		if( m_pRook ) {
			m_pRook->MoveTo( m_pntRook );
			m_pArmy->m_iCastling = (m_pntRook.x == 'F' ? 1 : 2 );
		}

		m_HasMoved = true;
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////
// CArmy
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construct army

CArmy::CArmy( CBoard * pBoard, bool fIsBlack ) : m_fIsBlack( fIsBlack )
{
	m_pBoard = pBoard;

	lArmy.push_back( new CRook  ( this, CPoint( 'A', ( fIsBlack ? 8 : 1 ) ) ) );
	lArmy.push_back( new CKnight( this, CPoint( 'B', ( fIsBlack ? 8 : 1 ) ) ) );
	lArmy.push_back( new CBishop( this, CPoint( 'C', ( fIsBlack ? 8 : 1 ) ) ) );
	lArmy.push_back( new CQueen ( this, CPoint( 'D', ( fIsBlack ? 8 : 1 ) ) ) );
	lArmy.push_back( new CKing  ( this, CPoint( 'E', ( fIsBlack ? 8 : 1 ) ) ) );
	lArmy.push_back( new CBishop( this, CPoint( 'F', ( fIsBlack ? 8 : 1 ) ) ) );
	lArmy.push_back( new CKnight( this, CPoint( 'G', ( fIsBlack ? 8 : 1 ) ) ) );
	lArmy.push_back( new CRook  ( this, CPoint( 'H', ( fIsBlack ? 8 : 1 ) ) ) );

	for( char i='A'; i<='H'; i++ )
		lArmy.push_back( new CPawn( this, CPoint( i, ( fIsBlack ? 7 : 2 ) ) ) );

}

//////////////////////////////////////////////////////////////////////
// Delete army

CArmy::~CArmy()
{
	list<CPiece *>::iterator PieceIt;

	for( PieceIt = lArmy.begin(); PieceIt != lArmy.end(); PieceIt++ )
		delete (*PieceIt);

	lArmy.clear();
}

//////////////////////////////////////////////////////////////////////
// Paint army on board

void CArmy::DoPaint( CDC * pDC, CRect& rBoardRect, CDC * pMemDC, bool fReverse )
{
	list<CPiece *>::iterator PieceIt;

	for( PieceIt = lArmy.begin(); PieceIt != lArmy.end(); PieceIt++ ) {
		CPoint pntSquare( 
			rBoardRect.left + BORDERWIDTH + ( fReverse ? (7 - ((*PieceIt)->GetX() - 'A')) : ( ((*PieceIt)->GetX() - 'A') ) ) * SQUAREWIDTH, 
			rBoardRect.top  + BORDERWIDTH + ( fReverse ? ((*PieceIt)->GetY() - 1  ) : ( 7 - ((*PieceIt)->GetY() - 1  ) ) ) * SQUAREHEIGHT 
		);

		pDC->BitBlt( pntSquare.x, pntSquare.y, SQUAREWIDTH, SQUAREHEIGHT, pMemDC, (*PieceIt)->GetType() * SQUAREWIDTH, MASKOFFSET, SRCAND   );
		pDC->BitBlt( pntSquare.x, pntSquare.y, SQUAREWIDTH, SQUAREHEIGHT, pMemDC, (*PieceIt)->GetType() * SQUAREWIDTH, ( m_fIsBlack ? BLACKOFFSET : WHITEOFFSET), SRCINVERT);

	}
}

//////////////////////////////////////////////////////////////////////
// Is a piece of the army on this square

CPiece * CArmy::PieceOnSquare(CPoint Square)
{
	list<CPiece *>::iterator PieceIt;

	for( PieceIt = lArmy.begin(); PieceIt != lArmy.end(); PieceIt++ ) {
		if( (*PieceIt)->IsOnSquare( Square ) )
			return (*PieceIt);
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////
// Delete a piece from the army

bool CArmy::DeletePiece(CPoint Square)
{
	CPiece * pPiece = PieceOnSquare( Square );

	if( pPiece ) {
		delete( pPiece );
		lArmy.remove( pPiece );
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////
// Move a piece of the army

bool CArmy::MovePiece(CArmy *pOtherArmy, CPoint orgSquare, CPoint newSquare )
{
	list<CPiece *>::iterator PieceIt;

	CPiece * pPiece = NULL;

	m_iCastling   = 0;
	m_fCapture    = false;
	m_fCheckMate  = false;
	m_fAttackKing = false;
	m_fPawnPromo  = false;

	for( PieceIt = lArmy.begin(); PieceIt != lArmy.end(); PieceIt++ ) {
		if( (*PieceIt)->IsOnSquare( orgSquare ) ) {
			pPiece = (*PieceIt);
			break;
		}
	}

	if( !pPiece )
		return false;

	if( !pPiece->ValidMove( newSquare ) ) {
		::MessageBox( NULL, _T("Move not allowed"), _T("Error"), MB_ICONHAND|MB_OK|MB_DEFBUTTON1 );
		return false;
	}

	m_fCapture = pOtherArmy->DeletePiece( newSquare );
		

	if( ( pPiece->GetType() == PAWN ) && ( newSquare.y == ( m_fIsBlack ? 1 : 8 ) ) ) {
		// Pawn promotion
		lArmy.remove( pPiece );
		CQueen * pnewQueen = new CQueen( this, orgSquare );
		lArmy.push_back( pnewQueen );
		pPiece = pnewQueen;
		m_fPawnPromo = true;
	}

	pPiece->MoveTo( newSquare );

	FormatMove( pPiece->GetType(), orgSquare, newSquare );

	return true;
}

//////////////////////////////////////////////////////////////////////
// Calculate the total material value of the army

int CArmy::CalcTotalMaterial()
{
	int iTotalMaterial = 0;
	list<CPiece *>::iterator PieceIt;


	for( PieceIt = lArmy.begin(); PieceIt != lArmy.end(); PieceIt++ ) {
		if( (*PieceIt)->GetType( ) != KING )
			iTotalMaterial += aPieceValues[ (*PieceIt)->GetType() ];
	}

	return iTotalMaterial ;
}

//////////////////////////////////////////////////////////////////////
// Calculate the pawn value of the army

int CArmy::CalcPawnMaterial()
{

	int iPawnMaterial = 0;
	list<CPiece *>::iterator PieceIt;


	for( PieceIt = lArmy.begin(); PieceIt != lArmy.end(); PieceIt++ ) {
		if( (*PieceIt)->GetType( ) == PAWN )
			iPawnMaterial += aPieceValues[ PAWN ];
	}

	return iPawnMaterial ;
}

//////////////////////////////////////////////////////////////////////
//  Set mating if weakest player has less than the equivalence of two bishops 
//  and the advantage is at least a rook for a bishop

bool CArmy::IsMating(int iTotalMaterial, int iMaterial )
{
    return( ( iTotalMaterial - abs( iMaterial ) ) / 2 <= aPieceValues[ BISHOP ] * 2 ) 
				&& ( abs( iMaterial ) >= aPieceValues[ ROOK ] - aPieceValues[ BISHOP ] );
}

void CArmy::FormatMove(PIECETYPES Type, CPoint orgSquare, CPoint newSquare )
{

	CString strTemp;

	if( m_iCastling ) {
		m_strLastMove = ( m_iCastling == 1 ) ? _T("0 - 0") : _T("0 - 0 - 0");
		return;
	}

	m_strLastMove.Empty();

	if( !m_fPawnPromo )
		m_strLastMove.Format( _T("%c"), aPieceShorthand[ Type ] );

	strTemp.Format( _T("%c%d"), (orgSquare.x - 'A' + 'a'), orgSquare.y );
	m_strLastMove += strTemp;

	strTemp = m_fCapture ? _T(" x "): _T(" - ");
	m_strLastMove += strTemp;

	strTemp.Format( _T("%c%d"), (newSquare.x - 'A' + 'a'), newSquare.y );
	m_strLastMove += strTemp;

	if( m_fPawnPromo ) {
		strTemp.Format( _T("%c"), aPieceShorthand[ Type ] );
		m_strLastMove += strTemp;
	}

	if( m_fAttackKing ) {
		strTemp = _T("†");
		m_strLastMove += strTemp;
	}

	if( m_fCheckMate ) {
		strTemp = _T("†");
		m_strLastMove += strTemp;
	}

	return;
}

CPoint CArmy::GetKingPos()
{
	list<CPiece *>::iterator PieceIt;

	for( PieceIt = lArmy.begin(); PieceIt != lArmy.end(); PieceIt++ ) {
		if( (*PieceIt)->GetType( ) == KING )
			return( CPoint( (*PieceIt)->GetX(), (*PieceIt)->GetY() ) );
	}
	return( CPoint( 0, 0 ) );
}

int CArmy::CalcAttackTable( int iMyMaterial, int iOtherMaterial, CPoint pntOtherKing, int iPawnMaterial ) 
{
	CPoint pntSquare;

	char distan[8] = { 3, 2, 1, 0, 0, 1, 2, 3 };
	const unsigned char squarerankvalue[8]     = { 0, 0, 0, 0, 1, 2, 4, 4 };
	CPoint apntSurround[] = { (-1, -1), (-1, 0), (-1, 1), (0, -1), (0, 1), (1, -1), (1, 0), (1, 1) };

	int totalmaterial     = iMyMaterial + iOtherMaterial;
	int material          = iMyMaterial - iOtherMaterial;

	bool mating           = IsMating( totalmaterial, material );

    int materiallevel     = max( 0, totalmaterial - 0x2000 ) / 0x100;

    //losingcolor = ( ( material < 0 ) ? white : black );

    /*  Calculate attack values of each square (importance of each square)  */
	
    for( pntSquare.x = 'A'; pntSquare.x <= 'H'; pntSquare.x++ ) {
        for( pntSquare.y = 1; pntSquare.y <= 8; pntSquare.y++ ) {

            AttackValue[ ( m_fIsBlack ? ( 'H' - pntSquare.x ) : ( pntSquare.x - 'A' ) ) * 8 + pntSquare.y - 1 ] =
			           ( ( squarerankvalue[ pntSquare.x - 'A' ] * 3 * ( materiallevel + 8 ) ) >> 5 )	/*  Rank importrance  */ 
			           + max( 0, 8 - 3 * ( distan[ pntSquare.x - 'A' ] + distan[ pntSquare.y - 1 ] ) ); /*  center importance */
        }
	}

	/*  Add importance of the 8 squares around the opponent's King  */
    if( materiallevel > 0 ) {

		for( int dir = 0; dir < 8; dir++ ) {

			pntSquare = pntOtherKing + apntSurround[ dir ];

			if( ( pntSquare.x >= 'A' ) && ( pntSquare.x <= 'H' ) && ( pntSquare.y >= 1 ) && ( pntSquare.y <= 8 ) )
	            AttackValue[ ( pntSquare.x - 'A' ) * 8 + pntSquare.y - 1 ] += ((12 * ( materiallevel + 8 ) ) >> 5 );
		}
	}


    /*  Initialise PVControl  */
#if 0
    for( pntSquare.x = 'A'; pntSquare.x <= 'H'; pntSquare.x++ ) {
        for( pntSquare.y = 1; pntSquare.y <= 8; pntSquare.y++ ) {
                pvcontrol[ ROOK ][ pntSquare ]   = 0;
                pvcontrol[ BISHOP ][ pntSquare ] = 0;
		}
	}

    /*  Calculate PVControl  */
    for( pntSquare.x = 'A'; pntSquare.x <= 'H'; pntSquare.x++ ) {
        for( pntSquare.y = 1; pntSquare.y <= 8; pntSquare.y++ ) {

            for( color = white; color <= black; ((int)color)++ ) {
                for( dir = 0; dir < 8; dir++ ) {

					// rook attacks vertical and horizontal, bishop attacks diagonal, queen from all directions
					PIECETYPES CompareType = ( ( apntSurround[ dir ].x + apntSurround[ dir ].y ) % 2 ) ? BISHOP : ROOK;

                /*  Count value of all attacs from the square int the Direction.
                    The Value of attacking a Square is Found in ATTACKVAL.
                    Indirect Attacks (e.g. a Rook attacking through another Rook) counts for a Normal attack,
                    Attacks through another Piece counts half  */

                    cnt = 0;
                    pntDirSquare = pntSquare;
                    direct = 1;
                    do {
                        sq += apntSurround[ dir ];
						if( ( pntSquare.x >= 'A' ) && ( pntSquare.x <= 'H' ) && ( pntSquare.y >= 1 ) && ( pntSquare.y <= 8 ) )
							break;

                        cnt += ( direct ? AttackValue[ pntDirSquare ] : ( AttackValue[ pntDirSquare ] >> 1 ) );

						direct = 0;
						for( PieceIt = lArmy.begin(); PieceIt != lArmy.end(); PieceIt++ ) {
							if( (*PieceIt)->IsOnSquare( orgSquare ) ) {

								PIECETYPES PieceType = (*PieceIt)->GetType();
								if( PieceType == CompareType || PieceType == QUEEN )
									direct = 1;
								


                        if( ( Board[ sq ].piece != empty ) && ( ( Board[ sq ].piece != piececount ) && ( Board[ sq ].piece != queen ) ) )
                            direct = 0;

                    } while( Board[ sq ].piece != pawn );

                    pvcontrol[ color ][ piececount ][ square ] += (cnt >> 2);
                }
            }
		}
	}
#endif






	return 0;
}
