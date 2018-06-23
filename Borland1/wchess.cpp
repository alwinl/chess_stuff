/*$T WCHESS.CPP GC 1.123 09/25/01 14:18:05 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


//
// OWLCVT 08/22/97 02:12:47 ObjectWindows - (C) Copyright 1992 by Borland
// International
// Owl Chess is based upon the chess program originally released with TURBO PASCAL
// GameWorks. The chess engine was ported over to C, and is essentially unchanged
// here. This demonstrates how OWL may be used to place a WINDOWS wrapper around
// DOS code, whether it is written in C or C++.
//
#include "owl/compat.h"
#include <owl/static.h>
#include <owl/filedial.h>
#include <owl/inputdia.h>
#include <bwcc.h>
#include <string.h>
#include <stdio.h>

#include "wcdefs.h"
#include "info.h"
#include "wchess.h"
#include "owl/edit.h"
#include "colors.h"
#include "pvalue.h"
#include "externs.h"

#undef MAXPATH
#define MAXPATH 160

/* Global Variables */
BYTE            WhiteSquareColors[3] = { ~0, ~0, ~0 };  /* white */
BYTE            BlackSquareColors[3] = { 0, 128, 0 };   /* green */

BOOL            ShowBestLine = TRUE;
PTInfoWindow    TInfo;
HBRUSH          hBlackBrush;
HBRUSH          hWhiteBrush;
HWND            hWndMain;
HBITMAP         PieceBmpArray[6][2];
HBITMAP         MaskArray[6];
HCURSOR         hArrowCursor;
HCURSOR         hWaitCursor;
HMENU           ThinkMenu;
HMENU           MainMenu;
HANDLE          hAccel;
COLORTYPE       ComputerColor;
short           LINESIZE;
short           CHARSIZE;
int             CAPTIONY;
BOOL            Editing;
int             BORDERSIZE;
const           BORDERYEXTRA = 4;                       // 4 for spacing
short           INFOXSIZE;
short           INFOYSIZE;
BOOL            SoundOn;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
TEndDialog::TEndDialog( PTWindowsObject AParent, int ResourceId, LPSTR Message ) :
    TDialog( AParent, ResourceId ) {
    EndMessage = Message;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEndDialog::SetupWindow( void ) {
    TDialog::SetupWindow();
    SetDlgItemText( HWindow, ID_INPUT, EndMessage );
}

//
// =======================================================================================================================
//    TChessWindow member functions
// =======================================================================================================================
//
TChessWindow::TChessWindow( PTWindowsObject AParent, LPSTR ATitle ) :
    TWindow( AParent, ATitle ) {
    HDC         hDC;
    TEXTMETRIC  tm;

    hDC = GetDC( HWindow );
    GetTextMetrics( hDC, &tm );
    ReleaseDC( HWindow, hDC );

    CHARSIZE = tm.tmAveCharWidth;
    LINESIZE = tm.tmHeight + tm.tmExternalLeading;
    CAPTIONY = GetSystemMetrics( SM_CYCAPTION ) + GetSystemMetrics( SM_CYMENU );
    BORDERSIZE = LINESIZE + MYFRAMESIZE;

    TInfo = new TInfoWindow( this, "InfoWindow" );
    TEditBar = new TEditBarWindow( this, "EditBar" );

    SoundOn = TRUE;
    ::TInfo = TInfo;

    Attr.Style = WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX;
    Attr.X = 5;
    Attr.Y = 5;
    Attr.H = CAPTIONY + ( BORDERSIZE * 2 ) + INFOYSIZE + BORDERYEXTRA + ( 2 * GetSystemMetrics(SM_CYBORDER) );
    Attr.W = ( BORDERSIZE * 4 ) + ( MAXBDSIZE * SQUARE_SIZE ) + INFOXSIZE + ( 2 * GetSystemMetrics(SM_CXBORDER) ) + ( 2 * MYFRAMESIZE );

    FileName = new char[MAXPATH];
    WhoseTurn = player;
    CurPlayer = white;
    ::ComputerColor = black;
    ::Editing = EditingBoard = GotStartSquare = FALSE;

    NewGame = TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
TChessWindow::~TChessWindow( void ) {
    delete FileName;
    delete TInfo;

    DeleteObject( hWhiteBrush );
    DeleteObject( hBlackBrush );
    DeleteObject( hBKBrushBmp );
    DestroyMenu( hThinkMenu );
    DestroyMenu( hEditMenu );

    for( int i = 0; i < 6; i++ ) {
        DeleteObject( MaskArray[i] );
        for( int j = 0; j < 2; j++ )
            DeleteObject( PieceBmpArray[i][j] );
    }

    ::KillTimer( HWindow, TIMEID );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TChessWindow::SetupWindow( void ) {
    TFrameWindow::SetupWindow();

    ::hWndMain = HWindow;
    ::hWaitCursor = LoadCursor( 0, IDC_WAIT );

    ::PieceBmpArray[pawn - 1][white] = LoadBitmap( GetApplication()->hInstance, "WPawnBmp" );
    ::PieceBmpArray[rook - 1][white] = LoadBitmap( GetApplication()->hInstance, "WRookBmp" );
    ::PieceBmpArray[knight - 1][white] = LoadBitmap( GetApplication()->hInstance, "WKnightBmp" );
    ::PieceBmpArray[bishop - 1][white] = LoadBitmap( GetApplication()->hInstance, "WBishopBmp" );
    ::PieceBmpArray[queen - 1][white] = LoadBitmap( GetApplication()->hInstance, "WQueenBmp" );
    ::PieceBmpArray[king - 1][white] = LoadBitmap( GetApplication()->hInstance, "WKingBmp" );

    ::PieceBmpArray[pawn - 1][black] = LoadBitmap( GetApplication()->hInstance, "BPawnBmp" );
    ::PieceBmpArray[rook - 1][black] = LoadBitmap( GetApplication()->hInstance, "BRookBmp" );
    ::PieceBmpArray[knight - 1][black] = LoadBitmap( GetApplication()->hInstance, "BKnightBmp" );
    ::PieceBmpArray[bishop - 1][black] = LoadBitmap( GetApplication()->hInstance, "BBishopBmp" );
    ::PieceBmpArray[queen - 1][black] = LoadBitmap( GetApplication()->hInstance, "BQueenBmp" );
    ::PieceBmpArray[king - 1][black] = LoadBitmap( GetApplication()->hInstance, "BKingBmp" );

    ::MaskArray[pawn - 1] = LoadBitmap( GetApplication()->hInstance, "PMaskBmp" );
    ::MaskArray[rook - 1] = LoadBitmap( GetApplication()->hInstance, "RMaskBmp" );
    ::MaskArray[knight - 1] = LoadBitmap( GetApplication()->hInstance, "KTMaskBmp" );
    ::MaskArray[bishop - 1] = LoadBitmap( GetApplication()->hInstance, "BMaskBmp" );
    ::MaskArray[queen - 1] = LoadBitmap( GetApplication()->hInstance, "QMaskBmp" );
    ::MaskArray[king - 1] = LoadBitmap( GetApplication()->hInstance, "KMaskBmp" );

    ::GetClientRect( HWindow, &MainWndRect );
    InfoAreaRect = MainWndRect;
    InfoAreaRect.left = ( MainWndRect.right -= ((BORDERSIZE * 2) + INFOXSIZE) );

    MainMenu = hMenu = ::GetMenu( HWindow );

    ThinkMenu = hThinkMenu = LoadMenu( GetApplication()->hInstance, "TChessThinkMenu" );

    hEditMenu = LoadMenu( GetApplication()->hInstance, "TChessEditMenu" );

    ::Talk();
    CheckLevelMenu( Level );

    ::hWhiteBrush = CreateSolidBrush( RGB(WhiteSquareColors[0], WhiteSquareColors[1], WhiteSquareColors[2]) );
    ::hBlackBrush = CreateSolidBrush( RGB(BlackSquareColors[0], BlackSquareColors[1], BlackSquareColors[2]) );

    Dragging = TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TChessWindow::EMError( void ) {
    GetApplication()->ExecDialog( new TMessageDialog(this, SD_CHESSERROR, (LPSTR) buf, "OWL Chess Error") );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TChessWindow::OnEMDone( void ) {
    SQUARETYPE  sq;
    int         KingCount[2] = { 0, 0 };
    int         TotalCount[2] = { 0, 0 };

    if( !Modified ) {
        NormalSetup();
        return;
    }

    for( sq = 0; sq < 0x78; sq++ ) {
        if( VALIDSQUARE(sq) && (Board[sq].piece != empty) ) {
            TotalCount[Board[sq].color]++;

            if( Board[sq].piece == king )
                KingCount[Board[sq].color]++;
        }
    }

    if( (TotalCount[white] > 16) || (KingCount[white] != 1) || (TotalCount[black] > 16) || (KingCount[black] != 1) ) {
        Error( "Wrong number of pieces" );
        return;
    }

    ResetNewPos();

    if( Attacks(Player, PieceTab[Opponent][0].isquare) ) {
        Error( "Illegal King position" );
        return;
    }

    NormalSetup();
    return;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TChessWindow::OnEMCancel( void ) {
    memcpy( Board, SaveBoard, sizeof(BOARDTYPE) * 0x78 );
    ResetNewPos();
    NormalSetup();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TChessWindow::OnOptionsSinglePlayer( void ) {
    if( ::Level != matching ) {
        UnCheckLevelMenu( Level );
        CheckMenuItem( hMenu, IDM_MATCHING, MF_CHECKED );
    }

    ::Level = matching;
    ::PrintCurLevel();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TChessWindow::OnOptionsTwoPlayer( void ) {
    if( ::Level != matesearch ) {
        UnCheckLevelMenu( Level );
        CheckMenuItem( hMenu, IDM_MATE, MF_CHECKED );
    }

    ::Level = matesearch;
    ::PrintCurLevel();
    ComputersTurn();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TChessWindow::IDMSinglePlayer( void ) {
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TChessWindow::IDMTwoPlayer( void ) {
    static LEVELTYPE    OldLevel = normal;
    ::MultiMove = !::MultiMove;

    if( ::MultiMove ) {
        OldLevel = ::Level;
        ModifyMenu( hMenu, IDM_TWOPLAYER, MF_BYCOMMAND | MF_STRING, IDM_TWOPLAYER, "&Single Player" );
        EnableMenuItem( hMenu, 2, MF_GRAYED | MF_BYPOSITION );
        ::DrawMenuBar( HWindow );
        ::Level = normal;
        ::PrintCurLevel();
    }
    else {
        ModifyMenu( hMenu, IDM_TWOPLAYER, MF_BYCOMMAND | MF_STRING, IDM_TWOPLAYER, "&Two Player" );
        EnableMenuItem( hMenu, 2, MF_ENABLED | MF_BYPOSITION );
        ::DrawMenuBar( HWindow );
        ::Level = OldLevel;
        ::PrintCurLevel();
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TChessWindow::OnDisplayReverseBoard( void ) {
    ::Turned = !::Turned;
    ::PrintBoard();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TChessWindow::OnEditUndo( void ) {
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

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TChessWindow::OnEditRedo( void ) {
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

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TChessWindow::EndGame( void ) {
    if( GetApplication()->ExecDialog(new TEndDialog(this, SD_ENDGAMEDLG, (LPSTR) EndGameMessage)) != IDYES ) {
        ::PostMessage( hWndMain, WM_COMMAND, CM_EXIT, 0L );
    }
    else {
        GameOver = FALSE;
        ::PostMessage( hWndMain, WM_COMMAND, CM_FILENEW, 0L );
    }

    return;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TChessWindow::OnGameHint( void ) {
    FindHintMove();
    ShowHint();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TChessWindow::OnGamePass( void ) {
    if( ::Level == easygame )
        HideAttacks();

    CurPlayer = Opponent;
    ComputerColor = Player;

    ComputersTurn();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TChessWindow::OnEMTurnWhite( void ) {
    CheckMenuItem( hEditMenu, IDM_BLACKTURN, MF_UNCHECKED );
    CheckMenuItem( hEditMenu, IDM_WHITETURN, MF_CHECKED );
    CurPlayer = white;
    ComputerColor = black;
    if( CurPlayer != Player ) {
        Opponent = ComputerColor;
        Player = ProgramColor = CurPlayer;
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TChessWindow::OnEMTurnBlack( void ) {
    CheckMenuItem( hEditMenu, IDM_BLACKTURN, MF_CHECKED );
    CheckMenuItem( hEditMenu, IDM_WHITETURN, MF_UNCHECKED );
    CurPlayer = black;
    ComputerColor = white;
    if( CurPlayer != Player ) {
        Opponent = ComputerColor;
        Player = ProgramColor = CurPlayer;
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TChessWindow::OnDisplayColors( void ) {
    GetApplication()->ExecDialog( new TColorsDialog(this, "ColorsDialog") );
    DeleteObject( hWhiteBrush );
    DeleteObject( hBlackBrush );
    ::hWhiteBrush = CreateSolidBrush( RGB(WhiteSquareColors[0], WhiteSquareColors[1], WhiteSquareColors[2]) );
    ::hBlackBrush = CreateSolidBrush( RGB(BlackSquareColors[0], BlackSquareColors[1], BlackSquareColors[2]) );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TChessWindow::OnOptionsPieceValues( void ) {
    GetApplication()->ExecDialog( new TPieceValueDialog(this, "PieceValueDlg") );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TChessWindow::OnDisplayBestLine( void ) {
    ShowBestLine = !ShowBestLine;

    if( ShowBestLine )
        CheckMenuItem( hMenu, IDM_BESTLINE, MF_CHECKED );
    else {
        CheckMenuItem( hMenu, IDM_BESTLINE, MF_UNCHECKED );
        TInfo->SetBestLineText( "" );
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TChessWindow::EvMouseMove( UINT, TPoint & ) {
    if( GotStartSquare && Dragging ) {
        TMessage    msg = __GetTMessage();
        Drag( MAKEPOINT(msg.LParam) );
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TChessWindow::OnOptionsMovementMouseDrag( void ) {
    POINT   Point;

    if( !Dragging && GotStartSquare ) {
        DrawNormalBitmap( MoveStartSquare );
        GetCursorPos( &Point );
        DragStart( MoveStartSquare, Point );
    }

    Dragging = TRUE;
    CheckMenuItem( hMenu, IDM_MOUSEDRAG, MF_CHECKED );
    CheckMenuItem( hMenu, IDM_MOUSESELECT, MF_UNCHECKED );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TChessWindow::OnOptionsMovementMouseSelect( void ) {
    DragEnd( TRUE );
    Dragging = FALSE;
    CheckMenuItem( hMenu, IDM_MOUSEDRAG, MF_UNCHECKED );
    CheckMenuItem( hMenu, IDM_MOUSESELECT, MF_CHECKED );
    if( GotStartSquare )
        DrawInvertedBitmap( MoveStartSquare );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TChessWindow::OnOptionsSound( void ) {
    SoundOn = !SoundOn;
    CheckMenuItem( hMenu, IDM_SOUND, (SoundOn ? MF_CHECKED : MF_UNCHECKED) );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TChessWindow::EvTimer( UINT ) {
    DisplayTime();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow ) {
    TChess  ChessApp( "ChessApp", hInstance, hPrevInstance, lpCmdLine, nCmdShow );
    ChessApp.Run();
    return( ChessApp.Status );
}

DEFINE_RESPONSE_TABLE1( TEndDialog, TDialog )
EV_CHILD_NOTIFY_ALL_CODES( 6, Yes ),
EV_CHILD_NOTIFY_ALL_CODES( 7, No ),
END_RESPONSE_TABLE;

DEFINE_RESPONSE_TABLE1( TChessWindow, TFrameWindow )
EV_WM_LBUTTONDOWN, EV_WM_MOUSEMOVE, EV_WM_TIMER,

/* ChessMenu */

/* "&New", */
EV_COMMAND( CM_FILENEW, OnGameNew ),

/* "&Restore...", */
EV_COMMAND( CM_FILEOPEN, OnGameRestore ),

/* "&Save", */
EV_COMMAND( CM_FILESAVE, OnGameSave ),

/* "Save &as...", */
EV_COMMAND( CM_FILESAVEAS, OnGameSaveAs ),

/* "E&xit", */
EV_COMMAND( IDM_EXIT, OnGameExit ),

/* "&Play", */
EV_COMMAND( IDM_PASS, OnGamePass ),

/* "&Hint", */
EV_COMMAND( IDM_HINT, OnGameHint ),

/* "&Undo\aALT+BkSp", */
EV_COMMAND( CM_UNDO, OnEditUndo ),

/* "&Redo\aCTRL+BkSp", */
EV_COMMAND( CM_REDO, OnEditRedo ),

/* "&Arrange", */
EV_COMMAND( IDM_MOVEPIECE, OnEditMovePieces ),

/* "&Easy", */
EV_COMMAND( IDM_EASY, OnLevelEasy ),

/* "&Time per move...", */
EV_COMMAND( IDM_MOVETIME, OnLevelMoveTime ),

/* "T&otal Time...", */
EV_COMMAND( IDM_TOTALTIME, OnLevelTotalTime ),

/* "&Matching", */
EV_COMMAND( IDM_MATCHING, OnLevelMatching ),

/* "&Infinite", */
EV_COMMAND( IDM_INFINITE, OnLevelInfiniteSearch ),

/* "&PlySearch...", */
EV_COMMAND( IDM_PLY, OnLevelPlaySearch ),

/* "M&ateSearch", */
EV_COMMAND( IDM_MATE, OnLevelMateSearch ),

/* "&Two player", */
EV_COMMAND( IDM_TWOPLAYER, OnOptionsTwoPlayer ),
EV_COMMAND( IDM_SINGLE, OnOptionsSinglePlayer ),

/* "&Demo mode", */
EV_COMMAND( IDM_DEMO, OnOptionsDemo ),

/* "&Piece Values...", */
EV_COMMAND( IDM_PIECEVALUES, OnOptionsPieceValues ),

/* "&Drag", */
EV_COMMAND( IDM_MOUSEDRAG, OnOptionsMovementMouseDrag ),

/* "&Select", */
EV_COMMAND( IDM_MOUSESELECT, OnOptionsMovementMouseSelect ),

/* "&Sound", */
EV_COMMAND( IDM_SOUND, OnOptionsSound ),

/* "&Colors...", */
EV_COMMAND( IDM_COLORS, OnDisplayColors ),

/* "&Reverse board", */
EV_COMMAND( IDM_REVERSE, OnDisplayReverseBoard ),

/* "&Show BestLine", */
EV_COMMAND( IDM_BESTLINE, OnDisplayBestLine ),

/* TChessEditMenu */

/* "&Done", */
EV_COMMAND( EM_DONE, OnEMDone ),

/* "&Clear", */
EV_COMMAND( EM_CLEAR, OnEMClear ),

/* "&White", */
EV_COMMAND( IDM_WHITETURN, OnEMTurnWhite ),

/* "&Black", */
EV_COMMAND( IDM_BLACKTURN, OnEMTurnBlack ),

/* "C&ancel", */
EV_COMMAND( EM_CANCEL, OnEMCancel ),
EV_COMMAND( EM_ERROR, EMError ),
END_RESPONSE_TABLE;

