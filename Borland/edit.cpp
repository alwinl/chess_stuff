// ObjectWindows - (C) Copyright 1992 by Borland International

#include <windows.h>
#include <owl.h>
#include "wcdefs.h"
#include "edit.h"
#include "externs.h"


TEditBarWindow::TEditBarWindow( PTWindowsObject AParent, LPSTR ATitle ) : TWindow( AParent, ATitle )
{
    Attr.Style = WS_CHILD;
    Attr.X = ( 3 * BORDERSIZE ) + ( MAXBDSIZE * SQUARE_SIZE ) + ( 2 * MYFRAMESIZE );
    Attr.Y = BORDERSIZE;
    Attr.W = ( SQUARE_SIZE * 2 ) + 6;
    Attr.H = ( SQUARE_SIZE * 6 ) + 6;

    EditBarRect = RECT( 0, 0, ( SQUARE_SIZE * 2 ) + 6, ( SQUARE_SIZE * 6 ) + 6 );

    SelectedItem = 0;
}

void TEditBarWindow::GetWindowClass( WNDCLASS& WndClass )
{
    TWindow::GetWindowClass( WndClass );
    WndClass.hbrBackground = ( HBRUSH ) GetStockObject( LTGRAY_BRUSH );
}

/*
 *  Paint all pieces in two columns, one white, one black
 */
void TEditBarWindow::Paint( HDC hDC, PAINTSTRUCT _FAR & /*unused*/ )
{
    DrawFrame( hDC, EditBarRect, true );

    HDC hMemoryDC = CreateCompatibleDC( hDC );
    HBITMAP hOldBmp = ( HBITMAP ) SelectObject( hMemoryDC, MaskArray[0] );

    for( int i = 0; i < 6; i++ ) {

        SelectObject( hMemoryDC, MaskArray[i] );
        BitBlt( hDC, 3,               3 + ( SQUARE_SIZE * i ), SQUARE_SIZE, SQUARE_SIZE, hMemoryDC, 0, 0, SRCAND );
        BitBlt( hDC, 3 + SQUARE_SIZE, 3 + ( SQUARE_SIZE * i ), SQUARE_SIZE, SQUARE_SIZE, hMemoryDC, 0, 0, SRCAND );

        SelectObject( hMemoryDC, PieceBmpArray[i][white] );
        BitBlt( hDC, 3,               3 + ( SQUARE_SIZE * i ), SQUARE_SIZE, SQUARE_SIZE, hMemoryDC, 0, 0, ( SelectedItem == i ) ? NOTSRCERASE : SRCINVERT );

        SelectObject( hMemoryDC, PieceBmpArray[i][black] );
        BitBlt( hDC, 3 + SQUARE_SIZE, 3 + ( SQUARE_SIZE * i ), SQUARE_SIZE, SQUARE_SIZE, hMemoryDC, 0, 0, ( SelectedItem == ( i + 6 ) ) ? NOTSRCERASE : SRCINVERT );

    }

    SelectObject( hMemoryDC, hOldBmp );

    DeleteDC( hMemoryDC );
}


void TEditBarWindow::WMLButtonUp( RTMessage /*unused*/ )
{
    POINT Point;

    GetCursorPos( &Point );
    ScreenToClient( HWindow, &Point );

    int PreviousItem = SelectedItem;

    for( int i = 0; i < 6; i++ ) {

        RECT Rect;

        Rect.top    = 3 + ( i * SQUARE_SIZE );
        Rect.bottom = 3 + ( ( i + 1 ) * SQUARE_SIZE );

        // white piece?
        Rect.left = 3;
        Rect.right = 3 + SQUARE_SIZE;

        if( PtInRect( &Rect, Point ) )
            SelectedItem = i;

        // black piece?
        Rect.left  += SQUARE_SIZE;
        Rect.right += SQUARE_SIZE;

        if( PtInRect( &Rect, Point ) )
            SelectedItem = i + 6;
    }

    if( SelectedItem != PreviousItem )
        InvalidateRect( NULL );
}
