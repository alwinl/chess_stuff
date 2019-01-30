// ObjectWindows - (C) Copyright 1992 by Borland International

//  Owl Chess is based upon the chess program originally released
//  with TURBO PASCAL GameWorks.  The chess engine was ported over
//  to C, and is essentially unchanged here.  This demonstrates
//  how OWL may be used to place a WINDOWS wrapper around
//  DOS code, whether it is written in C or C++.


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
#include "pvalue.h"
#include "externs.h"

#undef MAXPATH
#define MAXPATH  160

/*
 *  Global Variables
 */

char WhiteSquareColors[3] = { ~0, ~0, ~0 };  /* white */
char BlackSquareColors[3] = { 0, 128, 0 };   /* green */

PTInfoWindow  TInfo;
HBRUSH        hBlackBrush;
HBRUSH        hWhiteBrush;
HWND          hWndMain;
HBITMAP       PieceBmpArray[][2];
HBITMAP       MaskArray[];
HCURSOR       hWaitCursor;
HMENU         ThinkMenu;
HMENU         MainMenu;



HCURSOR hArrowCursor;
HANDLE hAccel;

bool ShowBestLine = true;
bool Editing;
bool SoundOn;

ENUMCOLOR ComputerColor;

int BORDERSIZE;
short CHARSIZE;
short LINESIZE;
short INFOXSIZE;
short INFOYSIZE;

const BORDERYEXTRA = 4; // 4 for spacing

// ------------- TChessWindow member functions -----------------

TChessWindow::TChessWindow( PTWindowsObject AParent, LPSTR ATitle ) : TWindow( AParent, ATitle )
{
    HDC hDC = GetDC( HWindow );
    TEXTMETRIC tm;

    GetTextMetrics( hDC, &tm );
    ReleaseDC( HWindow, hDC );

    CHARSIZE = tm.tmAveCharWidth;
    LINESIZE = tm.tmHeight + tm.tmExternalLeading;
    int CAPTIONY = GetSystemMetrics( SM_CYCAPTION ) + GetSystemMetrics( SM_CYMENU );
    BORDERSIZE = LINESIZE + MYFRAMESIZE;

    Attr.Style = WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX;
    Attr.X = 5;
    Attr.Y = 5;
    Attr.H = CAPTIONY + ( BORDERSIZE * 2 ) + INFOYSIZE + BORDERYEXTRA + ( 2 * GetSystemMetrics( SM_CYBORDER ) );
    Attr.W = ( BORDERSIZE * 4 ) + ( MAXBDSIZE * SQUARE_SIZE ) + INFOXSIZE + ( 2 * GetSystemMetrics( SM_CXBORDER ) ) + ( 2*MYFRAMESIZE );

    TInfo = new TInfoWindow( this, "InfoWindow" );
    TEditBar = new TEditBarWindow( this, "EditBar" );

    FileName = new char[MAXPATH];
    WhoseTurn = player;
    CurPlayer = white;
    SoundOn = true;
    NewGame = true;
    EditingBoard =  false;
    GotStartSquare = false;

    ::ComputerColor = black;
    ::Editing =  false;
    ::TInfo = TInfo;
}

TChessWindow::~TChessWindow()
{
    KillTimer( HWindow, TIMEID );

    delete FileName;
    delete TInfo;

    DeleteObject( hWhiteBrush );
    DeleteObject( hBlackBrush );
    DeleteObject( hBKBrushBmp );

    DestroyMenu( hThinkMenu );
    DestroyMenu( hEditMenu );

    for( int i = 0; i < 6; i++ ) {
        DeleteObject( MaskArray[i] );
        DeleteObject( PieceBmpArray[i][white] );
        DeleteObject( PieceBmpArray[i][black] );
    }
}

void TChessWindow::SetupWindow()
{
    TFrameWindow::SetupWindow();

    ::hWndMain    = HWindow;
    ::hWaitCursor = LoadCursor( 0, IDC_WAIT );

    ::PieceBmpArray[ pawn-1   ][ white ] = LoadBitmap( GetApplication()->hInstance, "WPawnBmp" );
    ::PieceBmpArray[ rook-1   ][ white ] = LoadBitmap( GetApplication()->hInstance, "WRookBmp" );
    ::PieceBmpArray[ knight-1 ][ white ] = LoadBitmap( GetApplication()->hInstance, "WKnightBmp" );
    ::PieceBmpArray[ bishop-1 ][ white ] = LoadBitmap( GetApplication()->hInstance, "WBishopBmp" );
    ::PieceBmpArray[ queen-1  ][ white ] = LoadBitmap( GetApplication()->hInstance, "WQueenBmp" );
    ::PieceBmpArray[ king-1   ][ white ] = LoadBitmap( GetApplication()->hInstance, "WKingBmp" );

    ::PieceBmpArray[ pawn-1   ][ black ] = LoadBitmap( GetApplication()->hInstance, "BPawnBmp" );
    ::PieceBmpArray[ rook-1   ][ black ] = LoadBitmap( GetApplication()->hInstance, "BRookBmp" );
    ::PieceBmpArray[ knight-1 ][ black ] = LoadBitmap( GetApplication()->hInstance, "BKnightBmp" );
    ::PieceBmpArray[ bishop-1 ][ black ] = LoadBitmap( GetApplication()->hInstance, "BBishopBmp" );
    ::PieceBmpArray[ queen-1  ][ black ] = LoadBitmap( GetApplication()->hInstance, "BQueenBmp" );
    ::PieceBmpArray[ king-1   ][ black ] = LoadBitmap( GetApplication()->hInstance, "BKingBmp" );

    ::MaskArray[ pawn-1   ] = LoadBitmap( GetApplication()->hInstance, "PMaskBmp" );
    ::MaskArray[ rook-1   ] = LoadBitmap( GetApplication()->hInstance, "RMaskBmp" );
    ::MaskArray[ knight-1 ] = LoadBitmap( GetApplication()->hInstance, "KTMaskBmp" );
    ::MaskArray[ bishop-1 ] = LoadBitmap( GetApplication()->hInstance, "BMaskBmp" );
    ::MaskArray[ queen-1  ] = LoadBitmap( GetApplication()->hInstance, "QMaskBmp" );
    ::MaskArray[ king-1   ] = LoadBitmap( GetApplication()->hInstance, "KMaskBmp" );

    ::GetClientRect( HWindow, &MainWndRect );
    InfoAreaRect      = MainWndRect;
    InfoAreaRect.left = ( MainWndRect.right -= ( ( BORDERSIZE * 2 ) + INFOXSIZE ) );

    MainMenu    = hMenu       = ::GetMenu( HWindow );

    ::ThinkMenu = hThinkMenu  = LoadMenu( GetApplication()->hInstance, "TChessThinkMenu" );

    hEditMenu   = LoadMenu( GetApplication()->hInstance, "TChessEditMenu" );

    ::Talk();
    CheckLevelMenu( Level );

    ::hWhiteBrush = CreateSolidBrush( RGB( WhiteSquareColors[ 0 ], WhiteSquareColors[ 1 ], WhiteSquareColors[ 2 ] ) );
    ::hBlackBrush = CreateSolidBrush( RGB( BlackSquareColors[ 0 ], BlackSquareColors[ 1 ], BlackSquareColors[ 2 ] ) );

    Dragging = true;
}

void TChessWindow::GetWindowClass( WNDCLASS& WndClass )
{
    // Again, call the ancestor version first, as must also
    // be done with SetupWindow and InitInstance
    TWindow::GetWindowClass( WndClass );

    hArrowCursor = LoadCursor( 0, IDC_ARROW );
    hBKBrushBmp = LoadBitmap( GetApplication()->hInstance, "BKBrushBmp" );
    hBKBrush = CreatePatternBrush( hBKBrushBmp );

    WndClass.hCursor = hArrowCursor;
    WndClass.hbrBackground = hBKBrush;
    WndClass.lpszMenuName = "TChessMenu";
    WndClass.hIcon = LoadIcon( GetApplication()->hInstance, "ChessIcon" );
}

void TChessWindow::Paint( HDC PaintDC, PAINTSTRUCT& )
{
    DrawFrame( PaintDC, MainWndRect );
    DrawFrame( PaintDC, InfoAreaRect );
    PrintBoard();
}

void TChessWindow::WMLButtonDown( TMessage& )
{
    POINT Point;

    if( EditingBoard ) {
        DoEdit();
        return;
    }

    if( WhoseTurn == computer && NoComputerMove == false )
        return;

    GetCursorPos( &Point );
    ScreenToClient( HWindow, &Point );

    if( !GotStartSquare ) {

        MoveStartSquare = GetValidSquare( Point );
        if( MoveStartSquare == -1 )
            return;

        if( ! PlayerOwnsSquare( MoveStartSquare, CurPlayer )
           return;

        GotStartSquare = true;

        if( !Dragging )
            DrawInvertedBitmap( MoveStartSquare );
        else
            DragStart( MoveStartSquare, Point );

    } else {

        MoveEndSquare = GetValidSquare( Point );
        GotStartSquare = false;

        if( MoveEndSquare == -1 ) {
            if( Dragging )
                DragEnd( false );
            Warning( "Invalid Move" );
            DrawNormalBitmap( MoveStartSquare );
            return;
        }

        if( !MoveCheck( MoveStartSquare, MoveEndSquare ) ) {
            if( Dragging )
                DragEnd( false );
            DrawNormalBitmap( MoveStartSquare );
            return;
        }

        ::GotValidMove = true;
        EnableMenuItem( hMenu, CM_UNDO, MF_BYCOMMAND | MF_ENABLED );

        if( GameOver )
            EndGame();
        else if( NoComputerMove == false )
            ComputersTurn();
        else if( ::MultiMove ) {
            CurPlayer = ( CurPlayer == white ) ? black : white;
        }

    }
}

void TChessWindow::WMMouseMove( RTMessage msg )
{
    if( GotStartSquare && Dragging )
        Drag( MAKEPOINT( msg.LParam ) );
}

void TChessWindow::WMTimer( RTMessage )
{
    char buf[255];

    TInfo->SetTimerText( DisplayTime( &ChessTime[RunColor], buf, 255 ) );
}

void TChessWindow::CMNewGame()
{
    ::KillTimer( HWindow, TIMEID );
    TInfo->SetTimerText( "" );

    EnableMenuItem( hMenu, CM_UNDO, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
    EnableMenuItem( hMenu, CM_REDO, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );

    NewGame = true;

    ::NewGame();
    ::PrintBoard();

    CurPlayer = Player;
    ComputerColor = Opponent;
}

void TChessWindow::CMRestoreGame()
{
    if( GetApplication()->ExecDialog( new TFileDialog( this, SD_FILEOPEN, strcpy( FileName, "*.CHS" ) ) ) != IDOK )
        return;

    if( ::RestoreGame( FileName ) != 0 ) {
        sprintf( buf, "Cannot open %s for reading", file_name );
        ::MessageBox( hWndMain, buf, "Chess", MB_OK | MB_ICONHAND );
        return;
    }

    NewGame = false;
    CurPlayer = Player;
    ComputerColor = Opponent;

    EnableMenuItem( hMenu, CM_REDO, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
    EnableMenuItem( hMenu, CM_UNDO, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
}

void TChessWindow::CMSaveGame()
{
    if( NewGame )
        SaveGameAs();
    else {
        switch( ::SaveGame( FileName ) ) {
        case -1:
            ::MessageBox( hWndMain, "Not enough memory to perform operation", "Chess", MB_OK | MB_ICONHAND );
            break;
        case -2:
            sprintf( buf, "Cannot open %s for writing", savefile );
            ::MessageBox( hWndMain, buf, "Chess", MB_OK | MB_ICONHAND );
            break;
        default:
            break;
        }
    }
}

void TChessWindow::CMSaveGameAs()
{
    SaveGameAs();
}

void TChessWindow::Exit( RTMessage )
{
    CloseWindow();
}

void TChessWindow::IDMPass( RTMessage )
{
    if( ::Lavel == easygame )
        HideAttacks();

    CurPlayer = Opponent;
    ComputerColor = Player;

    ComputersTurn();
}

void TChessWindow::IDMHint( RTMessage )
{
    FindHintMove();
    ShowHint();
}

void TChessWindow::UndoMove( RTMessage )
{

    if( ComputerThinking ) {
        MessageToPost = CM_UNDO;
        return;
    }
    if( !Undo() )
        EnableMenuItem( hMenu, CM_UNDO, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );

    EnableMenuItem( hMenu, CM_REDO, MF_BYCOMMAND | MF_ENABLED );

    CurPlayer = Player;
    ComputerColor = Opponent;
}

void TChessWindow::RedoUndo( RTMessage )
{
    if( ComputerThinking ) {
        MessageToPost = CM_REDO;
        return;
    }
    if( !Redo() )
        EnableMenuItem( hMenu, CM_REDO, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );

    EnableMenuItem( hMenu, CM_UNDO, MF_BYCOMMAND | MF_ENABLED );

    CurPlayer = Player;
    ComputerColor = Opponent;
}

void TChessWindow::IDMMovePieces()
{
    CopyBoard( Board, SaveBoard );

    Editing      = true;
    EditingBoard = true;

    TInfo->Show( SW_HIDE );
    ::SetMenu( HWindow, hEditMenu );

    Modified = false;

    CheckMenuItem( hEditMenu, IDM_BLACKTURN, ( ( CurPlayer == white ) ? MF_UNCHECKED : MF_CHECKED ) );
    CheckMenuItem( hEditMenu, IDM_WHITETURN, ( ( CurPlayer == white ) ? MF_CHECKED   : MF_UNCHECKED ) );

    TEditBar->Show( SW_NORMAL );
}

void TChessWindow::IDMEasy( RTMessage )
{
    if( ::Level != easygame ) {
        UnCheckLevelMenu( Level );
        CheckMenuItem( hMenu, IDM_EASY, MF_CHECKED );
    }

    ::Level = easygame;
    ::AverageTime = 5.0;
    ::MaxLevel = MAXPLY;
    ::PrintCurLevel();
}

void TChessWindow::IDMMoveTime( RTMessage )
{
    char SecsPerMove[40];

    sprintf( SecsPerMove, "%.2lf", ::AverageTime );

    if( GetApplication()->ExecDialog( new TInputDialog( this, "Set Seconds per Move","Seconds Per Move:", SecsPerMove, 19 ) ) != IDOK )
        return;

    double NewMoveTime = atof( SecsPerMove );

    if( NewMoveTime <= 0. ) {
        Error( "Invalid time.  No change made." );
        return;
    }

    if( ::Level != normal ) {
        UnCheckLevelMenu( Level );
        CheckMenuItem( hMenu, IDM_MOVETIME, MF_CHECKED );
    }

    ::Level = normal;
    ::AverageTime = NewMoveTime;
    setTotalTime( &ChessTime[white], ( ::MoveNo / 2 ) * NewMoveTime );
    setTotalTime( &ChessTime[black], ( ::MoveNo / 2 ) * NewMoveTime );
    ::MaxLevel = MAXPLY;

    PrintCurLevel();
}

void TChessWindow::IDMTotalTime( RTMessage )
{
    char *TotalMoveTime = new char[40];
    sprintf( TotalMoveTime, "%.2lf", ::AverageTime );

    if( GetApplication()->ExecDialog( new TInputDialog( this, "Set Seconds per Game", "Seconds Per Game:", TotalMoveTime, 19 ) ) != IDOK ) {
        delete TotalMoveTime;
        return;
    }

    double NewMoveTime = atof( TotalMoveTime );
    if( NewMoveTime <= 0. ) {
        Error( "Invalid time.  No change made." );
        delete TotalMoveTime;
        return;
    }

    if( ::Level != fullgametime ) {
        UnCheckLevelMenu( Level );
        CheckMenuItem( hMenu, IDM_TOTALTIME, MF_CHECKED );
    }

    ::Level = fullgametime;
    ::AverageTime = NewMoveTime;
    setTotalTime( &ChessTime[white], ( ::MoveNo / 2 ) * NewMoveTime );
    setTotalTime( &ChessTime[black], ( ::MoveNo / 2 ) * NewMoveTime );
    ::MaxLevel = MAXPLY;

    ::PrintCurLevel();

    delete TotalMoveTime;
}

void TChessWindow::IDMMatching( RTMessage )
{
    if( ::Level != matching ) {
        UnCheckLevelMenu( Level );
        CheckMenuItem( hMenu, IDM_MATCHING, MF_CHECKED );
    }

    ::Level = matching;

    ::PrintCurLevel();
}

void TChessWindow::IDMInfiniteSearch( RTMessage )
{
    if( ::Level != infinite ) {
        UnCheckLevelMenu( Level );
        CheckMenuItem( hMenu, IDM_INFINITE, MF_CHECKED );
    }

    ::Level = infinite;
    ::MaxLevel = MAXPLY;

    ::PrintCurLevel();
}

void TChessWindow::IDMPlySearch( RTMessage )
{
    char *PlySearchDepth = new char[40];

    sprintf( PlySearchDepth, "%d", ::MaxLevel );

    if( GetApplication()->ExecDialog( new TInputDialog( this, "Set ply depth", "Number of plys:", PlySearchDepth, 19 ) ) != IDOK ) {
        delete PlySearchDepth;
        return;
    }

    int NewPlyDepth = atoi( PlySearchDepth );
    if( NewPlyDepth <= 0 ) {
        ::Error( "Invalid time.  No change made." );
        delete PlySearchDepth;
        return;
    }

    if( ::Level != plysearch ) {
        UnCheckLevelMenu( Level );
        CheckMenuItem( hMenu, IDM_PLY, MF_CHECKED );
    }

    ::MaxLevel = ( char )( ( NewPlyDepth > MAXPLY ) ? MAXPLY : NewPlyDepth );
    ::Level = plysearch;

    ::PrintCurLevel();

    delete PlySearchDepth;
}

void TChessWindow::IDMMateSearch( RTMessage )
{
    if( ::Level != matesearch ) {
        UnCheckLevelMenu( Level );
        CheckMenuItem( hMenu, IDM_MATE, MF_CHECKED );
    }

    ::Level = matesearch;
    ::PrintCurLevel();

    ComputersTurn();
}

void TChessWindow::IDMTwoPlayer( RTMessage )
{
    static ENUMLEVEL OldLevel = normal;

    ::MultiMove = !::MultiMove;

    if( ::MultiMove ) {
        OldLevel = ::Level;
        ModifyMenu( hMenu, IDM_TWOPLAYER, MF_BYCOMMAND | MF_STRING, IDM_TWOPLAYER, "&Single Player" );
        EnableMenuItem( hMenu, 2, MF_GRAYED | MF_BYPOSITION );
        ::Level = normal;
    } else {
        ModifyMenu( hMenu, IDM_TWOPLAYER, MF_BYCOMMAND | MF_STRING, IDM_TWOPLAYER, "&Two Player" );
        EnableMenuItem( hMenu, 2, MF_ENABLED | MF_BYPOSITION );
        ::Level = OldLevel;
    }

    DrawMenuBar( HWindow );
    ::PrintCurLevel();
}

void TChessWindow::IDMDemo( RTMessage )
{
    extern bool AutoPlay;
    bool IsEasy = false;

    if( ::Level == easygame ) {
        IsEasy = true;
        ::Level = normal;
        HideAttacks();
    }

    AutoPlay = true;
    ComputersTurn();

    if( IsEasy ) {
        ::Level = easygame;
        ::UpdateBoard();
    }

    CurPlayer = Player;
    ComputerColor = Opponent;
    ::PrintCurLevel();
}

void TChessWindow::IDMPieceValues( RTMessage )
{
    GetApplication()->ExecDialog( new TPieceValueDialog( this, "PieceValueDlg" ) );
}

void TChessWindow::IDMMouseDrag( RTMessage )
{
    POINT Point;

    if( !Dragging && GotStartSquare ) {
        DrawNormalBitmap( MoveStartSquare );
        GetCursorPos( &Point );
        DragStart( MoveStartSquare, Point );
    }

    Dragging = true;

    CheckMenuItem( hMenu, IDM_MOUSEDRAG, MF_CHECKED );
    CheckMenuItem( hMenu, IDM_MOUSESELECT, MF_UNCHECKED );
}

void TChessWindow::IDMMouseSelect( RTMessage )
{
    DragEnd( true );

    Dragging = false;

    CheckMenuItem( hMenu, IDM_MOUSEDRAG, MF_UNCHECKED );
    CheckMenuItem( hMenu, IDM_MOUSESELECT, MF_CHECKED );

    if( GotStartSquare )
        DrawInvertedBitmap( MoveStartSquare );
}

void TChessWindow::IDMSound( RTMessage )
{
    SoundOn = !SoundOn;
    CheckMenuItem( hMenu, IDM_SOUND, SoundOn ? MF_CHECKED : MF_UNCHECKED );
}

void TChessWindow::IDMColors( RTMessage )
{
    GetApplication()->ExecDialog( new TColorsDialog( this, "ColorsDialog" ) );

    DeleteObject( hWhiteBrush );
    DeleteObject( hBlackBrush );

    ::hWhiteBrush = CreateSolidBrush( RGB( WhiteSquareColors[0], WhiteSquareColors[1], WhiteSquareColors[2] ) );
    ::hBlackBrush = CreateSolidBrush( RGB( BlackSquareColors[0], BlackSquareColors[1], BlackSquareColors[2] ) );
}

void TChessWindow::IDMReverseBoard( RTMessage )
{
    ::Turned = !::Turned;
    ::PrintBoard();
}

void TChessWindow::IDMBestLine( RTMessage )
{
    ShowBestLine = !ShowBestLine;

    if( ShowBestLine )
        CheckMenuItem( hMenu, IDM_BESTLINE, MF_CHECKED );
    else {
        CheckMenuItem( hMenu, IDM_BESTLINE, MF_UNCHECKED );
        TInfo->SetBestLineText( "" );
    }
}

void TChessWindow::OnEMDone( )
{
    if( !Modified ) {
        NormalSetup();
        return;
    }

    if( ! ValidBoard() ) {
        Error( "Wrong number of pieces" );
        return;
    }

    ResetNewPos();

    if( Attacks( Player, PieceTab[ Opponent ][ 0 ].isquare ) ) {
        Error( "Illegal King position" );
        return;
    }

    NormalSetup();
    return;
}

void TChessWindow::EMClear( RTMessage /*unused*/ )
{
    int sq;

    Modified = true;

    ClearBoard();
    ::UpdateBoard();
}

void TChessWindow::IDMWhiteTurn( RTMessage )
{
    CheckMenuItem( hEditMenu, IDM_BLACKTURN, MF_UNCHECKED );
    CheckMenuItem( hEditMenu, IDM_WHITETURN, MF_CHECKED );

    CurPlayer = white;
    ComputerColor = black;

    if( CurPlayer != Player ) {
        Opponent = ComputerColor;
        Player = ProgramColor = CurPlayer;
    }
}


void TChessWindow::IDMBlackTurn( RTMessage )
{
    CheckMenuItem( hEditMenu, IDM_BLACKTURN, MF_CHECKED );
    CheckMenuItem( hEditMenu, IDM_WHITETURN, MF_UNCHECKED );

    CurPlayer = black;
    ComputerColor = white;

    if( CurPlayer != Player ) {
        Opponent = ComputerColor;
        Player = ProgramColor = CurPlayer;
    }
}

void TChessWindow::OnEMCancel()
{
    CopyBoard( SaveBoard, Board );

    ResetNewPos();
    NormalSetup();
}




void TChessWindow::EMError()
{
    ::MessageBox( hWndMain, ( LPSTR )buf, "OWL Chess Error", MB_OK | MB_ICONHAND );
}



void TChessWindow::EndGame()
{
    if( ::MessageBox( hWndMain, (LPSTR) EndGameMessage, "OWL Chess", MB_YESNO ) == IDNO ) {
        PostMessage( hWndMain, WM_COMMAND, CM_EXIT, 0L );
        return;
    }

    GameOver = false;
    PostMessage( hWndMain, WM_COMMAND, CM_FILENEW, 0L );

    return;
}

void TChessWindow::DoEdit()
{
    POINT Point;

    GetCursorPos( &Point );
    ScreenToClient( HWindow, &Point );
    int Square = GetValidSquare( Point );

    if( Square == -1 )
        return;

    /* If there is no piece on the square, put the selected piece on,
     * otherwise if there is a piece on the square, take it off
     * Little trick: no_piece is zero in the enumeration, so
     * if we need to clear the square, set the SelectedItem index to -1
     */
    int SelectedItem = TEditBar->GetSelectedItem();

    if( Board[Square].piece != no_piece )
        SelectedItem = -1;

    InsertPiece( ( ENUMPIECE )( SelectedItem % 6 + 1 ), ( SelectedItem < 6 ) ? white : black, Square );

    Modified = true;
    ::UpdateBoard();
}

void TChessWindow::SaveGameAs()
{
    if( GetApplication()->ExecDialog( new TFileDialog( this, SD_FILESAVE, strcpy( FileName, "*.CHS" ) ) ) == IDOK ) {

        NewGame = false;
        switch( ::SaveGame( FileName ) ) {
        case -1:
            ::MessageBox( hWndMain, "Not enough memory to perform operation", "Chess", MB_OK | MB_ICONHAND );
            break;
        case -2:
            sprintf( buf, "Cannot open %s for writing", savefile );
            ::MessageBox( hWndMain, buf, "Chess", MB_OK | MB_ICONHAND );
            break;
        default:
            break;
        }

    }
}

void TChessWindow::ComputersTurn()
{
    SetClassWord( HWindow, GCW_HCURSOR, WORD( ::hWaitCursor ) );
    SetCursor( ::hWaitCursor );
    SetMenu( HWindow, hThinkMenu );

    WhoseTurn = computer;

    ProgramMove();

    SetClassWord( HWindow, GCW_HCURSOR, WORD( hArrowCursor ) );
    SetMenu( HWindow, hMenu );

    WhoseTurn = player;

    if( GameOver )
        EndGame();
}

void TChessWindow::CheckLevelMenu( ENUMLEVEL level )
{
    switch( level ) {
    case normal       : CheckMenuItem( hMenu, IDM_MOVETIME, MF_CHECKED );  break;
    case fullgametime : CheckMenuItem( hMenu, IDM_TOTALTIME, MF_CHECKED ); break;
    case plysearch    : CheckMenuItem( hMenu, IDM_PLY, MF_CHECKED );       break;
    case easygame     : CheckMenuItem( hMenu, IDM_EASY, MF_CHECKED );      break;
    case infinite     : CheckMenuItem( hMenu, IDM_INFINITE, MF_CHECKED );  break;
    case matesearch   : CheckMenuItem( hMenu, IDM_MATE, MF_CHECKED );      break;
    case matching     : CheckMenuItem( hMenu, IDM_MATCHING, MF_CHECKED );  break;
    default           : CheckMenuItem( hMenu, IDM_EASY, MF_CHECKED );      break;
    }
}

void TChessWindow::UnCheckLevelMenu( ENUMLEVEL level )
{
    switch( level ) {
    case normal       : CheckMenuItem( hMenu, IDM_MOVETIME, MF_UNCHECKED );  break;
    case fullgametime : CheckMenuItem( hMenu, IDM_TOTALTIME, MF_UNCHECKED ); break;
    case plysearch    : CheckMenuItem( hMenu, IDM_PLY, MF_UNCHECKED );       break;
    case easygame     : CheckMenuItem( hMenu, IDM_EASY, MF_UNCHECKED );      break;
    case infinite     : CheckMenuItem( hMenu, IDM_INFINITE, MF_UNCHECKED );  break;
    case matesearch   : CheckMenuItem( hMenu, IDM_MATE, MF_UNCHECKED );      break;
    case matching     : CheckMenuItem( hMenu, IDM_MATCHING, MF_UNCHECKED );  break;
    default           : CheckMenuItem( hMenu, IDM_EASY, MF_UNCHECKED );      break;
    }
}

void TChessWindow::NormalSetup()
{
    TEditBar->Show( SW_HIDE );
    TInfo->Show( SW_NORMAL );

    SetMenu( HWindow, hMenu );

    Editing = false;
    EditingBoard = false;

    ::UpdateBoard();
}

bool TChessWindow::CanClose()
{
    if( ComputerThinking || AutoPlay ) {
        PostMessage( HWindow, WM_COMMAND, CM_STOP, 0L );
        PostMessage( HWindow, WM_COMMAND, IDM_EXIT, 0L );
        return false;
    }

    QuitProgram();
    return true;
}

// ------------- TChess member functions -----------------------

void TChess::InitMainWindow()
{
    MainWindow = new TChessWindow( NULL, "OWL Chess" );
}

void TChess::InitInstance()
{
    TApplication::InitInstance();       // always call the base version first
    hAccel = HAccTable = LoadAccelerators( hInstance, "ChessCommands" );
    BWCCGetVersion();  // init of BWCC required to assure load of BWCC.DLL
}

/** \brief Main entry into the program
 *
 * \param hInstance HINSTANCE
 * \param hPrevInstance HINSTANCE
 * \param lpCmdLine LPSTR
 * \param nCmdShow int
 * \return int PASCAL
 *
 */
int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    TChess ChessApp( "ChessApp", hInstance, hPrevInstance, lpCmdLine, nCmdShow );

    ChessApp.Run();

    return ChessApp.Status;
}

