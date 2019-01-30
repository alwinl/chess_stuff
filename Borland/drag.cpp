/*
 * Copyright 2017 Alwin Leerling <alwin@jambo>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */

//#include <math.h>
#include "wcdefs.h"
//#include "externs.h"

typedef pair<int,int> POINT;
typedef unsigned int HDC;
typedef unsigned int HBITMAP;

const int AnimationSpeed = 2;

static HDC      hSaveDC;
static HBITMAP  hOldBmp;
static HBITMAP  hDragPiece;
static HBITMAP  hDragMask;

static bool IsDragging = false;

static POINT StartPoint;
static POINT CurPoint;
static POINT LastPoint;


static void UpdateBackground( bool is_save, POINT& point )
{
    HDC hDC = GetDC( hWndMain );

    if( is_save ) {
        hSaveDC = CreateCompatibleDC( hDC );
        hOldBmp = ( HBITMAP )SelectObject( hSaveDC, CreateCompatibleBitmap( hDC, SQUARE_SIZE, SQUARE_SIZE ) );

        BitBlt( hSaveDC, 0, 0, SQUARE_SIZE, SQUARE_SIZE, hDC, point.x, point.y, SRCCOPY );
    } else {
        BitBlt( hDC, point.x, point.y, SQUARE_SIZE, SQUARE_SIZE, hSaveDC, 0, 0, SRCCOPY );

        DeleteObject( SelectObject( hSaveDC, hOldBmp ) );
        DeleteDC( hSaveDC );
    }

    ReleaseDC( hWndMain, hDC );
}

static void ShowPiece( POINT& p )
{
    HDC hDC = GetDC( hWndMain );
    HDC hMemoryDC = CreateCompatibleDC( hDC );

    HBITMAP hOldBmp = ( HBITMAP )SelectObject( hMemoryDC, hDragMask );
    BitBlt( hDC, p.x, p.y, SQUARE_SIZE, SQUARE_SIZE, hMemoryDC, 0, 0, SRCAND );

    SelectObject( hMemoryDC, hDragPiece );
    BitBlt( hDC, p.x, p.y, SQUARE_SIZE, SQUARE_SIZE, hMemoryDC, 0, 0, SRCINVERT );

    SelectObject( hMemoryDC, hOldBmp );

    DeleteDC( hMemoryDC );
    ReleaseDC( hWndMain, hDC );
}

static void UpdateScreen( POINT& p )
{
    HDC hDC = GetDC( hWndMain );

    HDC hNewSaveDC = CreateCompatibleDC( hDC );
    HBITMAP hNewOldBmp = ( HBITMAP )SelectObject( hNewSaveDC, CreateCompatibleBitmap( hDC, SQUARE_SIZE, SQUARE_SIZE ) );

    rect.left = std::min( rect1.left, rect2.left );
    rect.top = std::min( rect1.top, rect2.top );
    rect.bottom = std::max( rect1.bottom, rect2.bottom );
    rect.right = std::max( rect1.right, rect2.right );

    int Width = rect.right - rect.left;
    int Height = rect.bottom - rect.top;

    HDC hUpdateDC = CreateCompatibleDC( hDC );
    HBITMAP hUOldBmp = ( HBITMAP )SelectObject( hUpdateDC, CreateCompatibleBitmap( hDC, Width,Height ) );

    /* get new section */
    BitBlt( hUpdateDC, 0, 0, Width, Height, hDC, rect.left, rect.top, SRCCOPY );

    /* restore old section */
    BitBlt( hUpdateDC, abs( rect.left - rect2.left ), abs( rect.top - rect2.top ), SQUARE_SIZE, SQUARE_SIZE, hSaveDC, 0, 0, SRCCOPY );

    /* save new background */
    BitBlt( hNewSaveDC, 0, 0, SQUARE_SIZE, SQUARE_SIZE, hUpdateDC, abs( rect.left - rect1.left ), abs( rect.top - rect1.top ), SRCCOPY );

    /* blit the mask */
    DeleteObject( SelectObject( hSaveDC, hDragMask ) );

    BitBlt( hUpdateDC, abs( rect.left - rect1.left ), abs( rect.top - rect1.top ), SQUARE_SIZE, SQUARE_SIZEt, hSaveDC, 0, 0, SRCAND );

    /* blit the piece */
    SelectObject( hSaveDC, hDragPiece );
    BitBlt( hUpdateDC, abs( rect.left - rect1.left ), abs( rect.top - rect1.top ), SQUARE_SIZE, SQUARE_SIZE, hSaveDC, 0, 0, SRCINVERT );

    /* put it on the screen */
    BitBlt( hDC, rect.left, rect.top, Width, Height, hUpdateDC, 0, 0, SRCCOPY );

    SelectObject( hSaveDC, hOldBmp ); // don't delete current bmp
    DeleteDC( hSaveDC );

    hSaveDC = hNewSaveDC;
    hOldBmp = hNewOldBmp;

    DeleteObject( SelectObject( hUpdateDC, hUOldBmp ) );
    DeleteDC( hUpdateDC );

    ReleaseDC( hWndMain, hDC );
}

static void UpdateCursor( bool is_set )
{
    static HCURSOR hSaveCursor = NULL;

    if( is_set ) {
        hSaveCursor = ::SetCursor( 0 );
        ::SetClassWord( hWndMain, GCW_HCURSOR, 0 );
    } else {
        ::SetCursor( hSaveCursor );
        ::SetClassWord( hWndMain, GCW_HCURSOR, WORD( hSaveCursor ) );
        hSaveCursor = NULL;
    }
}

static void InitPieceAndMaskHandles( int square )
{
    hDragPiece = PieceBmpArray[Board[square].piece-1][Board[square].color];
    hDragMask = MaskArray[Board[square].piece-1];
}


static bool Intersect( POINT& p1, POINT& p2 )
{
    RECT rect1, rect2;

    rect1.left = p1.x;
    rect1.top = p1.y;
    rect1.right = p1.x + SQUARE_SIZE;
    rect1.bottom = p1.y + SQUARE_SIZE;

    rect2.left = p2.x;
    rect2.top = p2.y;
    rect2.right = p2.x + SQUARE_SIZE;
    rect2.bottom = p2.y + SQUARE_SIZE;

    POINT p;

    /* move around the corners of rectangle 1 and test if that corner is in rect 2 */
    p.x = rect1.left;
    p.y = rect1.top;    if( PtInRect( &rect2, p ) ) return true;
    p.y = rect1.bottom; if( PtInRect( &rect2, p ) ) return true;
    p.x = rect1.right;  if( PtInRect( &rect2, p ) ) return true;
    p.y = rect1.top;    if( PtInRect( &rect2, p ) ) return true;

    return false;
}

static void FAR PASCAL ReturnHome( int X, int Y, LPSTR lpData )
{
    POINT point;
    static int Count = AnimationSpeed;

    if( Count != 0 ) {
        Count--;
        return;
    }

    Count = AnimationSpeed;
    point.x = X;
    point.y = Y;

    if( Intersect( point, LastPoint ) ) {
        UpdateScreen( point );
        LastPoint = point;
        return;
    }

    UpdateBackground( false, LastPoint );
    UpdateBackground( true, point );
    ShowPiece( point );

    LastPoint = point;
}

void DragStart( int square, POINT& point )
{
    point.x -= 18;
    point.y -= 18;

    StartPoint = GetSquareXY( square );

    InitPieceAndMaskHandles( square );

    UpdateCursor( true );

    ::SetCapture( hWndMain );

    ClearSquare( square );
    UpdateBackground( true, point );
    ShowPiece( point );

    LastPoint = point;
    IsDragging = true;
}

void Drag( POINT& point )
{
    point.x -= 18;
    point.y -= 18;
    CurPoint = point;

    if( Intersect( point, LastPoint ) ) {
        UpdateScreen( point );
        LastPoint = point;
        return;
    }

    UpdateBackground( false, LastPoint );
    UpdateBackground( true, point );
    ShowPiece( point );

    LastPoint = point;
}

void DragEnd( bool ValidMove )
{
    if( !IsDragging )
        return;

    if( ValidMove )
        UpdateBackground( false, LastPoint );
    else {
        LineDDA( CurPoint.x, CurPoint.y, StartPoint.x, StartPoint.y, LINEDDAPROC( ReturnHome ), NULL );
        UpdateBackground( false, LastPoint );
    }

    UpdateCursor( false );

    ReleaseCapture();
    IsDragging = false;
}

void SlidePiece( int end_square, int start_square )
{
    if( IsDragging ) {
        ::MessageBox( NULL, "SlidePiece called before DragEnd", "WChess", MB_OK );
        return;
    }

    InitPieceAndMaskHandles( start_square );

    UpdateCursor( true );

    POINT start_point = GetSquareXY( start_square );
    POINT end_point = GetSquareXY( end_square );

    ClearSquare( start_square );
    LastPoint = start_point;
    UpdateBackground( true, start_point );
    ShowPiece( start_point );

    LineDDA( start_point.x, start_point.y, end_point.x, end_point.y, LINEDDAPROC( ReturnHome ), NULL );

    if( Intersect( end_point, LastPoint )  ) {
        UpdateScreen( end_point );
        LastPoint = end_point;
    } else {
        UpdateBackground( false, LastPoint );
        UpdateBackground( true, end_point );
        ShowPiece( end_point );

        LastPoint = end_point;
    }

    DeleteObject( SelectObject( hSaveDC, hOldBmp ) );
    DeleteDC( hSaveDC );

    UpdateCursor( false );
}
