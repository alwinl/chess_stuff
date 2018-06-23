// ObjectWindows - (C) Copyright 1992 by Borland International

#include <windows.h>
#include <owl.h>
#include "wcdefs.h"
#include "edit.h"
#include "externs.h"


TEditBarWindow::TEditBarWindow( PTWindowsObject AParent, LPSTR ATitle ) : TWindow(AParent, ATitle)
{
    Attr.Style = WS_CHILD;
    Attr.X = (3 * BORDERSIZE) + (MAXBDSIZE * SQUARE_SIZE) + (2 * MYFRAMESIZE);
    Attr.Y = BORDERSIZE;

    EditBarRect.right = Attr.W = (SQUARE_SIZE*2) + 6;
    EditBarRect.bottom = Attr.H = (SQUARE_SIZE * 6) + 6;
    EditBarRect.left = EditBarRect.top = 0;
    SelectedItem = 0;
}


void TEditBarWindow::Paint( HDC hDC, PAINTSTRUCT _FAR & /*unused*/ )
{
    HDC hMemoryDC( CreateCompatibleDC( hDC ) );

    DrawFrame( hDC, EditBarRect, TRUE );

    BITMAP Bitmap;
    GetObject( GetBitmapHandle( pawn, white ), sizeof(BITMAP), (LPSTR)&Bitmap );

    for( int item = 0; item < 12; item++ ) {
        PIECETYPE piece = (PIECETYPE)((item%6)+1);
        COLORTYPE color = (item/6) ? black : white;

        HBITMAP hBitmap = GetBitmapHandle( piece, color );
        HBITMAP hMaskBmp = MaskArray[item%6];
        RECT rc;

        rc.top    = ((item%6) * SQUARE_SIZE) + 3;
        rc.left   = ((item/6) * SQUARE_SIZE) + 3;

        HBITMAP hOldBmp = (HBITMAP)SelectObject( hMemoryDC, hMaskBmp );
        BitBlt( hDC, rc.left, rc.top, Bitmap.bmWidth, Bitmap.bmHeight, hMemoryDC, 0, 0, SRCAND );
        SelectObject( hMemoryDC, hBitmap );
        BitBlt( hDC, rc.left, rc.top, Bitmap.bmWidth, Bitmap.bmHeight, hMemoryDC, 0, 0, (SelectedItem == item) ? NOTSRCERASE : SRCINVERT );
        SelectObject( hMemoryDC, hOldBmp );
    }

    DeleteDC( hMemoryDC );
}


void TEditBarWindow::WMLButtonUp( RTMessage /*unused*/ )
{
    POINT Point;
    int i, j, temp;
    RECT Rect;
    BITMAP Bitmap;
    HBITMAP hBitmap, hOldBmp, hMaskBmp;
    HDC hDC = GetDC(HWindow);
    HDC hMemoryDC = CreateCompatibleDC(hDC);

    int PreviousItem = SelectedItem;

    GetCursorPos( &Point );
    ScreenToClient( HWindow, &Point );

    /* two rows six columns */
    for( int item = 0, SelectedItem = -1; (item < 12) && (SelectedItem == -1); item++ ) {

        Rect.top    = ((item%6) * SQUARE_SIZE) + 3;
        Rect.left   = ((item/6) * SQUARE_SIZE) + 3;
        Rect.bottom = Rect.top  + SQUARE_SIZE;
        Rect.right  = Rect.left + SQUARE_SIZE;

        if( PtInRect( &Rect, Point ) )
            SelectedItem = item;
    }

    if( (SelectedItem == PreviousItem) || (SelectedItem == -1) )
        return;


    /* Erase the old bitmap */
    PIECETYPE piece = (PIECETYPE)((PreviousItem % 6) + 1);
    COLORTYPE color = (PreviousItem/6) ? black : white;

    hMaskBmp = MaskArray[PreviousItem % 6];
    Rect.top    = ((PreviousItem%6) * SQUARE_SIZE) + 3;
    Rect.left   = ((PreviousItem/6) * SQUARE_SIZE) + 3;

    // Get metrics into bitmap structure
    GetObject( GetBitmapHandle( piece, color ), sizeof(BITMAP), (LPSTR)&Bitmap );

    hOldBmp = (HBITMAP)SelectObject( hMemoryDC, hMaskBmp );
    BitBlt( hDC, Rect.left, Rect.top, Bitmap.bmWidth, Bitmap.bmHeight, hMemoryDC, 0, 0, SRCERASE );
    SelectObject( hMemoryDC, GetBitmapHandle( piece, color ) );
    BitBlt( hDC, Rect.left, Rect.top, Bitmap.bmWidth, Bitmap.bmHeight, hMemoryDC, 0, 0, SRCINVERT );
    SelectObject( hMemoryDC, hOldBmp );

    /* Paint the new bitmap */
    piece = (PIECETYPE)((SelectedItem % 6) + 1);
    color = (SelectedItem/6) ? black : white;
    hBitmap = ;
    hMaskBmp = MaskArray[SelectedItem % 6];
    Rect.top    = ((SelectedItem%6) * SQUARE_SIZE) + 3;
    Rect.left   = ((SelectedItem/6) * SQUARE_SIZE) + 3;

    hOldBmp = (HBITMAP)SelectObject(hMemoryDC, hMaskBmp);
    BitBlt( hDC, Rect.left, Rect.top, Bitmap.bmWidth, Bitmap.bmHeight, hMemoryDC, 0, 0, SRCAND );
    SelectObject( hMemoryDC, GetBitmapHandle( piece, color ) );
    BitBlt( hDC, Rect.left, Rect.top, Bitmap.bmWidth, Bitmap.bmHeight, hMemoryDC, 0, 0, NOTSRCERASE );
    SelectObject( hMemoryDC, hOldBmp );

    DeleteDC( hMemoryDC );
    ReleaseDC( HWindow, hDC );
}
