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
#include "pvalue.h"
#include "externs.h"

const int DefaultValues[5] = {0x90, 0x4c, 0x30, 0x30, 0x10};

TPieceValueDialog::TPieceValueDialog( PTWindowsObject AParent, LPSTR AName ) : TDialog( AParent, AName )
{
}

void TPieceValueDialog::PVSetFocus( HWND hWnd )
{
    ::SendMessage( HWindow, WM_NEXTDLGCTL, WPARAM( hWnd ), 1L );
    ::SendMessage( hWnd, EM_SETSEL, 0, MAKELONG( 0, 32767 ) );
}

void TPieceValueDialog::PVKillFocus( RTMessage msg )
{
    if( GetActiveWindow() != HWindow ) return;

    if( GetDlgItem( HWindow, msg.WParam  ) == GetFocus() ) return;
    if( GetDlgItem( HWindow, IDCANCEL    ) == GetFocus() ) return;
    if( GetDlgItem( HWindow, IDD_DEFAULT ) == GetFocus() ) return;

    GetColorValue( Id );
}

void TPieceValueDialog::WMCommand( RTMessage msg )
{
    if( msg.LP.Lo && msg.LP.Hi == EN_KILLFOCUS ) {
        if( SendMessage( HWND( msg.LP.Lo ), EM_GETMODIFY, 0, 0L ) )
            PostMessage( HWindow, PV_KILLFOCUS, msg.WParam, 0L );

        return;
    }

    TWindowsObject::WMCommand( msg );
}



bool TPieceValueDialog::GetColorValue( WORD Id )
{
    ePIECES piece;

    for( piece = pvqueen; (piece <= pvpawn) && ScollerIDs[piece] != Id; piece++ )
        ;

    if( piece > pvpawn )        /* should not happen */
        return;

    bool Ok;
    int NewVal = ::GetDlgItemInt( HWindow, Id, &Ok, true );

    if( ! Ok )
        return false;

    if( (NewVal > 255) || (NewVal < 0) ) {
        Error( "Please enter a valid number between 0 and 255." );
        PVSetFocus( GetDlgItem( HWindow, Id ) );
        return false;
    }

    if( NewVal != Values[piece] ) {
        Values[piece] = NewVal;
        SetScrollPos( hScroller[piece], SB_CTL, Values[piece], true );
    }

    return true;
}

void TPieceValueDialog::Ok( RTMessage /*unused*/ )
{
    MSG msg;

    while( PeekMessage( &msg, HWindow, PV_KILLFOCUS, PV_KILLFOCUS, PM_NOREMOVE ) )
        continue;

    if( GetColorValue( IDD_EQUEEN ) && GetColorValue( IDD_EROOK ) &&
            GetColorValue( IDD_EBISHOP ) && GetColorValue( IDD_EKNIGHT ) && GetColorValue( IDD_EPAWN ) ) {

        Values[pvqueen]  *= 16;
        Values[pvrook]   *= 16;
        Values[pvbishop] *= 16;
        Values[pvknight] *= 16;
        Values[pvpawn]   *= 16;

        setPieceValues( Values );

        CloseWindow( IDOK );
    }
}

void TPieceValueDialog::SetupWindow()
{
    TDialog::SetupWindow();

    getPieceValues( Values );

    Values[pvqueen ] /= 16;
    Values[pvrook  ] /= 16;
    Values[pvbishop] /= 16;
    Values[pvknight] /= 16;
    Values[pvpawn  ] /= 16;

    ScollerIDs[pvqueen]  = IDD_QUEEN;
    ScollerIDs[pvrook]   = IDD_ROOK;
    ScollerIDs[pvbishop] = IDD_BISHOP;
    ScollerIDs[pvknight] = IDD_KNIGHT;
    ScollerIDs[pvpawn]   = IDD_PAWN;

    for( ePIECES piece = pvqueen; piece <= pvpawn; piece++ ) {

        hScroller[piece] = ::GetDlgItem( HWindow, ScollerIDs[piece]  );

        ::SetDlgItemInt( HWindow, ScollerIDs[piece],  Values[piece], false );
        ::SetScrollRange( hScroller[piece], SB_CTL, 0, 255, false );
        ::SetScrollPos( hScroller[piece], SB_CTL, Values[piece], false );
    }
}


void TPieceValueDialog::IDDDefault( RTMessage /*unused*/ )
{
    memcpy( Values, DefaultValues, 5 * sizeof( int ) );

    for( ePIECES piece = pvqueen; piece <= pvpawn; piece++ ) {
        ::SetDlgItemInt( HWindow, ScollerIDs[piece],  Values[piece], false );
        ::SetScrollPos( hScroller[piece], SB_CTL, Values[piece ], true );
    }
}

void TPieceValueDialog::WMHScroll( RTMessage msg )
{
    ePIECES piece;

    for( piece = pvqueen; (piece <= pvpawn) && ( HWND( msg.LP.Hi ) != hScroller[piece] ); piece++ )
        ;

    if( piece > pvpawn )
        return;

    switch( msg.WParam ) {
    case SB_PAGEDOWN      : Values[piece] += 15;       break;
    case SB_LINEDOWN      : Values[piece] += 1;        break;
    case SB_PAGEUP        : Values[piece] -= 15;       break;
    case SB_LINEUP        : Values[piece] -= 1;        break;
    case SB_TOP           : Values[piece] = 0;         break;
    case SB_BOTTOM        : Values[piece] = 255;       break;
    case SB_THUMBPOSITION : Values[piece] = msg.LP.Lo; break;
    case SB_THUMBTRACK    : Values[piece] = msg.LP.Lo; break;
    default:
        break;
    }

    Values[piece] = std::min( 255, Values[piece] );     /* range checking */
    Values[piece] = std::max( 0, Values[piece] );

    SetScrollPos( hScroller[piece], SB_CTL, Values[piece], true );
    SetDlgItemInt( HWindow, ScollerIDs[piece], Values[piece], false );
}

DEFINE_RESPONSE_TABLE1( TPieceValueDialog, TDialog )
    EV_MESSAGE( 1124, PVKillFocus ),
    EV_CHILD_NOTIFY_ALL_CODES( 1, Ok ),
    EV_WM_HSCROLL,
    EV_CHILD_NOTIFY_ALL_CODES( 201, IDDDefault ),
END_RESPONSE_TABLE;
