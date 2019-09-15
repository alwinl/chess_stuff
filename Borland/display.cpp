// ObjectWindows - (C) Copyright 1992 by Borland International

#include <windows.h>

#include <stdio.h>
#include <string.h>

#include <static.h>
#include <filedial.h>
#include <inputdia.h>
#include <bwcc.h>

#include "wcdefs.h"
#include "externs.h"

#include "wchess.h"


/*
 *  Global variables
 */

static BOARDTYPE Display[0x78];
char buf[280];   //  general string buffer, used in several modules
char error_message_buffer[280];


/*
 * Creates a rectangle and two lines. The two lines are placed
 * on the bottom and the right of the rectangle, offset by one pixel.
 */
static void DrawBump( HDC hDC, int x, int y )
{
    RECT rc = { y - 1, x - 2, y + LINESIZE + 1, x + CHARSIZE + 2 };

    HBRUSH hOldBrush = (HBRUSH) SelectObject( hDC, GetStockObject( LTGRAY_BRUSH ) );
    HPEN hOldPen = (HPEN) SelectObject( hDC, GetStockObject( WHITE_PEN ) );

    Rectangle( hDC, rc.left, rc.top, rc.right, rc.bottom );

    SelectObject( hDC, CreatePen( PS_SOLID, 1, RGB( 128, 128, 128 ) ) );

    POINT vertices[3] = {
        { rc.left  + 1, rc.bottom },
        { rc.right + 1, rc.bottom },
        { rc.right + 1, rc.top + 1 }
    };

    Polyline( hDC, vertices, 3 );

    DeleteObject( SelectObject( hDC, hOldPen ) );
    SelectObject( hDC, hOldBrush );
}

static void DrawAlphaNum()
{
    static char * CharArray[] = { "a","b", "c", "d", "e", "f", "g", "h" };
    static char * NumArray[] = { "1", "2", "3", "4", "5", "6", "7", "8" };

    HDC hDC = GetDC( hWndMain );

    int XPos = ( BORDERSIZE + MYFRAMESIZE )/2 - CHARSIZE/2;
    int YPos = ( BORDERSIZE + SQUARE_SIZE/2 ) - LINESIZE/2;

    SetBkColor( hDC, RGB( 192, 192, 192 ) );

    for( int i = 7; i >= 0; i-- ) {
        DrawBump( hDC, XPos, YPos );
        TextOut( hDC, XPos, YPos, NumArray[( Turned ? 7-i : i ) ], 1 );
        YPos += SQUARE_SIZE;
    }

    XPos = BORDERSIZE + SQUARE_SIZE/2 - CHARSIZE/2;
    YPos = BORDERSIZE + ( 8 * SQUARE_SIZE ) + ( 2 * MYFRAMESIZE ) + 1;

    for( int i = 0; i < 8; i++ ) {
        DrawBump( hDC, XPos, YPos );
        TextOut( hDC, XPos, YPos, CharArray[( Turned ? 7-i : i ) ], 1 );
        XPos += SQUARE_SIZE;
    }

    ReleaseDC( hWndMain, hDC );
}

/*
 *   Draw a box in the square of an attacked piece.
 *   Square is black if Defended is true, else it is red.
 */
static void FrameSquare( int square, bool Defended )
{
    HDC hDC = GetDC( hWndMain );

    HBRUSH hOldBrush = ( HBRUSH )SelectObject( hDC, GetStockObject( NULL_BRUSH ) );
    HPEN hOldPen = ( HPEN )SelectObject( hDC, CreatePen( PS_SOLID,1,RGB( Defended ? 0 : 192, 0, 0 ) ) );

    POINT p = GetSquareXY( square );
    Rectangle( hDC, p.x+1, p.y+1, p.x+SQUARE_SIZE-1, p.y+SQUARE_SIZE-1 );

    SelectObject( hDC, hOldBrush );
    DeleteObject( SelectObject( hDC, hOldPen ) );

    ReleaseDC( hWndMain, hDC );
}

/*
 *  draw 3d type frame
 */
void DrawFrame( HDC hDC, RECT& frame_area, bool DrawBackground )
{
    int left   = frame_area.left;
    int right  = frame_area.right;
    int top    = frame_area.top;
    int bottom = frame_area.bottom;

    HBRUSH hOldBrush = ( HBRUSH )SelectObject( hDC, GetStockObject( DrawBackground ? LTGRAY_BRUSH: NULL_BRUSH ) );
    HPEN hOldPen = ( HPEN )SelectObject( hDC, GetStockObject( WHITE_PEN ) );

    Rectangle( hDC, left, top, right, bottom );

    SelectObject( hDC, CreatePen( PS_SOLID, 1, RGB( 192, 192, 192 ) ) );
    Rectangle( hDC, left+1, top+1, right-1, bottom-1 );

    POINT polyline1[3] = {
        { left  + 2,  bottom - 3 },
        { right - 3,  bottom - 3 },
        { right - 3,  top    + 2 }
    };

    DeleteObject( SelectObject( hDC, GetStockObject( WHITE_PEN ) ) );
    Polyline( hDC, polyline1, 3 );

    POINT polyline2[3] = {
        { left     ,  bottom - 1 },
        { right - 1,  bottom - 1 },
        { right - 1,  top        }
    };

    SelectObject( hDC, CreatePen( PS_SOLID, 1, RGB( 128, 128, 128 ) ) );
    Polyline( hDC, polyline2, 3 );

    POINT polyline3[3] = {
        { left  + 2,  bottom - 3 },
        { left  + 2,  top    + 2 },
        { right - 3,  top    + 2 }
    };

    Polyline( hDC, polyline3, 3 );

    DeleteObject( SelectObject( hDC, hOldPen ) );
    SelectObject( hDC, hOldBrush );
}

void ClearSquare( int square )
{
    HDC hDC = GetDC( hWndMain );

    HANDLE  hOldBrush = SelectObject( hDC, (( (square & 0x0F) + (square >> 4) ) % 2) ? hWhiteBrush : hBlackBrush );

    POINT p = GetSquareXY( square );

    PatBlt( hDC, p.x, p.y, SQUARE_SIZE, SQUARE_SIZE, PATCOPY );

    SelectObject( hDC, hOldBrush );

    ReleaseDC( hWndMain, hDC );
}

/*
 *  Draw the board on the screen
 */
static void DrawBoard()
{
    RECT BoardRect;

    BoardRect.left = BoardRect.top = BORDERSIZE;
    BoardRect.right = BoardRect.bottom = BORDERSIZE + ( 2 * MYFRAMESIZE ) + ( 8 * SQUARE_SIZE );

    HDC hDC = GetDC( hWndMain );

    DrawFrame( hDC, BoardRect );

    ReleaseDC( hWndMain, hDC );

    for( int square = 0; square < 0x78; square++ )
        if( VALIDSQUARE(square) )
            ClearSquare( square );
}

static void PrintPiece( int square, ENUMPIECE piece, ENUMCOLOR color, DWORD Rop )
{
	if( piece == no_piece )
		return;

    HDC hDC = GetDC( hWndMain );
    HDC hMemoryDC = CreateCompatibleDC( hDC );

    POINT p = GetSquareXY( square );

    HBITMAP hOldBmp = ( HBITMAP )SelectObject( hMemoryDC, MaskArray[piece-1] );
    BitBlt( hDC, p.x, p.y, SQUARE_SIZE, SQUARE_SIZE, hMemoryDC, 0, 0, SRCAND );

    SelectObject( hMemoryDC, PieceBmpArray[piece - 1][color] );
    BitBlt( hDC, p.x, p.y, SQUARE_SIZE, SQUARE_SIZE, hMemoryDC, 0, 0, Rop );

    SelectObject( hMemoryDC, hOldBmp );     /* restore */

    DeleteDC( hMemoryDC );
    ReleaseDC( hWndMain, hDC );
}

void Error( char *str )
{
    if( SoundOn )
        MessageBeep( 0 );

    strcpy( error_message_buffer, str );
    ::SendMessage( hWndMain, WM_COMMAND, EM_ERROR, 0L );
}


void set_wait_cursor()
{
	::SetClassWord( hWndMain, GCW_HCURSOR, WORD( hWaitCursor ) );
	::SetCursor( hWaitCursor );
}

void set_arrow_cursor()
{
    ::SetClassWord( hWndMain, GCW_HCURSOR, WORD( hArrowCursor ) );
    ::SetCursor( hArrowCursor );
}

//void set_save_cursor()
void start_timer()
{
	::SetTimer( hWndMain, TIMEID, 1000, NULL );
}

void stop_timer()
{
	::KillTimer( hWndMain, TIMEID );
}


static void MessageScan()
{
    MSG msg;
    extern HCURSOR hWaitCursor;
    extern HMENU ThinkMenu;
    extern HANDLE hAccel;

    if( !PeekMessage( &msg, hWndMain, 0, 0, PM_REMOVE ) )
        return;

    if( TranslateAccelerator( hWndMain, HACCEL( hAccel ), &msg ) ) {
        ::PostMessage( hWndMain, WM_COMMAND, MessageToPost, 0L );
        MessageToPost = 0;
        SkipSearch = false;
        return;
    }

    if( Analysis ) {
        switch( msg.message ) {
        case WM_SETCURSOR :
            DispatchMessage( &msg );
            break;

        case WM_COMMAND :
            switch( msg.wParam ) {
            case CM_STOP :
                SkipSearch = true;
                AutoPlay = false;
                break;
            }
            break;

        default:
            TranslateMessage( &msg );
            DispatchMessage( &msg );
            break;
        }

    } else {
        switch( msg.message ) {
        case WM_LBUTTONDOWN :
            getprogramstate();
            NoComputerMove = true;
            GotValidMove = false;
            DispatchMessage( &msg );
            NoComputerMove = false;

            if( Opan && !MultiMove && GotValidMove ) {
                if( EqMove( &KeyMove, &MovTab[Depth + 1] ) ) {
                    SkipSearch = false;
                    GotValidMove = false;
                    EnterKeyMove();

                    set_wait_cursor();

                    StartAnalysis();

                    TInfo_PrintBestMove( &MainLine[0], MainEvalu );
                    main_window->set_think_menu();
                    set_wait_cursor();
                } else
                    SkipSearch = true;
            }
            getsearchstate();
            break;

        default:
            if( ( msg.hwnd != hWndMain || msg.message != WM_COMMAND ) ) {
                TranslateMessage( &msg );
                DispatchMessage( &msg );
            } else {
                SkipSearch = true;
                if( msg.message != WM_PAINT )
                    ::PostMessage( msg.hwnd, msg.message, msg.wParam, msg.lParam );
            }
            break;
        }
    }
}



/*
 *  Clear all information from Info window
 */
void TInfo_ClearWindow()
{
    TInfo->Reset();
}

void TInfo_SetTurnText( ENUMCOLOR color )
{
    TInfo->SetTurnText( ( color == white ) ? "White" : "Black" );
}

void TInfo_SetMessageText( char *str )
{
    TInfo->SetMessageText( str );
}

void TInfo_PrintMove( int moveno, ENUMCOLOR acolour, MOVESTRUCT *amove, double time )
{
    char info_string[80];

    int minutes = ( int )( time / 60.0 );

    sprintf( info_string, "%2.2d:%#04.1f %3.3d. %s", minutes, time - minutes * 60.0, moveno / 2 + 1, MoveStr( amove ) );

    if( acolour == white )
        TInfo->SetWhiteInfoText( info_string );
    else
        TInfo->SetBlackInfoText( info_string );
}

/*
 *  Display the current level indicator
 */
void TInfo_PrintCurLevel()
{
    if( MultiMove ) {
        TInfo->SetLevelText( "Two Player" );
        return;
    }

    char info_string[80];

    switch( Level ) {
    case normal       : sprintf( info_string, "%1.0f sec / move", AverageTime ); break;
    case fullgametime : sprintf( info_string, "%2.2f min / game", AverageTime ); break;
    case easygame     : strcpy( info_string, "Easy" );                           break;
    case infinite     : strcpy( info_string, "Infinite" );                       break;
    case plysearch    : sprintf( info_string, "Ply-Depth = %d", MaxLevel );      break;
    case matesearch   : strcpy( info_string, "MateSearch" );                     break;
    case matching     : strcpy( info_string, "Match users time" );               break;
    }

    TInfo->SetLevelText( info_string );
}

void TInfo_PrintNodes( double nodes, double time )
{
    char info_string[80];

    if( time ) {
        sprintf( info_string, "%7.1f", nodes/time );
        TInfo->SetSecondsText( info_string );
    }

    sprintf( info_string, "%7.0f ", nodes );
    TInfo->SetNodeText( info_string );
}

/*
 *  Print bestline on screen
 */
void TInfo_PrintBestMove( MOVESTRUCT *mainline, int mainvalue )
{
    char info_string[280];

    if( ! ShowBestLine )
        return;

    short dep = 0;

    *info_string = 0;

    while( dep < 7 && ( mainline[dep].movpiece != no_piece ) ) {
        strcat( info_string, MoveStr( &mainline[dep++] ) );
        strcat( info_string, " " );
    }

    TInfo->SetBestLineText( info_string );

    sprintf( info_string, "%7.2f", mainvalue / 256.0 );
    TInfo->SetValueText( info_string );
}

void TInfo_PrintDepthMessage( int max_depth, MOVESTRUCT * amove )
{
	char depth_message[40];

	sprintf( depth_message, "%-7d%7s", max_depth, MoveStr( amove ) );
	TInfo->SetDepthText( depth_message );
}

void TInfo_ClearBestLine()
{
    TInfo->SetBestLineText( "" );
}






void Warning( char *str )
{
    if( SoundOn )
        MessageBeep( 0 );

    TInfo_SetMessageText( str );
}

/*
 *  convert a move to a string
 */
char *MoveStr( MOVESTRUCT *amove )
{
    static char str[7];
    static char *PieceLetter = " KQRBNP";

    strcpy( str, "      " );

    if( amove->movpiece == no_piece )
        return str;

    if( amove->spe && amove->movpiece == king ) {  /*  castling  */
        if( amove->new1 > amove->old )
            strcpy( str, "O-O   " );
        else
            strcpy( str, "O-O-O " );

        return str;
    }

    str[0] = PieceLetter[amove->movpiece];
    str[1] = 'a' + amove->old % 16;
    str[2] = '1' + amove->old / 16;
    str[3] = ( amove->content == no_piece ) ? '-' : 'x';
    str[4] = 'a' + amove->new1 % 16;
    str[5] = '1' + amove->new1 / 16;

    return str;
}

POINT GetSquareXY( int square )
{
    POINT p;

    if( Turned )
        square ^= 0x77;

    p.x = ( square % 8 ) * SQUARE_SIZE + BORDERSIZE + MYFRAMESIZE;
    p.y = ( 7 - square / 16 ) * SQUARE_SIZE + BORDERSIZE + MYFRAMESIZE;

    return p;
}

void ClearDisplay()
{
    TInfo_ClearWindow();

    for( int square = 0; square <= 0x77; square++ )
        Display[square].piece = no_piece;
}

void InitDisplay()
{
    int square;

    for( square = 0; square <= 0x77; square++ )
        if( VALIDSQUARE( square ) )
            if( ( Board[square].piece != Display[square].piece ) || ( Board[square].piece != no_piece ) && ( Board[square].color != Display[square].color ) ) {
                Display[square].piece = Board[square].piece;
                Display[square].color = Board[square].color;
            }
}

void HideAttacks( void )
{
    int square;

    for( square = 0; square <= 0x77; square++ )
        if( VALIDSQUARE( square ) ) {
            if( Board[square].attacked == true ) {
                Board[square].attacked = false;
                ClearSquare( square );
                if( Board[square].piece != no_piece )
                    PrintPiece( square, Board[square].piece, Board[square].color, SRCINVERT );
            }
        }
}

void ShowAttacks()
{
    int square;

    for( square = 0; square <= 0x77; square++ )
        if( VALIDSQUARE( square ) ) {
            if( Attacks( ComputerColor, square ) && Board[square].color != ComputerColor && Board[square].piece != no_piece ) {

                Board[square].attacked = true;

                if( Attacks( ( ENUMCOLOR )!ComputerColor, square ) )
                    FrameSquare( square, true );
                else
                    FrameSquare( square, false );

            } else if( Board[square].attacked ) {

                Board[square].attacked = false;
                ClearSquare( square );
                if( Board[square].piece != no_piece )
                    PrintPiece( square, Board[square].piece, Board[square].color, SRCINVERT );
            }
        }
}

void UpdateBoard()
{
    for( int square = 0; square <= 0x77; square++ )
        if( VALIDSQUARE( square ) )
            if( ( Board[square].piece != Display[square].piece ) || ( Board[square].piece != no_piece ) && ( Board[square].color != Display[square].color ) ) {

                if( Display[square].piece != no_piece )
                    ClearSquare( square );

                Display[square].piece = Board[square].piece;
                Display[square].color = Board[square].color;

                if( Display[square].piece != no_piece )
                    PrintPiece( square, Display[square].piece, Display[square].color, SRCINVERT );
            }

    if( Level == easygame && !Editing )
        ShowAttacks();
}

void PrintBoard()
{
    DrawBoard();

    for( int square = 0; square <= 0x77; square++ )
        if( VALIDSQUARE( square ) && ( Display[square].piece != no_piece ) )
            PrintPiece( square, Display[square].piece, Display[square].color, SRCINVERT );

    DrawAlphaNum();

    if( Level == easygame && !Editing )
        ShowAttacks();
}

/*
 *  find a square with a given point
 */
int GetValidSquare( POINT p )
{
    for( int square = 0; square <= 0x77; square++ ) {
        if( VALIDSQUARE( square ) ) {
            POINT point = GetSquareXY( square );

            RECT sqrect;
            sqrect.left = point.x;
            sqrect.top = point.y;
            sqrect.right = sqrect.left + SQUARE_SIZE;
            sqrect.bottom = sqrect.top + SQUARE_SIZE;

            if( PtInRect( &sqrect, p ) ) {
                return square;
            }
        }
    }
    return -1;
}

bool PlayerOwnsSquare( int square, ENUMCOLOR color )
{
    return ( ( Display[square].color == color && Display[square].piece != no_piece ) );
}

void DrawNormalBitmap( int square )
{
    ClearSquare( square );
    if( Display[square].piece != no_piece )
        PrintPiece( square, Display[square].piece, Display[square].color, SRCINVERT );
}

void DrawInvertedBitmap( int square )
{
    if( Display[square].piece != no_piece )
        PrintPiece( square, Display[square].piece, Display[square].color, NOTSRCERASE );
}

void OpeningLibMsg()
{
    TInfo_SetMessageText( "Using opening library" );
}

