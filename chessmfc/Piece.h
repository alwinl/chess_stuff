// Piece.h: interface for the CPiece class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PIECE_H__EC174B47_F87A_11D4_8C76_B04984000000__INCLUDED_)
#define AFX_PIECE_H__EC174B47_F87A_11D4_8C76_B04984000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CChessDoc;
class CArmy;

typedef	enum { PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING, EMPTY } PIECETYPES;

//////////////////////////////////////////////////////////////////////
// Base class for all pieces

class CPiece
{
public:
	CPiece( CArmy * pArmy, CPoint curPos );

	virtual ~CPiece();

	int GetX() { return m_curPos.x; };
	int GetY() { return m_curPos.y; };
	bool IsOnSquare( CPoint pntCmp ) { return( (pntCmp == m_curPos) ? true : false ); };

	virtual bool MoveTo( CPoint pntMoveTo );

	virtual bool ValidMove( CPoint pntTo);
	virtual PIECETYPES GetType() { return EMPTY; };

	bool operator==( CPiece* other );

protected:
	CPoint     m_curPos;
	CArmy*     m_pArmy;
};

//////////////////////////////////////////////////////////////////////
// CPawn

class CPawn : public CPiece  
{
public:
	CPawn( CArmy * pArmy, CPoint curPos) : CPiece( pArmy, curPos ) {};
	virtual ~CPawn() {};

	virtual bool ValidMove( CPoint pntTo);
	virtual PIECETYPES GetType() { return PAWN; };

protected:
};

//////////////////////////////////////////////////////////////////////
// CRook

class CRook : public CPiece  
{
public:
	CRook( CArmy * pArmy, CPoint curPos) : CPiece( pArmy, curPos ) { m_HasMoved = false; };
	virtual ~CRook() {};

	virtual bool MoveTo( CPoint pntTo );
	virtual bool ValidMove( CPoint pntTo);
	virtual PIECETYPES GetType() { return ROOK; };

	bool m_HasMoved;
protected:
};

//////////////////////////////////////////////////////////////////////
// CKnight

class CKnight : public CPiece  
{
public:
	CKnight( CArmy * pArmy, CPoint curPos) : CPiece( pArmy, curPos ) {};
	virtual ~CKnight() {};

	virtual bool ValidMove( CPoint pntTo);
	virtual PIECETYPES GetType() { return KNIGHT; };

protected:
};

//////////////////////////////////////////////////////////////////////
// CBishop

class CBishop : public CPiece  
{
public:
	CBishop( CArmy * pArmy, CPoint curPos) : CPiece( pArmy, curPos ) {};
	virtual ~CBishop() {};

	virtual bool ValidMove( CPoint pntTo);
	virtual PIECETYPES GetType() { return BISHOP; };

protected:
};

//////////////////////////////////////////////////////////////////////
// CQueen

class CQueen : public CPiece  
{
public:
	CQueen( CArmy * pArmy, CPoint curPos) : CPiece( pArmy, curPos ) {};
	virtual ~CQueen() {};

	virtual bool ValidMove( CPoint pntTo);
	virtual PIECETYPES GetType() { return QUEEN; };

protected:
};

//////////////////////////////////////////////////////////////////////
// CKing

class CKing : public CPiece  
{
public:
	CKing( CArmy * pArmy, CPoint curPos) : CPiece( pArmy, curPos ) { m_HasMoved = false; };
	virtual ~CKing() {};

	virtual bool MoveTo( CPoint pntTo );
	virtual bool ValidMove( CPoint pntTo);
	virtual PIECETYPES GetType() { return KING; };

protected:
	CPoint m_pntRook;
	CRook * m_pRook;
	bool m_HasMoved;
};

class CArmy
{
public:
	int CalcAttackTable( int iMyMaterial, int iOtherMaterial, CPoint pntOtherKing, int iPawnMaterial );

	int AttackValue[ 8*8 ];
	CPoint GetKingPos();
	bool IsMating( int iTotalMaterial, int iMaterial );
	int CalcPawnMaterial();
	int CalcTotalMaterial();
	CPiece* GetPiece( CPoint Square );

	CArmy( CBoard * pBoard, bool fIsBlack );
	virtual ~CArmy();

	bool IsBlack() { return m_fIsBlack; };
	void DoPaint( CDC * pDC, CRect& rBoardRect, CDC * pMemDC, bool fReverse );
	bool MovePiece(CArmy *OtherArmy, CPoint orgSquare, CPoint newSquare );
	bool DeletePiece(CPoint Square);
	CPiece * PieceOnSquare( CPoint Square );

	int m_iCastling;
	CString m_strLastMove;
	CBoard * m_pBoard;

protected:
	bool m_fPawnPromo;
	bool m_fCapture;
	bool m_fCheckMate;
	bool m_fAttackKing;
	void FormatMove( PIECETYPES Type, CPoint orgSquare, CPoint newSquare );
	bool m_fIsBlack;
	list<CPiece *> lArmy;
};

#endif // !defined(AFX_PIECE_H__EC174B47_F87A_11D4_8C76_B04984000000__INCLUDED_)
