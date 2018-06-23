// ChessView.cpp : implementation of the CChessView class
//

#include "stdafx.h"
#include "chessmfc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define BOARDLEFT    50
#define BOARDTOP     50

#define BORDERWIDTH  3
#define SQUAREHEIGHT 36
#define SQUAREWIDTH  36

#define MASKOFFSET  0
#define BLACKOFFSET SQUAREHEIGHT
#define WHITEOFFSET (2 * SQUAREHEIGHT)

/////////////////////////////////////////////////////////////////////////////
// CChessView

IMPLEMENT_DYNCREATE(CChessView, CView)

BEGIN_MESSAGE_MAP(CChessView, CView)
	//{{AFX_MSG_MAP(CChessView)
	ON_COMMAND(ID_DISPLAY_REVERSEBOARD, OnDisplayReverseboard)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_CREATE()
	ON_COMMAND(ID_DISPLAY_SHOWBESTLINE, OnDisplayShowbestline)
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(ID_DISPLAY_COLORS_BACKGROUND, ID_DISPLAY_COLORS_WHITESQUARECOLOR, OnDisplayColors)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CChessView construction/destruction

CChessView::CChessView()
{
	// TODO: add construction code here
	m_clrBlack = 0x00C0C0C0;
	m_clrWhite = 0x00FFFFFF;
	m_clrBack  = 0x00FF0000;

	m_penLtGrey.CreatePen( PS_SOLID, 1, RGB(192, 192, 192) );
    m_penGrey.CreatePen( PS_SOLID, 1, RGB( 128, 128, 128 ) );

	m_bmpPieces.LoadBitmap( IDB_BITMAP );

	m_fReverse = false;
}

CChessView::~CChessView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CChessView drawing

void CChessView::OnDraw(CDC* pDC)
{
	CChessDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CRect  ClientRect;
    POINT  Array[3];
    CPen   * pOldPen;
    CBrush * pOldBrush;
	CBrush BkBrush( m_clrBack );
	CBrush BlackBrush( m_clrBlack );
	CBrush WhiteBrush( m_clrWhite );
	CDC    MemDC;
	CRect& BoardRect = CRect( BOARDLEFT, BOARDTOP, BOARDLEFT + (8 * SQUAREWIDTH) + (2 * BORDERWIDTH), BOARDTOP + (8 * SQUAREHEIGHT) + (2 * BORDERWIDTH) );

	CMemDC dcCopy( pDC );

	GetClientRect( &ClientRect );

	pOldBrush = (CBrush *)dcCopy.SelectObject( &BkBrush );
    pOldPen   = (CPen   *)dcCopy.SelectStockObject( NULL_PEN );

	// Paint the client area
	dcCopy.SetBkColor( m_clrBack );
	dcCopy.Rectangle( &ClientRect );

	// Paint the bOARD
	dcCopy.SelectStockObject( LTGRAY_BRUSH );
    dcCopy.SelectStockObject( WHITE_PEN );
    dcCopy.Rectangle( BoardRect );

    dcCopy.SelectObject( &m_penLtGrey );
    dcCopy.Rectangle( BoardRect.left + 1, BoardRect.top + 1, BoardRect.right - 1, BoardRect.bottom - 1);

    Array[0].x = BoardRect.left  + 2; Array[0].y = BoardRect.bottom - 3;
	Array[1].x = BoardRect.right - 3; Array[1].y = BoardRect.bottom - 3; 
    Array[2].x = BoardRect.right - 3; Array[2].y = BoardRect.top    + 2;
    dcCopy.SelectStockObject( WHITE_PEN );
    dcCopy.Polyline( Array, 3);

    Array[0].x = BoardRect.left; 	  Array[0].y = BoardRect.bottom - 1;
	Array[1].x = BoardRect.right - 1; Array[1].y = BoardRect.bottom - 1; 
    Array[2].x = BoardRect.right - 1; Array[2].y = BoardRect.top;
    dcCopy.SelectObject( &m_penGrey );
    dcCopy.Polyline( Array, 3);

	Array[0].x = BoardRect.left  + 2; Array[0].y = BoardRect.bottom - 3;
    Array[1].x = BoardRect.left  + 2; Array[1].y = BoardRect.top    + 2;
    Array[2].x = BoardRect.right - 3; Array[2].y = BoardRect.top    + 2; 
    dcCopy.Polyline( Array, 3);

	// Paint the lettering next to the board
	if( m_fReverse ) {
		for( char i= 'A'; i <='H'; i++ )
			dcCopy.TextOut( (BoardRect.right - 24) - (int)(i-'A')*SQUAREWIDTH, BoardRect.top - 16, i );

		for( int iRow=0; iRow<8; iRow++ )
			dcCopy.TextOut( BoardRect.right + 5, BoardRect.top + 12 + iRow*SQUAREHEIGHT, iRow+'1' );
	} else {
		for( char i= 'A'; i <='H'; i++ )
			dcCopy.TextOut( (BoardRect.left + SQUAREWIDTH / 2) + (int)(i-'A')*SQUAREWIDTH, BoardRect.bottom, i );

		for( int iRow=0; iRow<8; iRow++ )
			dcCopy.TextOut( BoardRect.left - 10, BoardRect.bottom - (SQUAREHEIGHT- 6) - iRow*SQUAREHEIGHT, iRow+'1' );
	}

	// Paint the squares in the board
	CRect& InBoard = CRect( BoardRect.left + BORDERWIDTH, BoardRect.top - BORDERWIDTH, BoardRect.right + BORDERWIDTH, BoardRect.bottom - BORDERWIDTH );

	int iSquareWidth = (InBoard.right - InBoard.left)/8;
	int iSquareHeight = (InBoard.top - InBoard.bottom)/8;

	CRect& rect = CRect( InBoard.left, InBoard.bottom, InBoard.left + iSquareWidth, InBoard.bottom + iSquareHeight );

	for( int x=0; x<8; x++ ) {
		for( int y=0; y<8; y++ ) {

			dcCopy.SelectObject( ( (x%2) != (y%2) ) ? WhiteBrush : BlackBrush );
			dcCopy.Rectangle( &rect );

			rect.left   = rect.right;
			rect.right += iSquareWidth;
		}

		rect.left    = InBoard.left;
		rect.right   = InBoard.left + iSquareWidth;
		rect.top     = rect.bottom;
		rect.bottom += iSquareHeight;
	}

	// Paint all the pieces on the board
	MemDC.CreateCompatibleDC( pDC );
	MemDC.SelectObject( &m_bmpPieces );

	pDoc->PaintArmys( &dcCopy, BoardRect, &MemDC, m_fReverse );

    dcCopy.SelectObject( pOldBrush );
    dcCopy.SelectObject( pOldPen   );
}

/////////////////////////////////////////////////////////////////////////////
// CChessView diagnostics

#ifdef _DEBUG
void CChessView::AssertValid() const
{
	CView::AssertValid();
}

void CChessView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CChessDoc* CChessView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CChessDoc)));
	return (CChessDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChessView message handlers

void CChessView::OnDisplayReverseboard() 
{
	m_fReverse = !m_fReverse;
	Invalidate();
	
}

void CChessView::OnDisplayColors( UINT id ) 
{
	COLORREF * pColor;

	switch( id ) {
	case ID_DISPLAY_COLORS_BACKGROUND       : pColor = &m_clrBack;  break;
	case ID_DISPLAY_COLORS_BLACKSQUARECOLOR : pColor = &m_clrBlack; break;
	case ID_DISPLAY_COLORS_WHITESQUARECOLOR : pColor = &m_clrWhite; break;
	default : return;
	}

	CColorDialog dlg( *pColor, 0, this );
	
	if( dlg.DoModal() == IDOK ) {
		*pColor = dlg.GetColor();
		Invalidate();
	}

	return;
}

void CChessView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_pntOrigin.x = (( point.x - BOARDLEFT - BORDERWIDTH ) / SQUAREWIDTH) + 'A';
	m_pntOrigin.y = 8 - (( point.y - BOARDTOP - BORDERWIDTH ) / SQUAREHEIGHT);
	
	CView::OnLButtonDown(nFlags, point);
}

void CChessView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CPoint newpnt( ( (( point.x - BOARDLEFT - BORDERWIDTH ) / SQUAREWIDTH)) + 'A', (8 - (( point.y - BOARDTOP - BORDERWIDTH ) / SQUAREHEIGHT)) );

	CChessDoc* pDoc = GetDocument();

	pDoc->Move( m_pntOrigin, newpnt );
	Invalidate();

	CView::OnLButtonUp(nFlags, point);
}


int CChessView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	m_wndInfo.Create( IDD_INFO, this );
	m_wndInfo.ShowWindow( SW_SHOWNORMAL );

	return 0;
}

void CChessView::OnDisplayShowbestline() 
{
	// TODO: Add your command handler code here
}

void CChessView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	// TODO: Add your specialized code here and/or call the base class
	CChessDoc* pDoc = GetDocument();

	if( lHint )
		Invalidate();

	for( int iCount = IDC_INFOTURN; iCount <= IDC_INFOMESSAGE; iCount++ )
		m_wndInfo.SetInfo( iCount, pDoc->m_straInfoText[ iCount - IDC_INFOTURN ] );
	
	
}
