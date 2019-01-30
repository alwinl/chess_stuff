// ObjectWindows - (C) Copyright 1992 by Borland International

#ifndef __WCHESS_H
#define __WCHESS_H

#define IDM_BEGIN       24350
#define IDM_MOVEPIECE   24350
#define IDM_MOVETIME    24351
#define IDM_TOTALTIME   24352
#define IDM_DEMO        24353
#define IDM_INFINITE    24354
#define IDM_PLY         24355
#define IDM_MATE        24356
#define IDM_SINGLE      24357
#define IDM_TWOPLAYER   24358
#define IDM_AUTO        24359
#define IDM_REVERSE     24360
#define CM_UNDO         24361
#define CM_REDO         24362
#define CM_STOP         24363
#define IDM_EASY        24364
#define IDM_HINT        24365
#define IDM_PASS        24366
#define IDM_EXIT        24371
#define IDM_MATCHING    24372
#define IDM_WHITETURN   24373
#define IDM_BLACKTURN   24374
#define IDM_COLORS      24375
#define IDM_BESTLINE    24376
#define IDM_MOUSEDRAG   24377
#define IDM_MOUSESELECT 24378
#define IDM_PIECEVALUES 24379
#define IDM_SOUND       24380

#define EM_ERROR        24344
#define EM_CANCEL       24346
#define EM_CLEAR        24347
#define EM_DONE         24348
#define SD_ENDGAMEDLG   24349
#define SD_CHESSERROR   24345

#define IDD_WRED        24854
#define IDD_WGREEN      24855
#define IDD_WBLUE       24856
#define IDD_WEDITRED    24857
#define IDD_WEDITGREEN  24858
#define IDD_WEDITBLUE   24859
#define IDD_BRED        24860
#define IDD_BGREEN      24861
#define IDD_BBLUE       24862
#define IDD_BEDITRED    24863
#define IDD_BEDITGREEN  24864
#define IDD_BEDITBLUE   24865
#define IDD_BLACKCOLOR  24866
#define IDD_WHITECOLOR  24867

#define IDD_QUEEN       24868
#define IDD_ROOK        24869
#define IDD_BISHOP      24870
#define IDD_KNIGHT      24871
#define IDD_PAWN        24872
#define IDD_EQUEEN      24873
#define IDD_EROOK       24874
#define IDD_EBISHOP     24875
#define IDD_EKNIGHT     24876
#define IDD_EPAWN       24877
#define IDD_DEFAULT     201

#define TIMEID          202

/**
 * --------------- TChessWindow ----------------------
 */
_CLASSDEF( TChessWindow )
class TChessWindow : public TWindow
{
public:
    TChessWindow( PTWindowsObject AParent, LPSTR ATitle );
    ~TChessWindow();

    virtual void SetupWindow();
    virtual void GetWindowClass( WNDCLASS& WndClass );
    virtual bool CanClose();

    virtual void Paint( HDC PaintDC, PAINTSTRUCT& PaintInfo );

    /* chess menu */
    virtual void WMLButtonDown( TMessage& msg ) = [ WM_FIRST + WM_LBUTTONDOWN ];
    virtual void WMMouseMove( RTMessage ) = [ WM_FIRST + WM_MOUSEMOVE ];
    virtual void WMTimer( RTMessage ) = [WM_FIRST + WM_TIMER];

    virtual void CMNewGame( RTMessage Msg ) = [CM_FIRST + CM_FILENEW];
    virtual void CMRestoreGame( RTMessage Msg ) = [CM_FIRST + CM_FILEOPEN];
    virtual void CMSaveGame( RTMessage Msg ) = [CM_FIRST + CM_FILESAVE];
    virtual void CMSaveGameAs( RTMessage Msg ) = [CM_FIRST + CM_FILESAVEAS];
    virtual void Exit( RTMessage ) = [CM_FIRST + IDM_EXIT] { CloseWindow(); }
    virtual void IDMPass( RTMessage ) = [CM_FIRST + IDM_PASS];
    virtual void IDMHint( RTMessage ) = [CM_FIRST + IDM_HINT];

    virtual void UndoMove( RTMessage Msg ) = [CM_FIRST + CM_UNDO];
    virtual void RedoUndo( RTMessage Msg ) = [CM_FIRST + CM_REDO];
    virtual void IDMMovePieces( RTMessage ) = [CM_FIRST + IDM_MOVEPIECE];

    virtual void IDMEasy( RTMessage ) = [CM_FIRST + IDM_EASY];
    virtual void IDMMoveTime( RTMessage ) = [CM_FIRST + IDM_MOVETIME];
    virtual void IDMTotalTime( RTMessage ) = [CM_FIRST + IDM_TOTALTIME];
    virtual void IDMMatching( RTMessage ) = [CM_FIRST + IDM_MATCHING];
    virtual void IDMInfiniteSearch( RTMessage ) = [CM_FIRST + IDM_INFINITE];
    virtual void IDMPlySearch( RTMessage ) = [CM_FIRST + IDM_PLY];
    virtual void IDMMateSearch( RTMessage ) = [CM_FIRST + IDM_MATE];

    virtual void IDMTwoPlayer( RTMessage ) = [CM_FIRST + IDM_TWOPLAYER];
    virtual void IDMDemo( RTMessage ) = [CM_FIRST + IDM_DEMO];
    virtual void IDMPieceValues( RTMessage ) = [CM_FIRST + IDM_PIECEVALUES];
    virtual void IDMMouseDrag( RTMessage ) = [CM_FIRST + IDM_MOUSEDRAG];
    virtual void IDMMouseSelect( RTMessage ) = [CM_FIRST + IDM_MOUSESELECT];
    virtual void IDMSound( RTMessage ) = [CM_FIRST + IDM_SOUND];

    virtual void IDMColors( RTMessage ) = [CM_FIRST + IDM_COLORS];
    virtual void IDMReverseBoard( RTMessage ) = [CM_FIRST + IDM_REVERSE];
    virtual void IDMBestLine( RTMessage ) = [CM_FIRST + IDM_BESTLINE];

    /* Edit menu */
    virtual void EMDone( RTMessage ) = [CM_FIRST + EM_DONE];
    virtual void EMClear( RTMessage ) = [CM_FIRST + EM_CLEAR];
    virtual void IDMWhiteTurn( RTMessage ) = [CM_FIRST + IDM_WHITETURN];
    virtual void IDMBlackTurn( RTMessage ) = [CM_FIRST + IDM_BLACKTURN];
    virtual void EMCancel( RTMessage ) = [CM_FIRST + EM_CANCEL];

    virtual void EMError( RTMessage ) = [CM_FIRST + EM_ERROR];

private:
    enum TURNTYPE { player, computer };

    HMENU hMenu;              // handle to the main menu
    HMENU hThinkMenu;         // handle to the short (stop) menu
    HMENU hEditMenu;          // handle to the edit (arrange) menu
    HBRUSH hBKBrush;          // handle to brush to paint main window backgnd
    HBITMAP hBKBrushBmp;      // small bitmap used to paint main window bkgnd

    PTInfoWindow TInfo;       // pointer to the information window
    TEditBarWindow *TEditBar; // pointer to the edit bar

    bool NewGame;             // a new game is being set up?
    bool GotStartSquare;      // did the user select a valid piece to move?
    bool Modified;            // has the board been changed in edit mode?
    bool EditingBoard;        // is the user editing the board
    bool Dragging;            // in drag mode?

    char *FileName;           // last saved game
    RECT MainWndRect;         // coordinates for the main window
    RECT InfoAreaRect;        // coordinates for the information window
    TURNTYPE WhoseTurn;       // players or computers turn?
    int MoveStartSquare; // starting square for move
    int MoveEndSquare; // ending square for move
    ENUMCOLOR CurPlayer;      // current player
    BOARDTYPE SaveBoard[0x78];     // saved board setup in case user cancels edit

    void EndGame();
    void DoEdit();
    void SaveGameAs();
    void ComputersTurn();
    void CheckLevelMenu( ENUMLEVEL );
    void UnCheckLevelMenu( ENUMLEVEL );
    void NormalSetup();
};

/** \brief Main Application
 *
 * It's only function is to spawn a window (assigned to TApplication's MainWindow member)
 * TApplication's InitMainWindow function is a factory method to create an application
 * specific window.
 * In this case we create a TChessWindow
 */
class TChess: public TApplication
{
public:
    TChess( LPSTR AName, HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
        : TApplication( AName, hInstance, hPrevInstance, lpCmdLine, nCmdShow ) { }

    virtual void InitMainWindow();

    void InitInstance();
};


#endif // __WCHESS_H
