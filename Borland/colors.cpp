// ObjectWindows - (C) Copyright 1992 by Borland International

#include <static.h>
#include <filedial.h>
#include <inputdia.h>
#include <bwcc.h>

#include <string.h>
#include <stdio.h>

#include "wcdefs.h"
#include "info.h"
#include "wchess.h"
#include "edit.h"
#include "colors.h"
#include "externs.h"

#define min(x, y)   (((x) < (y)) ? (x) : (y))
#define max(x, y)   (((x) > (y)) ? (x) : (y))

TColorsDialog::TColorsDialog( PTWindowsObject AParent, LPSTR AName ) : TDialog( AParent, AName )
{
}

TColorsDialog::~TColorsDialog()
{
    DeleteObject( hSBBrush[Red] );
    DeleteObject( hSBBrush[Green] );
    DeleteObject( hSBBrush[Blue] );
}

void TColorsDialog::SetupWindow()
{
    TDialog::SetupWindow();

    memcpy( WSqColors, WhiteSquareColors, sizeof( WSqColors ) );
    memcpy( BSqColors, BlackSquareColors, sizeof( BSqColors ) );

    SetDlgItemInt( HWindow, IDD_WEDITRED,   WSqColors[0], false );
    SetDlgItemInt( HWindow, IDD_WEDITGREEN, WSqColors[1], false );
    SetDlgItemInt( HWindow, IDD_WEDITBLUE,  WSqColors[2], false );
    SetDlgItemInt( HWindow, IDD_BEDITRED,   BSqColors[0], false );
    SetDlgItemInt( HWindow, IDD_BEDITGREEN, BSqColors[1], false );
    SetDlgItemInt( HWindow, IDD_BEDITBLUE,  BSqColors[2], false );

    RWID = GetDlgCtrlID( GetDlgItem( HWindow, IDD_WRED ) );
    GWID = GetDlgCtrlID( GetDlgItem( HWindow, IDD_WGREEN ) );
    BWID = GetDlgCtrlID( GetDlgItem( HWindow, IDD_WBLUE ) );
    RBID = GetDlgCtrlID( GetDlgItem( HWindow, IDD_BRED ) );
    GBID = GetDlgCtrlID( GetDlgItem( HWindow, IDD_BGREEN ) );
    BBID = GetDlgCtrlID( GetDlgItem( HWindow, IDD_BBLUE ) );

    SetScrollRange( GetDlgItem( HWindow, IDD_WRED ),   SB_CTL, 0, 255, false );
    SetScrollRange( GetDlgItem( HWindow, IDD_WGREEN ), SB_CTL, 0, 255, false );
    SetScrollRange( GetDlgItem( HWindow, IDD_WBLUE ),  SB_CTL, 0, 255, false );
    SetScrollRange( GetDlgItem( HWindow, IDD_BRED ),   SB_CTL, 0, 255, false );
    SetScrollRange( GetDlgItem( HWindow, IDD_BGREEN ), SB_CTL, 0, 255, false );
    SetScrollRange( GetDlgItem( HWindow, IDD_BBLUE ),  SB_CTL, 0, 255, false );

    SetScrollPos( GetDlgItem( HWindow, IDD_WRED ),   SB_CTL, WSqColors[0], false );
    SetScrollPos( GetDlgItem( HWindow, IDD_WGREEN ), SB_CTL, WSqColors[1], false );
    SetScrollPos( GetDlgItem( HWindow, IDD_WBLUE ),  SB_CTL, WSqColors[2], false );
    SetScrollPos( GetDlgItem( HWindow, IDD_BRED ),   SB_CTL, BSqColors[0], false );
    SetScrollPos( GetDlgItem( HWindow, IDD_BGREEN ), SB_CTL, BSqColors[1], false );
    SetScrollPos( GetDlgItem( HWindow, IDD_BBLUE ),  SB_CTL, BSqColors[2], false );

    WStatic = GetDlgCtrlID( WhiteSq = GetDlgItem( HWindow, IDD_WHITECOLOR ) );
    BStatic = GetDlgCtrlID( BlackSq = GetDlgItem( HWindow, IDD_BLACKCOLOR ) );

    hSBBrush[Red]   = CreateSolidBrush( RGB( 255, 0, 0 ) );
    hSBBrush[Green] = CreateSolidBrush( RGB( 0, 255, 0 ) );
    hSBBrush[Blue]  = CreateSolidBrush( RGB( 0, 0, 255 ) );
}

void TColorsDialog::CLSetFocus( HWND hWnd )
{
    ::SendMessage( HWindow, WM_NEXTDLGCTL, WPARAM( hWnd ), 1L );
    ::SendMessage( hWnd, EM_SETSEL, 0, MAKELONG( 0, 32767 ) );
}

bool TColorsDialog::GetColorValue( WORD Id )
{
    int NewVal;
    bool Ok;
    char *CurVal;
    HWND hScroller, hButton;

    NewVal = ::GetDlgItemInt( HWindow, Id, &Ok, true );

    switch( Id ) {
    case IDD_WEDITRED:
        CurVal = &WSqColors[0];
        hScroller = ::GetDlgItem( HWindow, IDD_WRED );
        hButton = WhiteSq;
        break;
    case IDD_WEDITGREEN:
        CurVal = &WSqColors[1];
        hScroller = ::GetDlgItem( HWindow, IDD_WGREEN );
        hButton = WhiteSq;
        break;
    case IDD_WEDITBLUE:
        CurVal = &WSqColors[2];
        hScroller = ::GetDlgItem( HWindow, IDD_WBLUE );
        hButton = WhiteSq;
        break;
    case IDD_BEDITRED:
        CurVal = &BSqColors[0];
        hScroller = ::GetDlgItem( HWindow, IDD_BRED );
        hButton = BlackSq;
        break;
    case IDD_BEDITGREEN:
        CurVal = &BSqColors[1];
        hScroller = ::GetDlgItem( HWindow, IDD_BGREEN );
        hButton = BlackSq;
        break;
    case IDD_BEDITBLUE:
        CurVal = &BSqColors[2];
        hScroller = ::GetDlgItem( HWindow, IDD_BBLUE );
        hButton = BlackSq;
        break;
    }

    if( Ok )
        Ok = ( NewVal > 255 ) ? false : ( ( NewVal < 0 ) ? false : true );
    else
        NewVal = *CurVal;

    if( !Ok ) {
        Error( "Please enter a valid number between 0 and 255." );
        CLSetFocus( GetDlgItem( HWindow, Id ) );

    } else if( NewVal != *CurVal ) {
        *CurVal = NewVal;
        SetScrollPos( hScroller, SB_CTL, *CurVal, true );
        InvalidateRect( hButton, 0, false );
    }

    return Ok;
}

void TColorsDialog::Ok( RTMessage /*unused*/ )
{
    MSG msg;

    while( PeekMessage( &msg, HWindow, CL_KILLFOCUS, CL_KILLFOCUS, PM_NOREMOVE ) )
        continue;

    if( GetColorValue( IDD_WEDITRED ) && GetColorValue( IDD_WEDITGREEN ) && GetColorValue( IDD_WEDITBLUE )
       && GetColorValue( IDD_BEDITRED ) && GetColorValue( IDD_BEDITGREEN ) && GetColorValue( IDD_BEDITBLUE ) ) {

        memcpy( WhiteSquareColors, WSqColors, sizeof( WSqColors ) );
        memcpy( BlackSquareColors, BSqColors, sizeof( BSqColors ) );

        CloseWindow( IDOK );
    }
}

void TColorsDialog::WMControlColor( RTMessage msg )
{
    if( msg.LP.Hi == CTLCOLOR_SCROLLBAR ) {

        switch( GetDlgCtrlID( HWND( msg.LP.Lo ) ) ) {
        case RWID: msg.Result = long( hSBBrush[Red] ); break;
        case GWID: msg.Result = long( hSBBrush[Green] ); break;
        case BWID: msg.Result = long( hSBBrush[Blue] ); break;
        case RBID: msg.Result = long( hSBBrush[Red] ); break;
        case GBID: msg.Result = long( hSBBrush[Green] ); break;
        case BBID: msg.Result = long( hSBBrush[Blue] ); break;
        default:
            DefWndProc( msg );
        }
        return;
    }

    DefWndProc( msg );
}


void TColorsDialog::WMVScroll( RTMessage msg )
{
    char *color;
    HWND hScroller, hButton;
    int dlgitem;

    switch( GetDlgCtrlID( HWND( msg.LP.Hi ) ) ) {
    case RWID:
        color = &WSqColors[0];
        hScroller = GetDlgItem( HWindow, IDD_WRED );
        dlgitem = IDD_WEDITRED;
        hButton = WhiteSq;
        break;
    case RBID:
        color = &BSqColors[0];
        hScroller = GetDlgItem( HWindow, IDD_BRED );
        dlgitem = IDD_BEDITRED;
        hButton = BlackSq;
        break;
    case BBID:
        color = &BSqColors[2];
        hScroller = GetDlgItem( HWindow, IDD_BBLUE );
        dlgitem = IDD_BEDITBLUE;
        hButton = BlackSq;
        break;
    case BWID:
        color = &WSqColors[2];
        hScroller = GetDlgItem( HWindow, IDD_WBLUE );
        dlgitem = IDD_WEDITBLUE;
        hButton = WhiteSq;
        break;
    case GBID:
        color = &BSqColors[1];
        hScroller = GetDlgItem( HWindow, IDD_BGREEN );
        dlgitem = IDD_BEDITGREEN;
        hButton = BlackSq;
        break;
    case GWID:
        color = &WSqColors[1];
        hScroller = GetDlgItem( HWindow, IDD_WGREEN );
        dlgitem = IDD_WEDITGREEN;
        hButton = WhiteSq;
        break;
    default:
        return;
    }

    switch( msg.WParam ) {
    case SB_PAGEDOWN :
        *color += 15;
    /* fall through */
    case SB_LINEDOWN :
        *color = min( 255, *color + 1 );
        break;
    case SB_PAGEUP :
        *color -= 15;
    /* fall through */
    case SB_LINEUP :
        *color = max( 0, *color - 1 );
        break;
    case SB_TOP:
        *color = 0;
        break;
    case SB_BOTTOM :
        *color = 255;
        break;
    case SB_THUMBPOSITION :
    case SB_THUMBTRACK :
        *color = msg.LP.Lo;
        break;
    default:
        break;
    }

    SetScrollPos( hScroller, SB_CTL, *color, true );
    SetDlgItemInt( HWindow, dlgitem, *color, false );
    InvalidateRect( hButton, 0, false );
}

void TColorsDialog::WMDrawItem( RTMessage msg )
{
    LPDRAWITEMSTRUCT lpDI = ( LPDRAWITEMSTRUCT )msg.LParam;

    if( lpDI->itemAction != ODA_DRAWENTIRE )
        return;

    char *colors;

    if( lpDI->CtlID == WStatic )
        colors = WSqColors;
    else if( lpDI->CtlID == BStatic )
        colors = BSqColors;
    else
        return;

    HBRUSH hOldBrush = (HBRUSH) SelectObject( lpDI->hDC, CreateSolidBrush( RGB( colors[0], colors[1], colors[2] ) ) );

    LPRECT lpR = &( lpDI->rcItem );

    Rectangle( lpDI->hDC, lpR->left, lpR->top, lpR->right, lpR->bottom );
    DeleteObject( SelectObject( lpDI->hDC, hOldBrush ) );
}

void TColorsDialog::CLKillFocus( RTMessage msg )
{
    if( GetActiveWindow() != HWindow )
        return;

    if( GetDlgItem( HWindow, IDCANCEL ) == GetFocus() )
        return;

    WORD Id = msg.WParam;
    if( GetDlgItem( HWindow, Id ) == GetFocus() )
        return;

    GetColorValue( Id );
}

void TColorsDialog::WMCommand( RTMessage msg )
{
    if( msg.LP.Lo && ( msg.LP.Hi == EN_KILLFOCUS ) )
        if( SendMessage( HWND( msg.LP.Lo ), EM_GETMODIFY, 0, 0L ) )
            PostMessage( HWindow, CL_KILLFOCUS, ID, 0L );

    TWindowsObject::WMCommand( msg );
}

DEFINE_RESPONSE_TABLE1( TColorsDialog, TDialog )
    EV_WM_CTLCOLOR,
    EV_WM_VSCROLL,
    EV_WM_DRAWITEM,
    EV_CHILD_NOTIFY_ALL_CODES( 1, Ok ),
    EV_MESSAGE( CL_KILLFOCUS, CLKillFocus ),
END_RESPONSE_TABLE;
