// Board.cpp: implementation of the CSquare class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "chessmfc.h"
#include "Board.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSquare::CSquare()
{
	m_pPiece = NULL;
}

CSquare::~CSquare()
{

}

//////////////////////////////////////////////////////////////////////
// CBoard Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBoard::CBoard()
{

}

CBoard::~CBoard()
{

}

bool CBoard::SetPieceOnSquare(CPoint pntSquare, CPiece * pPiece )
{
	if( ( pntSquare.x < 'A' ) || ( pntSquare.x > 'H' ) || ( pntSquare.y < 1 ) && ( pntSquare.y > 8 ) )
		return false;

	Squares[ ( pntSquare.x - 'A' ) * 8 + pntSquare.y - 1 ].m_pPiece = pPiece;
	return true;
}

CPiece * CBoard::GetPieceFromSquare(CPoint pntSquare)
{
	if( ( pntSquare.x < 'A' ) || ( pntSquare.x > 'H' ) || ( pntSquare.y < 1 ) && ( pntSquare.y > 8 ) )
		return NULL;

	return( Squares[ ( pntSquare.x - 'A' ) * 8 + pntSquare.y - 1 ].m_pPiece );

}

bool CBoard::TakePieceFromSquare(CPoint pntSquare)
{
	if( ( pntSquare.x < 'A' ) || ( pntSquare.x > 'H' ) || ( pntSquare.y < 1 ) && ( pntSquare.y > 8 ) )
		return false;

	Squares[ ( pntSquare.x - 'A' ) * 8 + pntSquare.y - 1 ].m_pPiece = NULL;
	return true;
}
