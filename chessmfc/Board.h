// Board.h: interface for the CSquare class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BOARD_H__A5365901_076B_11D5_8C89_C25214000000__INCLUDED_)
#define AFX_BOARD_H__A5365901_076B_11D5_8C89_C25214000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPiece;

class CSquare : public CPoint  
{
public:
	CSquare();
	virtual ~CSquare();
	CPiece * m_pPiece;

};


class CBoard : public CObject  
{
public:
	bool TakePieceFromSquare( CPoint pntSquare );
	CPiece * GetPieceFromSquare( CPoint pntSquare );
	bool SetPieceOnSquare( CPoint pntSquare, CPiece * pPiece );
	CSquare Squares[ 64 ];
	CBoard();
	virtual ~CBoard();

};

#endif // !defined(AFX_BOARD_H__A5365901_076B_11D5_8C89_C25214000000__INCLUDED_)
