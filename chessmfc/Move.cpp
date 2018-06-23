// Move.cpp: implementation of the CMove class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "chessmfc.h"
#include "Move.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMove::CMove()
{

}

CMove::~CMove()
{

}

bool CMove::operator ==(CMove *other)
{

	if( m_fCaptured != other->m_fCaptured )
		return false;

	if( m_pntFrom != other->m_pntFrom )
		return false;

	if( m_pntTo != other->m_pntTo )
		return false;

	if( m_Type != other->m_Type )
		return false;

	return true;
}

#define SHORTCASTLE   0x00000001
#define LONGCASTLE    0x00000010

CString CMove::GetText()
{
/*
	CString strTemp;
	CString strReturn;

	if( m_iFlags & CASTLEMASK )
		return( ( m_iFlags & SHORTCASTLE ) ? _T("0 - 0") : _T("0 - 0 - 0") );

	m_strReturn.Empty();

	if( !( m_iFlags & PAWNPROMOMASK ) )
		strReturn.Format( _T("%c"), aPieceShorthand[ Type ] );

	strTemp.Format( _T("%c%d"), (orgSquare.x - 'A' + 'a'), orgSquare.y );
	strReturn += strTemp;

	strTemp = m_fCapture ? _T(" x "): _T(" - ");
	strReturn += strTemp;

	strTemp.Format( _T("%c%d"), (newSquare.x - 'A' + 'a'), newSquare.y );
	strReturn += strTemp;

	if( m_fPawnPromo ) {
		strTemp.Format( _T("%c"), aPieceShorthand[ Type ] );
		strReturn += strTemp;
	}

	if( m_fAttackKing ) {
		strTemp = _T("†");
		strReturn += strTemp;
	}

	if( m_fCheckMate ) {
		strTemp = _T("†");
		strReturn += strTemp;
	}

	return strReturn;
*/
	return( _T("Move") );

}
