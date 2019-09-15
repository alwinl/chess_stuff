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

#define CL_KILLFOCUS    (WM_USER + 101)
#define PV_KILLFOCUS     (WM_USER + 100)

#define TIMEID          202


_CLASSDEF( TNoEraseBkGndStatic )
_CLASSDEF( TColorsDialog )
_CLASSDEF( TPieceValueDialog )
_CLASSDEF( TEditBarWindow )
_CLASSDEF( TInfoWindow )
_CLASSDEF( TChessWindow )

class TNoEraseBkGndStatic : public TStatic
{
public:
    TNoEraseBkGndStatic( PTWindows AParent, int AnId, LPSTR ATitle, int X,
                         int Y, int W, int H, WORD ATextLen, PTModule AModule = NULL );

    void SetText( LPSTR text );

private:
    virtual void WMEraseBkGnd( RTMessage msg ) = [ WM_FIRST + WM_ERASEBKGND ];

    bool    DoErase;
    int     LastLen;
};

class TColorsDialog : public TDialog
{
public:
    TColorsDialog( PTWindowsObject AParent, LPSTR AName );
    ~TColorsDialog();

    virtual void SetupWindow();
    virtual void WMControlColor( RTMessage ) = [WM_FIRST + WM_CTLCOLOR];
    virtual void WMVScroll( RTMessage );
    virtual void WMCommand( RTMessage );
    virtual void WMDrawItem( RTMessage ) = [WM_FIRST + WM_DRAWITEM];
    virtual void Ok( RTMessage );
    virtual void CLKillFocus( RTMessage ) = [WM_FIRST + CL_KILLFOCUS];
    virtual void CLSetFocus( HWND hWnd );

private:
    enum ScrollBarColors { Red, Green, Blue };

    HBRUSH hWStatic;
    HBRUSH hBStatic;
    HBRUSH hSBBrush[3];
    int RWID;
    int GWID;
    int BWID;
    int RBID;
    int GBID;
    int BBID;
    int WStatic;
    int BStatic;
    HWND BlackSq;
    HWND WhiteSq;
    char WSqColors[3];
    char BSqColors[3];

    void CLSetFocus( HWND );
    bool GetColorValue( WORD );
};

class TPieceValueDialog : public TDialog
{
public:
    TPieceValueDialog( PTWindowsObject AParent, LPSTR AName );

    virtual void SetupWindow();

    virtual void Ok( RTMessage );
    virtual void WMHScroll( RTMessage );
    virtual void WMCommand( RTMessage );

    virtual void PVKillFocus( RTMessage ) = [WM_FIRST + PV_KILLFOCUS];
    virtual void IDDDefault( RTMessage ) = [ID_FIRST + IDD_DEFAULT];

private:
    enum ePIECES { pvqueen, pvrook, pvbishop, pvknight, pvpawn };

    int Values[5];
    HWND hScroller[5];
    unsigned int ScollerIDs[5];

    void PVSetFocus( HWND );
    bool GetColorValue( WORD );
};

class TEditBarWindow: public TWindow
{
public:
    TEditBarWindow( PTWindowsObject AParent, LPSTR ATitle );

    virtual void Paint( HDC PaintDC, PAINTSTRUCT _FAR & PaintInfo );
    virtual void WMLButtonUp( TMessage& ) = [WM_FIRST + WM_LBUTTONUP];
    virtual void GetWindowClass( WNDCLASS& WndClass )

    int GetSelectedItem()
    {
        return SelectedItem;
    }

private:
    int SelectedItem;
    RECT EditBarRect;
};

class TInfoWindow: public TWindow
{
public:
    TInfoWindow( PTWindowsObject AParent, LPSTR ATitle );

    void SetTurnText( LPSTR text ) { Color->SetText( text ); }
    void SetWhiteInfoText( LPSTR text ) { WhiteInfo->SetText( text ); }
    void SetBlackInfoText( LPSTR text ) { BlackInfo->SetText( text ); }
    void SetTimerText( LPSTR text ) { Timer->SetText( text ); }
    void SetLevelText( LPSTR text ) { txtLevel->SetText( text ); }
    void SetIterationText( LPSTR text ) { Iteration->SetText( text ); }
    void SetValueText( LPSTR text ) { Value->SetText( text ); }
    void SetNodeText( LPSTR text ) { txtNodes->SetText( text ); }
    void SetSecondsText( LPSTR text ) { Seconds->SetText( text ); }
    void SetDepthText( LPSTR text ) { Depth->SetText( text ); }
    void SetBestLineText( LPSTR text ) { BestLine->SetText( text ); }
    void SetMessageText( LPSTR text ) { txtMessage->SetText( text ); }

    void Reset( void );

protected:
    virtual void Paint( HDC PaintDC, PAINTSTRUCT _FAR &PaintInfo );
    virtual void WMControlColor( TMessage & Msg ) = [WM_FIRST + WM_CTLCOLOR];

    PTStatic                Color;
    PTStatic                WhiteInfo;
    PTStatic                BlackInfo;
    PTNoEraseBkGndStatic    Timer;
    PTStatic                txtLevel;
    PTStatic                Iteration;
    PTStatic                Value;
    PTStatic                txtNodes;
    PTStatic                Seconds;
    PTNoEraseBkGndStatic    Depth;
    PTStatic                BestLine;
    PTStatic                txtMessage;
    RECT                    InfoRect;

    void DrawSFrame( HDC, RECT * );
    void DrawStaticFrames( HDC );

    void IterReset( void );
};

class TChessWindow : public TWindow
{
public:
    TChessWindow( PTWindowsObject AParent, LPSTR ATitle );
    ~TChessWindow();


    void set_menu() { SetMenu( hMenu); };
    void set_edit_menu() { SetMenu( hEditMenu); };
    void set_think_menu() { SetMenu( ThinkMenu); };
    void set_main_menu() { SetMenu( MainMenu); };

private:
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
