// OWLCVT 08/22/97 02:12:47
// ObjectWindows - (C) Copyright 1992 by Borland International

//  Owl Chess is based upon the chess program originally released
//  with TURBO PASCAL GameWorks.  The chess engine was ported over
//  to C, and is essentially unchanged here.  This demonstrates
//  how OWL may be used to place a WINDOWS wrapper around
//  DOS code, whether it is written in C or C++.


#include "owl\compat.h"
#include <owl\static.h>
#include <owl\filedial.h>
#include <owl\inputdia.h>
#include <bwcc.h>
#include <string.h>
#include <stdio.h>

#include "wcdefs.h"
#include "info.h"
#include "wchess.h"
#include "owl\edit.h"
#include "colors.h"
#include "pvalue.h"
#include "externs.h"

#undef MAXPATH
#define MAXPATH  160

/*
 *  Global Variables
 */

BYTE WhiteSquareColors[3] = { ~0, ~0, ~0 };  /* white */
BYTE BlackSquareColors[3] = { 0, 128, 0 };   /* green */

BOOL ShowBestLine = TRUE;
PTInfoWindow TInfo;
HBRUSH hBlackBrush;
HBRUSH hWhiteBrush;
HWND  hWndMain;
HBITMAP PieceBmpArray[6][2];
HBITMAP MaskArray[6];
HCURSOR hArrowCursor;
HCURSOR hWaitCursor;
HMENU ThinkMenu, MainMenu;
HANDLE hAccel;
COLORTYPE ComputerColor;
short LINESIZE;
short CHARSIZE;
int CAPTIONY;
BOOL Editing;
int BORDERSIZE;
const BORDERYEXTRA = 4; // 4 for spacing
short INFOXSIZE, INFOYSIZE;
BOOL SoundOn;

// --------------- TChess ----------------------

class TChess: public TApplication
{
  public:
   TChess(LPSTR AName, HINSTANCE hInstance, HINSTANCE hPrevInstance,
      LPSTR lpCmdLine, int nCmdShow) : TApplication(AName, hInstance,
      hPrevInstance, lpCmdLine, nCmdShow)
      {
      }
   virtual void InitMainWindow();
   void InitInstance()
   {
      // always call the base class version first
      TApplication::InitInstance();
      hAccel = HAccTable = LoadAccelerators(hInstance, "ChessCommands");
      BWCCGetVersion();  // init of BWCC required to assure load of BWCC.DLL
   }
};

// --------------- TMessageDialog ----------------------

// This creates a BWCC style message box using a static template
// from the RC file.  A more versatile version would dynamically
// create the message box to fit the size of each message, and allow
// the user to specify what type of buttons to display, much like
// the windows version of ::MessageBox

_CLASSDEF(TMessageDialog)
class TMessageDialog : public TDialog
{
   LPSTR NewMessage;
   LPSTR NewTitle;
   public:
      TMessageDialog(TWindow * AParent, int ResourceId, LPSTR Message, LPSTR Title);
      virtual void SetupWindow();
};

TMessageDialog::TMessageDialog(TWindow * AParent, int ResourceId,
   LPSTR Message, LPSTR Title)
   : TDialog(AParent, ResourceId)
{
   NewMessage = Message;
   NewTitle = Title;
}

void TMessageDialog::SetupWindow()
{
   // failure to explicitly call the ancestor version of SetupWindow
   // will result in incorrect behavior.  It should always be done
   // first in any derivation of SetupWindow.
   TDialog::SetupWindow();
   ::SetDlgItemText(HWindow, ID_INPUT, NewMessage);
   ::SetWindowText(HWindow, NewTitle);
}

// --------------- TEndDialog ----------------------

_CLASSDEF(TEndDialog)
class TEndDialog : public TDialog
{
   LPSTR EndMessage;
   public:
      TEndDialog(TWindow * AParent, int ResourceId, LPSTR Message);
      virtual void SetupWindow();
      inline void TEndDialog::Yes( UINT )
{

         CloseWindow(IDYES);
      }
      inline void TEndDialog::No( UINT )
{

         ShutDownWindow();
      }
DECLARE_RESPONSE_TABLE( TEndDialog );
};

TEndDialog::TEndDialog(TWindow * AParent, int ResourceId, LPSTR Message)
   : TDialog(AParent, ResourceId)
{
   EndMessage = Message;
}

void TEndDialog::SetupWindow()
{
   TDialog::SetupWindow();
   ::SetDlgItemText(HWindow, ID_INPUT, EndMessage);
}


// --------------- TChessWindow ----------------------

_CLASSDEF(TChessWindow)

class TChessWindow : public TFrameWindow
{
   enum TURNTYPE { player, computer };
   PTInfoWindow TInfo;       // pointer to the information window class
   TEditBarWindow *TEditBar; // pointer to the edit bar class
   BOOL NewGame;             // a new game is being set up?
   char *FileName;           // last saved game
   RECT MainWndRect;         // coordinates for the main window
   RECT InfoAreaRect;        // coordinates for the information window
   HMENU hMenu;              // handle to the main menu
   HMENU hThinkMenu;         // handle to the short (stop) menu
   HMENU hEditMenu;          // handle to the edit (arrange) menu
   TURNTYPE WhoseTurn;       // players or computers turn?
   SQUARETYPE MoveStartSquare; // starting square for move
   SQUARETYPE MoveEndSquare; // ending square for move
   COLORTYPE CurPlayer;      // current player
   BOOL GotStartSquare;      // did the user select a valid piece to move?
   HBRUSH hBKBrush;          // handle to brush to paint main window backgnd
   HBITMAP hBKBrushBmp;      // small bitmap used to paint main window bkgnd
   BOOL Modified;            // has the board been changed in edit mode?
   BOOL EditingBoard;        // is the user editing the board
   BOOL Dragging;            // in drag mode?
   BOARDTYPE *SaveBoard;     // saved board setup in case user cancels edit
  public:
   TChessWindow(TWindow * AParent, LPSTR ATitle);
   ~TChessWindow();
   virtual void SetupWindow();
   virtual void GetWindowClass(WNDCLASS& WndClass)
   {
      // Again, call the ancestor version first, as must also
      // be done with SetupWindow and InitInstance
      TFrameWindow::GetWindowClass( WndClass );

      // this allows you to specify your own values for the
      // window class, which OWL will register for you

      ::hArrowCursor = WndClass.hCursor = LoadCursor(0, IDC_ARROW);
      hBKBrushBmp = LoadBitmap(GetApplication()->hInstance, "BKBrushBmp");
      hBKBrush = CreatePatternBrush(hBKBrushBmp);

      WndClass.hbrBackground = hBKBrush;
      WndClass.lpszMenuName = "TChessMenu";
      WndClass.hIcon = LoadIcon(GetApplication()->hInstance, "ChessIcon");
   }
   virtual void Paint( HDC PaintDC, PAINTSTRUCT& PaintInfo );
   void EvLButtonDown( UINT, TPoint& );
   void EvMouseMove( UINT, TPoint& );
   void CMNewGame();
   void CMRestoreGame();
   void CMSaveGame();
   void CMSaveGameAs();
   inline void TChessWindow::Exit()
{

      CloseWindow();
      }
   void UndoMove();
   void RedoUndo();
	void IDMMovePieces();
	void IDMMoveTime();
	void IDMTotalTime();
	void IDMDemo();
	void IDMInfiniteSearch();
	void IDMPlySearch();
	void IDMMateSearch();
	void IDMSinglePlayer();
	void IDMTwoPlayer();
	void IDMAutoPlay();
	void IDMReverseBoard();
   void IDMEasy();
   void IDMHint();
   void IDMPass();
   void EMDone();
   void EMClear();
   void EMCancel();
   void EMError();
   void IDMMatching();
   void IDMWhiteTurn();
   void IDMBlackTurn();
   void IDMColors();
   void IDMPieceValues();
   void IDMBestLine();
   void IDMMouseDrag();
   void IDMMouseSelect();
   void IDMSound();
   void EvTimer( UINT );
   void EndGame();
   void DoEdit();
   void SaveGameAs();
   void ComputersTurn();
   void UnCheckLevelMenu(LEVELTYPE);
   void NormalSetup();
   void CheckLevelMenu(LEVELTYPE);
   virtual BOOL CanClose()
      {
         if (ComputerThinking || AutoPlay)
            {
            ::PostMessage(HWindow, WM_COMMAND, CM_STOP, 0L);
            ::PostMessage(HWindow, WM_COMMAND, IDM_EXIT, 0L);
            return FALSE;
            }
         QuitProgram();
         return TRUE;
      }
DECLARE_RESPONSE_TABLE( TChessWindow );
};


// ------------- TChess member functions -----------------------

void TChess::InitMainWindow()
{
   MainWindow = new TChessWindow(NULL, "OWL Chess");
}

// ------------- TChessWindow member functions -----------------

TChessWindow::TChessWindow(TWindow * AParent, LPSTR ATitle) :
   TFrameWindow(AParent, ATitle)
{
   HDC hDC;
   hDC = GetDC(HWindow);
   TEXTMETRIC tm;
   GetTextMetrics(hDC, &tm);
   CHARSIZE = tm.tmAveCharWidth;
   LINESIZE = tm.tmHeight + tm.tmExternalLeading;
   ReleaseDC(HWindow, hDC);
   CAPTIONY = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYMENU);
   BORDERSIZE = LINESIZE + MYFRAMESIZE;
   TInfo = new TInfoWindow(this, "InfoWindow");
   TEditBar = new TEditBarWindow(this, "EditBar");
   SoundOn = TRUE;
   ::TInfo = TInfo;
   Attr.Style = WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX;
   Attr.X = 5;
   Attr.Y = 5;
   Attr.H = CAPTIONY + (BORDERSIZE * 2) + INFOYSIZE + BORDERYEXTRA +
      (2 * GetSystemMetrics(SM_CYBORDER));

   Attr.W = (BORDERSIZE * 4) + (MAXBDSIZE * SQUARE_SIZE) + INFOXSIZE +
      (2 * GetSystemMetrics(SM_CXBORDER)) + (2*MYFRAMESIZE);
   FileName = new char[MAXPATH];
   WhoseTurn = player;
   CurPlayer = white;
   ::ComputerColor = black;
   ::Editing = EditingBoard = GotStartSquare = FALSE;
   NewGame = TRUE;
}

TChessWindow::~TChessWindow()
{
   delete FileName;
   delete TInfo;
   DeleteObject(hWhiteBrush);
   DeleteObject(hBlackBrush);
   DeleteObject(hBKBrushBmp);
   DestroyMenu(hThinkMenu);
   DestroyMenu(hEditMenu);
   for (int i = 0; i < 6; i++)
      {
      DeleteObject(MaskArray[i]);
      for (int j = 0; j < 2; j++)
         DeleteObject(PieceBmpArray[i][j]);
      }
   ::KillTimer(HWindow, TIMEID);
}


void TChessWindow::SetupWindow()
{
   TFrameWindow::SetupWindow();
   ::hWndMain = HWindow;
   ::hWaitCursor = LoadCursor(0, IDC_WAIT);
   ::PieceBmpArray[pawn-1][white] =
      LoadBitmap(GetApplication()->hInstance, "WPawnBmp");
    ::PieceBmpArray[pawn-1][black] =
      LoadBitmap(GetApplication()->hInstance, "BPawnBmp");
   ::PieceBmpArray[rook-1][white] =
      LoadBitmap(GetApplication()->hInstance, "WRookBmp");
   ::PieceBmpArray[rook-1][black] =
      LoadBitmap(GetApplication()->hInstance, "BRookBmp");
   ::PieceBmpArray[knight-1][black] =
      LoadBitmap(GetApplication()->hInstance, "BKnightBmp");
   ::PieceBmpArray[bishop-1][black] =
      LoadBitmap(GetApplication()->hInstance, "BBishopBmp");
   ::PieceBmpArray[queen-1][black] =
      LoadBitmap(GetApplication()->hInstance, "BQueenBmp");


   ::PieceBmpArray[knight-1][white] =
      LoadBitmap(GetApplication()->hInstance, "WKnightBmp");
   ::PieceBmpArray[bishop-1][white] =
      LoadBitmap(GetApplication()->hInstance, "WBishopBmp");
   ::PieceBmpArray[queen-1][white] =
      LoadBitmap(GetApplication()->hInstance, "WQueenBmp");

   ::PieceBmpArray[king-1][white] =
      LoadBitmap(GetApplication()->hInstance, "WKingBmp");
   ::PieceBmpArray[king-1][black] =
      LoadBitmap(GetApplication()->hInstance, "BKingBmp");

   ::MaskArray[pawn-1] =
      LoadBitmap(GetApplication()->hInstance, "PMaskBmp");
   ::MaskArray[rook-1] =
      LoadBitmap(GetApplication()->hInstance, "RMaskBmp");
   ::MaskArray[knight-1] =
      LoadBitmap(GetApplication()->hInstance, "KTMaskBmp");
   ::MaskArray[bishop-1] =
      LoadBitmap(GetApplication()->hInstance, "BMaskBmp");
   ::MaskArray[queen-1] =
      LoadBitmap(GetApplication()->hInstance, "QMaskBmp");
   ::MaskArray[king-1] =
      LoadBitmap(GetApplication()->hInstance, "KMaskBmp");

	::GetClientRect(HWindow, &MainWndRect);
   InfoAreaRect = MainWndRect;
   InfoAreaRect.left = (MainWndRect.right -= ((BORDERSIZE * 2) + INFOXSIZE));
   MainMenu = hMenu = ::GetMenu(HWindow);
   ::ThinkMenu = hThinkMenu = LoadMenu(GetApplication()->hInstance, "TChessThinkMenu");
   hEditMenu = LoadMenu(GetApplication()->hInstance, "TChessEditMenu");

   ::Talk();
   CheckLevelMenu(Level);
   ::hWhiteBrush = CreateSolidBrush(
      RGB(WhiteSquareColors[0], WhiteSquareColors[1], WhiteSquareColors[2]));
   ::hBlackBrush = CreateSolidBrush(
      RGB(BlackSquareColors[0], BlackSquareColors[1], BlackSquareColors[2]));
   Dragging = TRUE;
}


void TChessWindow::Paint(HDC PaintDC, PAINTSTRUCT&)
{
   DrawFrame(PaintDC, MainWndRect);
   DrawFrame(PaintDC, InfoAreaRect);
   PrintBoard();
}



void TChessWindow::EvLButtonDown( UINT, TPoint& )
{

   POINT Point;
   if (EditingBoard)
      {
      DoEdit();
      return;
      }

   if (WhoseTurn == computer && NoComputerMove == FALSE)
      return;
   GetCursorPos( &Point );
   ::ScreenToClient(HWindow, &Point);

   if (!GotStartSquare)
      {
      MoveStartSquare = GetValidSquare(Point, CurPlayer, TRUE);
      if (MoveStartSquare == -1)
         return;
      GotStartSquare = TRUE;
      if (!Dragging)
         DrawInvertedBitmap(MoveStartSquare);
      else
         DragStart(MoveStartSquare, Point);
      }
   else
      {
      MoveEndSquare = GetValidSquare(Point, CurPlayer, FALSE);
      GotStartSquare = FALSE;
      if (MoveEndSquare == -1)
         {
         if (Dragging)
              DragEnd(FALSE);
         Warning("Invalid Move");
         DrawNormalBitmap(MoveStartSquare);
         return;
         }
      if (!MoveCheck(MoveStartSquare, MoveEndSquare))
         {
         if (Dragging)
            DragEnd(FALSE);
         DrawNormalBitmap(MoveStartSquare);
         return;
         }
      ::GotValidMove = TRUE;
      EnableMenuItem(hMenu, CM_UNDO, MF_BYCOMMAND | MF_ENABLED);
      if (GameOver)
         EndGame();
      else if (NoComputerMove == FALSE)
         ComputersTurn();
      else if (::MultiMove)
         {
         CurPlayer = (CurPlayer == white) ? black : white;
         }
      }
}

void TChessWindow::ComputersTurn()
{
   WhoseTurn = computer;
   ::SetClassWord(HWindow, GCW_HCURSOR, WORD( ::hWaitCursor ));
   ::SetCursor(::hWaitCursor);
   ::SetMenu(HWindow, hThinkMenu);
   ProgramMove();
   ::SetMenu(HWindow, hMenu);
   WhoseTurn = player;
   ::SetClassWord( HWindow, GCW_HCURSOR, WORD( hArrowCursor ));
   if (GameOver)
      EndGame();
}

void TChessWindow::CMNewGame()
{

   ::KillTimer(HWindow, TIMEID);
   TInfo->SetTimerText("");
   EnableMenuItem(hMenu, CM_UNDO, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
   EnableMenuItem(hMenu, CM_REDO, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
   NewGame = TRUE;
   ::NewGame();
   ::PrintBoard();
   CurPlayer = Player;
   ComputerColor = Opponent;
}

void TChessWindow::CMRestoreGame()
{

   if (GetApplication()->ExecDialog(new TFileDialog(this,
      SD_FILEOPEN, strcpy(FileName, "*.CHS"))) == IDOK)
      {
      NewGame = FALSE;
      ::RestoreGame(FileName);
      CurPlayer = Player;
      ComputerColor = Opponent;
      EnableMenuItem(hMenu, CM_REDO, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
      EnableMenuItem(hMenu, CM_UNDO, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
      }
}

void TChessWindow::SaveGameAs()
{
   if (GetApplication()->ExecDialog(new TFileDialog(this,
      SD_FILESAVE, strcpy(FileName, "*.CHS")))
         == IDOK)
      {
      NewGame = FALSE;
      ::SaveGame(FileName);
      }
}

void TChessWindow::CMSaveGame()
{

   if (NewGame == TRUE)
      SaveGameAs();
   else
      ::SaveGame(FileName);
}

void TChessWindow::CMSaveGameAs()
{

   SaveGameAs();
}

void TChessWindow::EMError()
{

   GetApplication()->ExecDialog(new TMessageDialog(this, SD_CHESSERROR,
      (LPSTR)buf, "OWL Chess Error"));
}

void TChessWindow::DoEdit()
{
   POINT Point;
   SQUARETYPE Square;
   int SelectedItem;

   Modified = TRUE;
   GetCursorPos( &Point );
   ::ScreenToClient(HWindow, &Point);
   Square = GetValidSquare(Point, black, FALSE);

   if (Board[Square].piece != empty)
      {
      Board[Square].piece = empty;
      ::UpdateBoard();
      return;
      }
   SelectedItem = TEditBar->GetSelectedItem();

   Board[Square].piece = (PIECETYPE)(SelectedItem % 6 + 1);
   Board[Square].color = (SelectedItem < 6) ? white : black;
   ::UpdateBoard();
}

void TChessWindow::IDMMovePieces()
{

   SaveBoard = new BOARDTYPE[0x78];
   if (SaveBoard == NULL)
      {
      Error("Not enough memory");
      return;
      }
   memcpy(SaveBoard, Board, sizeof(BOARDTYPE) * 0x78);
   Editing = EditingBoard = TRUE;
   TInfo->Show(SW_HIDE);
   ::SetMenu(HWindow, hEditMenu);
   Modified = FALSE;
   if (CurPlayer == white)
      {
      CheckMenuItem(hEditMenu, IDM_BLACKTURN, MF_UNCHECKED);
      CheckMenuItem(hEditMenu, IDM_WHITETURN, MF_CHECKED);
      }
   else      {
      CheckMenuItem(hEditMenu, IDM_BLACKTURN, MF_CHECKED);
      CheckMenuItem(hEditMenu, IDM_WHITETURN, MF_UNCHECKED);
      }
   TEditBar->Show(SW_NORMAL);
}

void TChessWindow::EMDone()
{

   SQUARETYPE sq;
   int KingCount[2] = { 0, 0 };
   int TotalCount[2] = { 0, 0 };
   BOOL Done = FALSE;

   if (Modified)
      {
      for (sq = 0; sq < 0x78; sq++)
         if (!(sq & 0x88))
            {
            if (Board[sq].piece != empty)
               {
               TotalCount[Board[sq].color]++;
               if (Board[sq].piece == king)
                  KingCount[Board[sq].color]++;
               }
            }
      if (TotalCount[white] <= 16 && KingCount[white] == 1
         && TotalCount[black] <= 16 && KingCount[black] == 1)
         {
         ResetNewPos();
         if (!Attacks(Player, PieceTab[Opponent][0].isquare))
            Done = TRUE;
         else
            Error("Illegal King position");
          }
      else
          Error("Wrong number of pieces");
      }
   else
      NormalSetup();

   if (Done)
      NormalSetup();
}

void TChessWindow::EMClear()
{

   SQUARETYPE sq;

   Modified = TRUE;
   for (sq = 0; sq <= 0x77; sq++)
      Board[sq].piece = empty;
   ::UpdateBoard();
}

void TChessWindow::EMCancel()
{

   memcpy(Board, SaveBoard, sizeof(BOARDTYPE) * 0x78);
   ResetNewPos();
   NormalSetup();
}


void TChessWindow::NormalSetup()
{
   TEditBar->Show(SW_HIDE);
   TInfo->Show(SW_NORMAL);
   ::SetMenu(HWindow, hMenu);
   delete[] SaveBoard;
   Editing = EditingBoard = FALSE;
   ::UpdateBoard();
}

void TChessWindow::IDMMoveTime()
{

   char *SecsPerMove = new char[40];

   sprintf(SecsPerMove, "%.2lf", ::AverageTime);
   if (GetApplication()->ExecDialog(new TInputDialog(this, "Set Seconds per Move",
      "Seconds Per Move:", SecsPerMove, 19)) == IDOK)
      {
      double NewMoveTime = atof(SecsPerMove);
      if (NewMoveTime > 0.)
         {
         if (::Level != normal)
            {
            UnCheckLevelMenu(Level);
            CheckMenuItem(hMenu, IDM_MOVETIME, MF_CHECKED);
            }
         ::Level = normal;
         ::AverageTime = NewMoveTime;
         PrintCurLevel();
         for (COLORTYPE color = white; color <= black; ((int)color)++)
            ::ChessTime[color].totaltime = (::MoveNo / 2) * ::AverageTime;
         ::MaxLevel = MAXPLY;
         }
      else
         Error("Invalid time.  No change made.");
      }
   delete SecsPerMove;
}


void TChessWindow::IDMTotalTime()
{

   char *TotalMoveTime = new char[40];
   sprintf(TotalMoveTime, "%.2lf", ::AverageTime);

   if (GetApplication()->ExecDialog(new TInputDialog(this, "Set Seconds per Game",
      "Seconds Per Game:", TotalMoveTime, 19)) == IDOK)
      {
      double NewMoveTime = atof(TotalMoveTime);
      if (NewMoveTime > 0.)
         {
         if (::Level != fullgametime)
            {
            UnCheckLevelMenu(Level);
            CheckMenuItem(hMenu, IDM_TOTALTIME, MF_CHECKED);
            }
         ::Level = fullgametime;
         ::AverageTime = NewMoveTime;
         ::PrintCurLevel();
         for (COLORTYPE color = white; color <= black; ((int)color)++)
            ::ChessTime[color].totaltime = (::MoveNo / 2) * ::AverageTime;
         ::MaxLevel = MAXPLY;
         }
      else
         Error("Invalid time.  No change made.");
      }
   delete TotalMoveTime;
}

void TChessWindow::IDMDemo()
{

   extern BOOL AutoPlay;
   BOOL IsEasy = FALSE;
   if (::Level == easygame)
      {
      IsEasy = TRUE;
      ::Level = normal;
      HideAttacks();
      }
   AutoPlay = TRUE;
   ComputersTurn();
   if (IsEasy)
      {
      ::Level = easygame;
      ::UpdateBoard();
      }
   CurPlayer = Player;
   ComputerColor = Opponent;
   ::PrintCurLevel();
}

void TChessWindow::IDMMatching()
{

   if (::Level != matching)
      {
      UnCheckLevelMenu(Level);
      CheckMenuItem(hMenu, IDM_MATCHING, MF_CHECKED);
      }
   ::Level = matching;
   ::PrintCurLevel();
}

void TChessWindow::IDMInfiniteSearch()
{

   if (::Level != infinite)
      {
      UnCheckLevelMenu(Level);
      CheckMenuItem(hMenu, IDM_INFINITE, MF_CHECKED);
      }
   ::Level = infinite;
   ::MaxLevel = MAXPLY;
   ::PrintCurLevel();
}

void TChessWindow::IDMPlySearch()
{

   char *PlySearchDepth = new char[40];
   sprintf(PlySearchDepth, "%d", ::MaxLevel);
   if (GetApplication()->ExecDialog(new TInputDialog(this, "Set ply depth", "Number of plys:",
      PlySearchDepth, 19)) == IDOK)
      {
      int NewPlyDepth = atoi(PlySearchDepth);
      if (NewPlyDepth > 0)
         {
         if (::Level != plysearch)
            {
            UnCheckLevelMenu(Level);
            CheckMenuItem(hMenu, IDM_PLY, MF_CHECKED);
            }
         ::MaxLevel = (BYTE)((NewPlyDepth > MAXPLY) ? MAXPLY : NewPlyDepth);
         ::Level = plysearch;
         ::PrintCurLevel();
         }
      else
        ::Error("Invalid time.  No change made.");
      }
   delete PlySearchDepth;
}

void TChessWindow::IDMMateSearch()
{

   if (::Level != matesearch)
      {
      UnCheckLevelMenu(Level);
      CheckMenuItem(hMenu, IDM_MATE, MF_CHECKED);
      }
   ::Level = matesearch;
   ::PrintCurLevel();
   ComputersTurn();
}

void TChessWindow::IDMSinglePlayer()
{

}

void TChessWindow::IDMTwoPlayer()
{

   static LEVELTYPE OldLevel = normal;
   ::MultiMove = !::MultiMove;


   if (::MultiMove)
      {
      OldLevel = ::Level;
      ModifyMenu(hMenu, IDM_TWOPLAYER, MF_BYCOMMAND | MF_STRING,
         IDM_TWOPLAYER, "&Single Player");
      EnableMenuItem(hMenu, 2, MF_GRAYED | MF_BYPOSITION);
      ::DrawMenuBar(HWindow);
      ::Level = normal;
      ::PrintCurLevel();
      }
   else
      {
      ModifyMenu(hMenu, IDM_TWOPLAYER, MF_BYCOMMAND | MF_STRING,
         IDM_TWOPLAYER, "&Two Player");
      EnableMenuItem(hMenu, 2, MF_ENABLED | MF_BYPOSITION);
      ::DrawMenuBar(HWindow);
      ::Level = OldLevel;
      ::PrintCurLevel();
      }
}

void TChessWindow::IDMAutoPlay()
{

}

void TChessWindow::IDMReverseBoard()
{

   ::Turned = !::Turned;
   ::PrintBoard();
}

void TChessWindow::UndoMove()
{


   if (ComputerThinking)
      {
      MessageToPost = CM_UNDO;
      return;
      }
   if (!Undo())
      EnableMenuItem(hMenu, CM_UNDO, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
   EnableMenuItem(hMenu, CM_REDO, MF_BYCOMMAND | MF_ENABLED);
   CurPlayer = Player;
   ComputerColor = Opponent;
}


void TChessWindow::RedoUndo()
{

   if (ComputerThinking)
      {
      MessageToPost = CM_REDO;
      return;
      }
   if (!Redo())
      EnableMenuItem(hMenu, CM_REDO, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
   EnableMenuItem(hMenu, CM_UNDO, MF_BYCOMMAND | MF_ENABLED);
   CurPlayer = Player;
   ComputerColor = Opponent;
}


void TChessWindow::UnCheckLevelMenu(LEVELTYPE level)
{
   WORD CheckItem = IDM_EASY;

   switch (level)
      {
      case normal :
         CheckItem = IDM_MOVETIME;
         break;
      case fullgametime :
         CheckItem = IDM_TOTALTIME;
         break;
      case plysearch :
         CheckItem = IDM_PLY;
         break;
      case easygame :
         CheckItem = IDM_EASY;
         break;
      case infinite :
         CheckItem = IDM_INFINITE;
         break;
      case matesearch :
         CheckItem = IDM_MATE;
         break;
      case matching :
         CheckItem = IDM_MATCHING;
         break;
      }
   CheckMenuItem(hMenu, CheckItem, MF_UNCHECKED);
}

void TChessWindow::CheckLevelMenu(LEVELTYPE level)
{
   WORD CheckItem = IDM_EASY;

   switch (level)
      {
      case normal :
         CheckItem = IDM_MOVETIME;
         break;
      case fullgametime :
         CheckItem = IDM_TOTALTIME;
         break;
      case plysearch :
         CheckItem = IDM_PLY;
         break;
      case easygame :
         CheckItem = IDM_EASY;
         break;
      case infinite :
         CheckItem = IDM_INFINITE;
         break;
      case matesearch :
         CheckItem = IDM_MATE;
         break;
      case matching :
         CheckItem = IDM_MATCHING;
         break;
      }
   CheckMenuItem(hMenu, CheckItem, MF_CHECKED);
}


void TChessWindow::IDMEasy()
{

   if (::Level != easygame)
      {
      UnCheckLevelMenu(Level);
      CheckMenuItem(hMenu, IDM_EASY, MF_CHECKED);
      }
   ::Level = easygame;
   ::AverageTime = 5.;
   ::MaxLevel = MAXPLY;
   ::PrintCurLevel();
}

void TChessWindow::EndGame()
{
   if (GetApplication()->ExecDialog(new TEndDialog(this, SD_ENDGAMEDLG,
      (LPSTR) EndGameMessage)) != IDYES)
      {
      ::PostMessage(hWndMain, WM_COMMAND, CM_EXIT, 0L);
      return;
      }
   GameOver = FALSE;
   ::PostMessage(hWndMain, WM_COMMAND, CM_FILENEW, 0L);
   return;
}

void TChessWindow::IDMHint()
{

   FindHintMove();
   ShowHint();
}

void TChessWindow::IDMPass()
{

   if (::Level == easygame)
      HideAttacks();
   CurPlayer = Opponent;
   ComputerColor = Player;
   ComputersTurn();
}

void TChessWindow::IDMWhiteTurn()
{

   CheckMenuItem(hEditMenu, IDM_BLACKTURN, MF_UNCHECKED);
   CheckMenuItem(hEditMenu, IDM_WHITETURN, MF_CHECKED);
   CurPlayer = white;
   ComputerColor = black;
   if (CurPlayer != Player)
      {
      Opponent = ComputerColor;
      Player = ProgramColor = CurPlayer;
      }
}


void TChessWindow::IDMBlackTurn()
{

   CheckMenuItem(hEditMenu, IDM_BLACKTURN, MF_CHECKED);
   CheckMenuItem(hEditMenu, IDM_WHITETURN, MF_UNCHECKED);
   CurPlayer = black;
   ComputerColor = white;
   if (CurPlayer != Player)
      {
      Opponent = ComputerColor;
      Player = ProgramColor = CurPlayer;
      }
}

void TChessWindow::IDMColors()
{

   GetApplication()->ExecDialog(new TColorsDialog(this, "ColorsDialog"));
   DeleteObject(hWhiteBrush);
   DeleteObject(hBlackBrush);
   ::hWhiteBrush = CreateSolidBrush(
      RGB(WhiteSquareColors[0], WhiteSquareColors[1], WhiteSquareColors[2]));
   ::hBlackBrush = CreateSolidBrush(
      RGB(BlackSquareColors[0], BlackSquareColors[1], BlackSquareColors[2]));
}

void TChessWindow::IDMPieceValues()
{

   GetApplication()->ExecDialog(new TPieceValueDialog(this, "PieceValueDlg"));
}

void TChessWindow::IDMBestLine()
{

   ShowBestLine = !ShowBestLine;

   if (ShowBestLine)
      CheckMenuItem(hMenu, IDM_BESTLINE, MF_CHECKED);
   else
      {
      CheckMenuItem(hMenu, IDM_BESTLINE, MF_UNCHECKED);
      TInfo->SetBestLineText("");
      }
}

void TChessWindow::EvMouseMove( UINT, TPoint& )
{
	TMessage msg = __GetTMessage();

   if (!GotStartSquare || !Dragging)
      return;
   Drag(MAKEPOINT(msg.LParam));
}

void TChessWindow::IDMMouseDrag()
{

   POINT Point;

   if (!Dragging && GotStartSquare)
      {
         DrawNormalBitmap(MoveStartSquare);
         GetCursorPos(&Point);
         DragStart(MoveStartSquare, Point);
      }

   Dragging = TRUE;
   CheckMenuItem(hMenu, IDM_MOUSEDRAG, MF_CHECKED);
   CheckMenuItem(hMenu, IDM_MOUSESELECT, MF_UNCHECKED);
}

void TChessWindow::IDMMouseSelect()
{

   DragEnd(TRUE);
   Dragging = FALSE;
   CheckMenuItem(hMenu, IDM_MOUSEDRAG, MF_UNCHECKED);
   CheckMenuItem(hMenu, IDM_MOUSESELECT, MF_CHECKED);
   if (GotStartSquare)
         DrawInvertedBitmap(MoveStartSquare);
}

void TChessWindow::IDMSound()
{

   SoundOn = !SoundOn;
   if (SoundOn)
      CheckMenuItem(hMenu, IDM_SOUND, MF_CHECKED);
   else
      CheckMenuItem(hMenu, IDM_SOUND, MF_UNCHECKED);
}

void TChessWindow::EvTimer( UINT )
{

   DisplayTime();
}


int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                      LPSTR lpCmdLine, int nCmdShow)
{
   TChess ChessApp("ChessApp", hInstance, hPrevInstance, lpCmdLine,
      nCmdShow);
   ChessApp.Run();
   return ChessApp.Status;
}


DEFINE_RESPONSE_TABLE1( TEndDialog, TDialog )
    EV_CHILD_NOTIFY_ALL_CODES( 6, Yes ),
    EV_CHILD_NOTIFY_ALL_CODES( 7, No ),
END_RESPONSE_TABLE;

DEFINE_RESPONSE_TABLE1( TChessWindow, TFrameWindow )
    EV_WM_LBUTTONDOWN,
    EV_WM_MOUSEMOVE,
    EV_COMMAND( 24329, CMNewGame ),
    EV_COMMAND( 24330, CMRestoreGame ),
    EV_COMMAND( 24333, CMSaveGame ),
    EV_COMMAND( 24334, CMSaveGameAs ),
    EV_COMMAND( 24371, Exit ),
    EV_COMMAND( 24361, UndoMove ),
    EV_COMMAND( 24362, RedoUndo ),
    EV_COMMAND( 24350, IDMMovePieces ),
    EV_COMMAND( 24351, IDMMoveTime ),
    EV_COMMAND( 24352, IDMTotalTime ),
    EV_COMMAND( 24353, IDMDemo ),
    EV_COMMAND( 24354, IDMInfiniteSearch ),
    EV_COMMAND( 24355, IDMPlySearch ),
    EV_COMMAND( 24356, IDMMateSearch ),
    EV_COMMAND( 24357, IDMSinglePlayer ),
    EV_COMMAND( 24358, IDMTwoPlayer ),
    EV_COMMAND( 24359, IDMAutoPlay ),
    EV_COMMAND( 24360, IDMReverseBoard ),
    EV_COMMAND( 24364, IDMEasy ),
    EV_COMMAND( 24365, IDMHint ),
    EV_COMMAND( 24366, IDMPass ),
    EV_COMMAND( 24348, EMDone ),
    EV_COMMAND( 24347, EMClear ),
    EV_COMMAND( 24346, EMCancel ),
    EV_COMMAND( 24344, EMError ),
    EV_COMMAND( 24372, IDMMatching ),
    EV_COMMAND( 24373, IDMWhiteTurn ),
    EV_COMMAND( 24374, IDMBlackTurn ),
    EV_COMMAND( 24375, IDMColors ),
    EV_COMMAND( 24379, IDMPieceValues ),
    EV_COMMAND( 24376, IDMBestLine ),
    EV_COMMAND( 24377, IDMMouseDrag ),
    EV_COMMAND( 24378, IDMMouseSelect ),
    EV_COMMAND( 24380, IDMSound ),
    EV_WM_TIMER,
END_RESPONSE_TABLE;
