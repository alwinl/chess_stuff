// Move.h: interface for the CMove class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOVE_H__BF58081E_FDFC_11D4_8C7D_EAB763000000__INCLUDED_)
#define AFX_MOVE_H__BF58081E_FDFC_11D4_8C7D_EAB763000000__INCLUDED_

#include "Piece.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMove : public CObject  
{
public:
	CString GetText();
	CMove();
	virtual ~CMove();

	bool operator==( CMove * other );

	bool m_fCastle;					
	bool m_fCaptured;

	CPoint m_pntFrom;
	CPoint m_pntTo;
	PIECETYPES m_Type;
};

#endif // !defined(AFX_MOVE_H__BF58081E_FDFC_11D4_8C7D_EAB763000000__INCLUDED_)
