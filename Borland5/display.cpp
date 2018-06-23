// OWLCVT 08/22/97 02:11:13
// ObjectWindows - (C) Copyright 1992 by Borland International

#include <owl\owlpch.h>
#pragma hdrstop

#include <stdio.h>
#include <string.h>
#include <bwcc.h>

#include "wcdefs.h"
#include "externs.h"


/*
 *  Global variables
 */

BOARDIDTYPE Display[0x78];
char *PieceLetter = " KQRBNP";
char buf[280];   //  general string buffer, used in several modules

/*
 *  static global variables
 */

static RECT BoardRect;


/*
 *  get handle to bitmap of current piece
 */

HBITMAP GetBitmapHandle( PIECETYPE piece, COLORTYPE pcolor )
{
   if( piece == 0 )
      return 0;

   return PieceBmpArray[piece - 1][pcolor];
}


/*
 *  Clear all information from Info window
 */

void ClearInfoWindow()
{
	TInfo->Reset();
}

/*
 *  Prints current color to play
 */
void ColorToPlay( COLORTYPE color )
{
	TInfo->SetTurnText( (color == white) ? "White" : "Black" );
}

void Message( char *str )
{
   TInfo->SetMessageText(str);
}

void Error( char *str )
{
   if( SoundOn )
      MessageBeep(0);

   strcpy( buf, str );
   ::SendMessage( hWndMain, WM_COMMAND, EM_ERROR, 0L );
}

void Warning( char *str )
{
   if( SoundOn )
      MessageBeep(0);
   Message( str );
}


/*
 *  convert a move to a string
 */

char *MoveStr( MOVETYPE *move )
{
    static char str[7];

    strcpy( str, "      " );

    if( move->movpiece == no_piece )
        return str;

    if( move->spe && move->movpiece == king ) {  /*  castling  */
        if( move->new1 > move->old )
            strcpy( str, "O-O   " );
        else
            strcpy( str, "O-O-O " );

        return (str);
    }

    str[0] = PieceLetter[move->movpiece];
    str[1] = 'a' + move->old % 16;
    str[2] = '1' + move->old / 16;
    str[3] = ( move->content == no_piece ) ? '-' : 'x';
    str[4] = 'a' + move->new1 % 16;
    str[5] = '1' + move->new1 / 16;

    return str;
}


void PrintMove( int moveno, COLORTYPE programcolor, MOVETYPE *move, double time )
{
    char info_string[80];

    int minutes = (int)(time / 60.0);

    sprintf( info_string, "%2.2d:%#04.1f %3.3d. %s", minutes, time - minutes * 60.0, moveno / 2 + 1, MoveStr(move) );

    if( programcolor == white )
        TInfo->SetWhiteInfoText( info_string );
    else
        TInfo->SetBlackInfoText( info_string );
}


/*
 *  draw 3d type frame
 */

void DrawFrame( HDC hDC, RECT& frame_area, BOOL DrawBackground )
{
    int left   = frame_area.left;
    int right  = frame_area.right;
    int top    = frame_area.top;
    int bottom = frame_area.bottom;

    HBRUSH hOldBrush = (HBRUSH)SelectObject( hDC, GetStockObject( DrawBackground ? LTGRAY_BRUSH: NULL_BRUSH ) );
    {
        HPEN hOldPen = (HPEN)SelectObject( hDC, GetStockObject( WHITE_PEN ) );
        {
            POINT polyline[3];

            Rectangle( hDC, left, top, right, bottom );

            SelectObject( hDC, CreatePen( PS_SOLID, 1, RGB(192, 192, 192)) );
            Rectangle( hDC, left+1, top+1, right-1, bottom-1 );

            DeleteObject( SelectObject( hDC, GetStockObject(WHITE_PEN)) );

            polyline[0].x = left + 2;  polyline[0].y = bottom - 3;
            polyline[1].x = right - 3; polyline[1].y = bottom - 3;
            polyline[2].x = right - 3; polyline[2].y = top + 2;

            Polyline( hDC, polyline, 3 );


            SelectObject( hDC, CreatePen(PS_SOLID, 1, RGB(128, 128, 128)) );

            polyline[0].x = left;    polyline[0].y = bottom-1;
            polyline[1].x = right-1; polyline[1].y = bottom-1;
            polyline[2].x = right-1; polyline[2].y = top;
            Polyline( hDC, polyline, 3 );

            polyline[0].x = left + 2;  polyline[0].y = bottom - 3;
            polyline[1].x = left + 2;  polyline[1].y = top + 2;
            polyline[2].x = right - 3; polyline[2].y = top + 2;
            Polyline( hDC, polyline, 3 );
        }

        DeleteObject( SelectObject(hDC, hOldPen) );
    }

    SelectObject( hDC, hOldBrush );
}


/*
 *  Display the current level indicator
 */

void PrintCurLevel()
{
    extern BOOL MultiMove;

    if( MultiMove ) {
        TInfo->SetLevelText( "Two Player" );
        return;
    }

    char info_string[80];

    switch (Level) {
    case normal:       sprintf( info_string, "%1.0f sec / move", AverageTime ); break;
    case fullgametime: sprintf( info_string, "%2.2f min / game", AverageTime ); break;
    case easygame:     strcpy( info_string, "Easy" ); break;
    case infinite :    strcpy( info_string, "Infinite" ); break;
    case plysearch :   sprintf( info_string, "Ply-Depth = %d", MaxLevel ); break;
    case matesearch:   strcpy( info_string, "MateSearch" ); break;
    case matching :    strcpy( info_string, "Match users time" ); break;
    }

    TInfo->SetLevelText(info_string);
}

POINT GetSquareXY( SQUARETYPE square )
{
   POINT p;

   if( Turned )
      square ^= 0x77;

   p.x = (square % 8) * SQUARE_SIZE + BORDERSIZE + MYFRAMESIZE;
   p.y = (7 - square / 16) * SQUARE_SIZE + BORDERSIZE + MYFRAMESIZE;

   return p;
}

void ClearSquare( SQUARETYPE square )
{
   HANDLE hOldBrush;
   POINT p;
   HDC hDC;

   p = GetSquareXY(square);

   hDC = GetDC(hWndMain);
   if( (square % 8 + square /16) % 2 == 1 )
      hOldBrush = SelectObject(hDC, hWhiteBrush);
   else
      hOldBrush = SelectObject(hDC, hBlackBrush);

   PatBlt( hDC, p.x, p.y, SQUARE_SIZE, SQUARE_SIZE, PATCOPY );
   SelectObject( hDC, hOldBrush );
   ReleaseDC( hWndMain, hDC );
}

void ClearDisplay()
{
   SQUARETYPE sq;
   ClearInfoWindow();
   for (sq = 0; sq <= 0x77; sq++)
      Display[sq].piece = no_piece;
}


/*
 *  Draw the board on the screen
 */

void DrawBoard()
{
   unsigned char no;
   HDC hDC;
   const SQUARETYPE printno[64] = {
        0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70,
        0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x67,
        0x57, 0x47, 0x37, 0x27, 0x17, 0x07, 0x06, 0x05,
        0x04, 0x03, 0x02, 0x01, 0x11, 0x21, 0x31, 0x41,
        0x51, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x56,
        0x46, 0x36, 0x26, 0x16, 0x15, 0x14, 0x13, 0x12,
        0x22, 0x32, 0x42, 0x52, 0x53, 0x54, 0x55, 0x45,
        0x35, 0x25, 0x24, 0x23, 0x33, 0x43, 0x44, 0x34
    };


   BoardRect.left = BoardRect.top = BORDERSIZE;
   BoardRect.right = BoardRect.bottom = BORDERSIZE + (2 * MYFRAMESIZE) + (8 * SQUARE_SIZE);

   hDC = GetDC( hWndMain );
   DrawFrame( hDC, BoardRect );
   ReleaseDC( hWndMain, hDC );

   for( no = 0; no < 64; no++ )
      ClearSquare( printno[no] );
}


void PrintPiece( SQUARETYPE square, PIECETYPE piece, COLORTYPE color, DWORD Rop )
{
   HBITMAP hBitmap, hOldBmp, hMaskBmp;
   HDC hMemoryDC, hDC;
   POINT p;

   if( piece == no_piece )
      return;

   HDC hDC = GetDC( hWndMain );
   HDC hMemoryDC = CreateCompatibleDC( hDC );

   HBITMAP hBitmap = GetBitmapHandle( piece, color );
   HBITMAP hMaskBmp = MaskArray[piece-1];
   HBITMAP hOldBmp = (HBITMAP)SelectObject( hMemoryDC, hMaskBmp );
   POINT p = GetSquareXY( square );

   BITMAP Bitmap;
   GetObject( hBitmap, sizeof(BITMAP), (LPSTR) &Bitmap );

   BitBlt( hDC, p.x, p.y, Bitmap.bmWidth, Bitmap.bmHeight, hMemoryDC, 0, 0, SRCAND );
   SelectObject( hMemoryDC, hBitmap );
   BitBlt( hDC, p.x, p.y, Bitmap.bmWidth, Bitmap.bmHeight, hMemoryDC, 0, 0, Rop );
   SelectObject( hMemoryDC, hOldBmp );

   DeleteDC( hMemoryDC );
   ReleaseDC( hWndMain, hDC );
}

void InitDisplay()
{
    SQUARETYPE square;

    for( square = 0; square <= 0x77; square++ )
        if( !(square & 0x88) )
            if( (Board[square].piece != Display[square].piece ) || (Board[square].piece != no_piece) && (Board[square].color != Display[square].color) ) {
                Display[square].piece = Board[square].piece;
                Display[square].color = Board[square].color;
            }
}


/*
 *   Draw a box in the square of an attacked piece.
 *   Square is black if Defended is TRUE, else it is red.
 */

static void FrameSquare(SQUARETYPE square, BOOL Defended)
{
   POINT p;
   HPEN   hOldPen;
   HBRUSH hOldBrush;
   HDC hDC;
   hDC = GetDC(hWndMain);

   p = GetSquareXY(square);
   hOldBrush = (HBRUSH)SelectObject(hDC, GetStockObject(NULL_BRUSH));
   if (Defended)
      hOldPen = (HPEN)SelectObject(hDC, GetStockObject(BLACK_PEN));
   else
      hOldPen = (HPEN)SelectObject(hDC, CreatePen(PS_SOLID,1,RGB(192, 0, 0)));


   Rectangle(hDC, p.x+1, p.y+1, p.x+SQUARE_SIZE-1, p.y+SQUARE_SIZE-1);
   SelectObject(hDC, hOldBrush);
   if (Defended)
      SelectObject(hDC, hOldPen);
   else
      DeleteObject(SelectObject(hDC, hOldPen));
   ReleaseDC(hWndMain, hDC);
}

void HideAttacks(void)
{
   SQUARETYPE square;

   for (square = 0; square <= 0x77; square++)
      if (!(square & 0x88))
         {
            if (Board[square].attacked == TRUE)
            {
            Board[square].attacked = FALSE;
            ClearSquare(square);
            PrintPiece(square, Board[square].piece,
               Board[square].color, SRCINVERT);
            }
         }
}

void ShowAttacks()
{
   SQUARETYPE square;

   for (square = 0; square <= 0x77; square++)
      if (!(square & 0x88))
         {
         if (Attacks(ComputerColor, square) && Board[square].color != ComputerColor && Board[square].piece != no_piece)
            {
            Board[square].attacked = TRUE;
            if (Attacks((COLORTYPE)!ComputerColor, square))
               FrameSquare(square, TRUE);
            else
               FrameSquare(square, FALSE);
            }
         else if (Board[square].attacked == TRUE)
            {
            Board[square].attacked = FALSE;
            ClearSquare(square);
            PrintPiece(square, Board[square].piece,
               Board[square].color, SRCINVERT);
            }
         }
}

void UpdateBoard()
{
   SQUARETYPE square;

   for (square = 0; square <= 0x77; square++)
      if (!(square & 0x88))
         if ((Board[square].piece != Display[square].piece) ||
            (Board[square].piece != no_piece) && (Board[square].color !=
            Display[square].color))
            {
               if (Display[square].piece != no_piece)
                  ClearSquare(square);
               Display[square].piece = Board[square].piece;
               Display[square].color = Board[square].color;
               if (Board[square].piece != no_piece)
                  PrintPiece(square, Board[square].piece,
                     Board[square].color, SRCINVERT);
            }
   if (Level == easygame && !Editing)
      ShowAttacks();
}

static void DrawAlphaNum();

void PrintBoard()
{
   SQUARETYPE square;

   DrawBoard();
   for (square = 0; square <= 0x77; square++)
      if (!(square & 0x88))
            {
               if (Display[square].piece != no_piece)
                  PrintPiece(square, Display[square].piece,
                     Display[square].color, SRCINVERT);
            }
   DrawAlphaNum();
   if (Level == easygame && !Editing)
      ShowAttacks();
}


/*
 *  find a square with a given point and determine whether it
 *  contains a player of the given color
 */
SQUARETYPE GetValidSquare( POINT p, COLORTYPE player, BOOL CheckPiece )
{
    POINT point;
    SQUARETYPE square;
    RECT sqrect;


    for( square = 0; square <= 0x77; square++ ) {
        if( !(square & 0x88) ) {
            point = GetSquareXY( square );

            sqrect.left = point.x;
            sqrect.top = point.y;
            sqrect.right = sqrect.left + SQUARE_SIZE;
            sqrect.bottom = sqrect.top + SQUARE_SIZE;

            if( PtInRect( &sqrect, p ) ) {
                if( !CheckPiece || (Display[square].color == player && Display[square].piece != no_piece) )
                    return square;
            }
        }
    }
    return -1;
}

void DrawNormalBitmap( SQUARETYPE square )
{
    ClearSquare(square);
    PrintPiece( square, Display[square].piece, Display[square].color, SRCINVERT );
}

void DrawInvertedBitmap( SQUARETYPE square )
{
    PrintPiece( square, Display[square].piece, Display[square].color, NOTSRCERASE );
}

void OpeningLibMsg()
{
    TInfo->SetMessageText("Using opening library");
}

void PrintNodes( NODEVAL *nodes, double time )
{
    double nodereal;
    char info_string[80];

    nodereal = (nodes->nodebase * MAXINT) + nodes->nodeoffset;

    if( time ) {
        sprintf( info_string, "%7.1f", nodereal/time );
        TInfo->SetSecondsText( info_string );
    }

    sprintf( info_string, "%7.0f ", nodereal );
    TInfo->SetNodeText( info_string );
}


/*
 *  Print bestline on screen
 */
void PrintBestMove( MOVETYPE *mainline, MAXTYPE mainvalue )
{
    char info_string[280];

    if( ! ShowBestLine )
        return;

    DEPTHTYPE dep = 0;

    *info_string = 0;

    while( dep < 7 && (mainline[dep].movpiece != no_piece) ) {
        strcat( info_string, MoveStr(&mainline[dep++]) );
        strcat( info_string, " " );
    }

    TInfo->SetBestLineText( info_string );

    sprintf( info_string, "%7.2f", mainvalue /256.0);
    TInfo->SetValueText( info_string );
}

void ClearBestLine()
{
   TInfo->SetBestLineText("");
}

void ClearMessage()
{
   TInfo->SetMessageText("");
}

static char * CharArray[] = { "a","b", "c", "d", "e", "f", "g", "h" };
static char * NumArray[] = { "1", "2", "3", "4", "5", "6", "7", "8" };

/*
 * Creates a rectangle and two lines. The two lines are placed
 * on the bottom and the right of the rectangle, offset by one pixel.
 */
static void DrawBump( HDC hDC, int x, int y )
{
   RECT rc;

   rc.top    = y - 1;
   rc.left   = x - 2;
   rc.bottom = y + LINESIZE + 1;
   rc.right  = x + CHARSIZE + 2;

   HBRUSH hOldBrush = (HBRUSH)SelectObject( hDC, GetStockObject(LTGRAY_BRUSH) );
   HPEN hOldPen = (HPEN)SelectObject( hDC, GetStockObject(WHITE_PEN) );

   Rectangle( hDC, rc.left, rc.top, rc.right, rc.bottom );

   SelectObject( hDC, CreatePen( PS_SOLID, 1, RGB(128, 128, 128)) );

   POINT vertices[3];
   vertices[0].x = rc.left + 1;  vertices[0].y = rc.bottom;
   vertices[1].x = rc.right + 1; vertices[1].y = rc.bottom;
   vertices[2].x = rc.right + 1; vertices[2].y = rc.top + 1;
   Polyline( hDC, vertices, 3 );

   DeleteObject( SelectObject( hDC, hOldPen) );
   SelectObject( hDC, hOldBrush );
}

static void DrawAlphaNum()
{
    int i;
    int XPos, YPos;
    HDC hDC;

    hDC = GetDC( hWndMain );
    {
        XPos = (BORDERSIZE + MYFRAMESIZE)/2 - CHARSIZE/2;
        YPos = (BORDERSIZE + SQUARE_SIZE/2) - LINESIZE/2;

        SetBkColor( hDC, RGB(192, 192, 192) );

        for( i = 7; i >= 0; i-- ) {
            DrawBump( hDC, XPos, YPos );
            TextOut( hDC, XPos, YPos, NumArray[ (Turned ? 7-i : i) ], 1 );
            YPos += SQUARE_SIZE;
        }

        XPos = BORDERSIZE + SQUARE_SIZE/2 - CHARSIZE/2;
        YPos = BORDERSIZE + (8 * SQUARE_SIZE) + (2 * MYFRAMESIZE) + 1;

        for( i = 0; i < 8; i++ ) {
            DrawBump( hDC, XPos, YPos );
            TextOut(hDC, XPos, YPos, CharArray[ (Turned ? 7-i : i) ], 1);
            XPos += SQUARE_SIZE;
        }
    }
    ReleaseDC(hWndMain, hDC);
}

