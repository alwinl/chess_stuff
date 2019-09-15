// ObjectWindows - (C) Copyright 1992 by Borland International

#include <owl.h>
#include <static.h>
#include "wcdefs.h"
#include "info.h"
#include "externs.h"

short LEFTMARGIN = 6;
short SEPERATOR = 4;
#define MAXLINE    (INFOXSIZE - LEFTMARGIN -1)

#define COLORX     (LEFTMARGIN + CHARSIZE * 6)
#define COLORY     (SEPERATOR + MYFRAMESIZE)
#define COLORW     (CHARSIZE * 6)
#define TIMERX    (LEFTMARGIN + CHARSIZE * 6)
#define TIMERY    (COLORY + LINESIZE + SEPERATOR)
#define TIMERW    (MAXLINE - TIMERX)
#define WHITEINFOX    (LEFTMARGIN + CHARSIZE * 7)
#define WHITEINFOY    (TIMERY + LINESIZE + SEPERATOR * 2)
#define WHITEINFOW    (MAXLINE - WHITEINFOX)
#define BLACKINFOX    (WHITEINFOX)
#define BLACKINFOY    (WHITEINFOY + LINESIZE + SEPERATOR)
#define BLACKINFOW    (WHITEINFOW)
#define LEVELX    (LEFTMARGIN + CHARSIZE * 7)
#define LEVELY    (BLACKINFOY + LINESIZE + SEPERATOR)
#define LEVELW    (MAXLINE - LEVELX)
#define VALUEX    (LEFTMARGIN + CHARSIZE * 7)
#define VALUEY    (LEVELY + LINESIZE + SEPERATOR * 2)
#define VALUEW    (MAXLINE - VALUEX)
#define NODEX     (LEFTMARGIN + CHARSIZE * 8)
#define NODEY     (VALUEY + LINESIZE + SEPERATOR)
#define NODEW     (MAXLINE - NODEX)
#define SECX      (LEFTMARGIN + CHARSIZE * 7)
#define SECY      (NODEY + LINESIZE + SEPERATOR)
#define SECW      (MAXLINE - SECX)
#define DEPTHY    (SECY + LINESIZE + SEPERATOR)
#define DEPTHX    (LEFTMARGIN + CHARSIZE * 7)
#define DEPTHW    (MAXLINE - DEPTHX)
#define STATBESTY (DEPTHY + LINESIZE + SEPERATOR * 2)
#define BESTX     (LEFTMARGIN)
#define BESTY     (STATBESTY + LINESIZE + 1)
#define BESTW     (MAXLINE - LEFTMARGIN)
#define BESTH     (LINESIZE * 3 + SEPERATOR * 3 - 9)
#define MESSX     (LEFTMARGIN)
#define MESSY     (BESTY + BESTH + SEPERATOR * 2)
#define MESSW     (MAXLINE - MESSX)


#define EmptyStr  ""


//--------------- TNoEraseBkGndStatic methods -------------------

TNoEraseBkGndStatic::TNoEraseBkGndStatic( PTWindows AParent, int AnId, LPSTR ATitle, int X,
        int Y, int W, int H, WORD ATextLen, PTModule AModule = NULL ) : TStatic( AParent, AnId, ATitle, X, Y, W, H, ATextLen )
{
    DoErase = true;
    LastLen = 0;
}

void TNoEraseBkGndStatic::WMEraseBkGnd( RTMessage msg )
{
    if( !DoErase ) {
        msg.Result = 0;
        return;
    }

    HDC hDC = HDC( msg.WParam );
    RECT Rect;
    HBRUSH hOldBrush = ( HBRUSH )SelectObject( hDC, GetStockObject( LTGRAY_BRUSH ) );

    GetClientRect( HWindow, &Rect );
    PatBlt( hDC, Rect.left, Rect.top, Rect.right - Rect.left, Rect.bottom - Rect.top, PATCOPY );
    SelectObject( hDC, hOldBrush );
    DoErase = false;

    msg.Result = 1;
}


void TNoEraseBkGndStatic::SetText( LPSTR text )
{
    int Len = strlen( text );
    if( Len < LastLen )
        DoErase = true;

    TStatic::SetText( text );
    LastLen = Len;
}



//---------------- TInfoWindow Methods -------------------------

TInfoWindow::TInfoWindow( PTWindowsObject AParent, LPSTR ATitle ) : TWindow( AParent, ATitle )
{
    LEFTMARGIN = MYFRAMESIZE + CHARSIZE/2;
    SEPERATOR  = LINESIZE/4;
    INFOXSIZE  = ( 28 * CHARSIZE ) + ( 2 * LEFTMARGIN );
    INFOYSIZE  = MESSY + LINESIZE + SEPERATOR + MYFRAMESIZE;

    short temp = MAXBDSIZE * SQUARE_SIZE + ( 2 * MYFRAMESIZE );
    INFOYSIZE = ( temp > INFOYSIZE ) ? temp : INFOYSIZE;

    Attr.Style |= WS_CHILD | WS_VISIBLE;
    Attr.X      = ( 3 * BORDERSIZE ) + ( MAXBDSIZE * SQUARE_SIZE ) + ( 2 * MYFRAMESIZE );
    Attr.Y      = BORDERSIZE;
    Attr.W      = INFOXSIZE;
    Attr.H      = INFOYSIZE;

    InfoRect.right  = INFOXSIZE;
    InfoRect.bottom = INFOYSIZE;
    InfoRect.left   = 0;
    InfoRect.top    = 0;

    new TStatic( this, -1, " Turn:",     LEFTMARGIN,   COLORY,     CHARSIZE * 6,  LINESIZE, 5 );
    new TStatic( this, -1, " White:",    LEFTMARGIN,   WHITEINFOY, CHARSIZE * 7,  LINESIZE, 4 );
    new TStatic( this, -1, " Black:",    LEFTMARGIN,   BLACKINFOY, CHARSIZE * 7,  LINESIZE, 6 );
    new TStatic( this, -1, " Time:",     LEFTMARGIN,   TIMERY,     CHARSIZE * 6,  LINESIZE, 5 );
    new TStatic( this, -1, " Level:",    LEFTMARGIN,   LEVELY,     CHARSIZE * 13, LINESIZE, 12 );
    new TStatic( this, -1, " Value:",    LEFTMARGIN,   VALUEY,     CHARSIZE * 7,  LINESIZE, 6 );
    new TStatic( this, -1, " Nodes:",    LEFTMARGIN,   NODEY,      CHARSIZE * 8,  LINESIZE, 7 );
    new TStatic( this, -1, " N/Sec:",    LEFTMARGIN,   SECY,       CHARSIZE * 9,  LINESIZE, 9 );
    new TStatic( this, -1, " Depth:",    LEFTMARGIN,   DEPTHY,     CHARSIZE * 7,  LINESIZE, 6 );
    new TStatic( this, -1, " Bestline:", LEFTMARGIN-2, STATBESTY,  MAXLINE - 2,   LINESIZE, 9 );

    Color     = new TStatic( this, -1, EmptyStr, COLORX, COLORY, COLORW, LINESIZE, 6 );
    WhiteInfo = new TStatic( this, -1, EmptyStr, WHITEINFOX, WHITEINFOY, WHITEINFOW, LINESIZE, WHITEINFOW/CHARSIZE );
    BlackInfo = new TStatic( this, -1, EmptyStr, BLACKINFOX, BLACKINFOY, BLACKINFOW, LINESIZE, BLACKINFOW / CHARSIZE );
    Timer     = new TNoEraseBkGndStatic( this, -1, EmptyStr, TIMERX, TIMERY, TIMERW, LINESIZE, TIMERW /CHARSIZE );
    txtLevel  = new TStatic( this, -1, EmptyStr, LEVELX, LEVELY, LEVELW, LINESIZE, LEVELW / CHARSIZE );
    //Iteration     = new TStatic( this, -1, EmptyStr, LEVELX, LEVELY, LEVELW, LINESIZE, LEVELW / CHARSIZE );
    Value     = new TStatic( this, -1, EmptyStr, VALUEX, VALUEY, VALUEW, LINESIZE, VALUEW / CHARSIZE );
    txtNodes     = new TStatic( this, -1, EmptyStr, NODEX, NODEY, NODEW, LINESIZE, NODEW / CHARSIZE );
    Seconds   = new TStatic( this, -1, EmptyStr, SECX, SECY, SECW, LINESIZE, SECW / CHARSIZE );
    Depth     = new TNoEraseBkGndStatic( this, -1, EmptyStr, DEPTHX, DEPTHY, DEPTHW, LINESIZE, DEPTHW /CHARSIZE );
    BestLine  = new TStatic( this, -1, EmptyStr, BESTX, BESTY, BESTW, BESTH, ( ( BESTW / CHARSIZE ) * 3 ) );
    txtMessage   = new TStatic( this, -1, EmptyStr, MESSX, MESSY, MESSW, LINESIZE, MESSW / CHARSIZE );
}

void TInfoWindow::Paint( HDC PaintDC, PAINTSTRUCT _FAR & /*unused*/ );
{
    DrawFrame( PaintDC, InfoRect );
    DrawInfoFrame( PaintDC );
    DrawStaticFrames( PaintDC );
}

void TInfoWindow::WMControlColor( TMessage& msg )
{
    SetBkColor( HDC( msg.WParam ), RGB( 192, 192, 192 ) );
    msg.Result = ( long )GetStockObject( LTGRAY_BRUSH );
}

void TInfoWindow::Reset()
{
    Color->SetText( "White" );
    WhiteInfo->Clear();
    BlackInfo->Clear();
    txtLevel->Clear();
    Value->Clear();
    txtNodes->Clear();
    Seconds->Clear();
    BestLine->Clear();
    txtMessage->Clear();
}

void TInfoWindow::IterReset()
{
    Color->SetText( "White" );
    Value->Clear();
    txtNodes->Clear();
    Seconds->Clear();
    BestLine->Clear();
}

void TInfoWindow::DrawSFrame( HDC hDC, RECT *rect )
{
    HPEN hPen, hOldPen;
    HBRUSH hOldBrush;
    POINT pArray[3];

    hOldBrush = ( HBRUSH )SelectObject( hDC, GetStockObject( LTGRAY_BRUSH ) );
    PatBlt( hDC, rect->left, rect->top, rect->right - rect->left, rect->bottom - rect->top, PATCOPY );
    SelectObject( hDC, hOldBrush );

    hPen = CreatePen( PS_SOLID, 1, RGB( 128, 128, 128 ) );

    hOldPen = ( HPEN )SelectObject( hDC, hPen );

    pArray[0].x = rect->left;
    pArray[0].y = rect->bottom;
    pArray[1].x = rect->left;
    pArray[1].y = rect->top;
    pArray[2].x = rect->right;
    pArray[2].y = rect->top;

    Polyline( hDC, pArray, 3 );

    DeleteObject( SelectObject( hDC, GetStockObject( WHITE_PEN ) ) );

    pArray[1].x = rect->right;
    pArray[1].y = rect->bottom;
    pArray[2].y = rect->top - 1;

    Polyline( hDC, pArray, 3 );
    SelectObject( hDC, hOldPen );
}

void TInfoWindow::DrawStaticFrames( HDC hDC )
{
    RECT rect;

    rect.left = LEFTMARGIN - 1;
    rect.right = MAXLINE + 1;

    rect.top = COLORY - 1; rect.bottom = rect.top + LINESIZE + 2; DrawSFrame( hDC, &rect );
    rect.top = WHITEINFOY - 1; rect.bottom = rect.top + LINESIZE + 2; DrawSFrame( hDC, &rect );
    rect.top = BLACKINFOY - 1; rect.bottom = rect.top + LINESIZE + 2; DrawSFrame( hDC, &rect );
    rect.top = TIMERY - 1; rect.bottom = rect.top + LINESIZE + 2; DrawSFrame( hDC, &rect );
    rect.top = LEVELY - 1; rect.bottom = rect.top + LINESIZE + 2; DrawSFrame( hDC, &rect );
    rect.top = VALUEY - 1; rect.bottom = rect.top + LINESIZE + 2; DrawSFrame( hDC, &rect );
    rect.top = NODEY - 1; rect.bottom = rect.top + LINESIZE + 2; DrawSFrame( hDC, &rect );
    rect.top = SECY - 1; rect.bottom = rect.top + LINESIZE + 2; DrawSFrame( hDC, &rect );
    rect.top = DEPTHY - 1; rect.bottom = rect.top + LINESIZE + 2; DrawSFrame( hDC, &rect );
    rect.top = BESTY - 1; rect.bottom = rect.top + BESTH + 2; DrawSFrame( hDC, &rect );
    rect.top = MESSY - 1; rect.bottom = rect.top + LINESIZE + 2; DrawSFrame( hDC, &rect );
}


DEFINE_RESPONSE_TABLE1( TNoEraseBkGndStatic, TStatic )
EV_WM_ERASEBKGND,
END_RESPONSE_TABLE;

DEFINE_RESPONSE_TABLE1( TInfoWindow, TFrameWindow )
EV_WM_CTLCOLOR,
END_RESPONSE_TABLE;
