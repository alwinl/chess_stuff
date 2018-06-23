/*  Project chess
    Naivasha Enterprises
    Copyright © 1995. All Rights Reserved.

    SUBSYSTEM:    chess.exe Application
    FILE:         chsswndw.cpp
    AUTHOR:       A.J. Leerling


    OVERVIEW
    ========
    Source file for implementation of chessWindow (TWindow).      
*/


#include <owl\owlpch.h>
#pragma hdrstop

#include "chessapp.h"

#include "chsswndw.h"

#include <stdio.h>


//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(chessWindow, TWindow)
//{{chessWindowRSP_TBL_BEGIN}}
    EV_WM_LBUTTONDOWN,
    EV_COMMAND(IDM_MOVEPIECE, IDMMovePiece),
    EV_WM_MOUSEMOVE,
//{{chessWindowRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{chessWindow Implementation}}


//////////////////////////////////////////////////////////
// chessWindow
// ==========
// Construction/Destruction handling.
chessWindow::chessWindow (TWindow* parent, const char far* title, TModule* module)
	 : TWindow(parent, title, module)
{
	 // INSERT>> Your constructor code here.

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


chessWindow::~chessWindow ()
{
	 Destroy();

	 // INSERT>> Your destructor code here.

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

void chessWindow::SetupWindow ()
{
	 TWindow::SetupWindow();

	 // INSERT>> Your code here.

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


void chessWindow::Paint (TDC& dc, bool erase, TRect& rect)
{
//    TWindow::Paint(dc, erase, rect);

	 // INSERT>> Your code here.

	DrawFrame(dc, MainWndRect);
	DrawFrame(dc, InfoAreaRect);
	PrintBoard();
}


void chessWindow::EvLButtonDown (uint modKeys, TPoint& point)
{
    TWindow::EvLButtonDown(modKeys, point);

    // INSERT>> Your code here.

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



void chessWindow::IDMMovePiece ()
{
	 // INSERT>> Your code here.

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
	else
		{
		CheckMenuItem(hEditMenu, IDM_BLACKTURN, MF_CHECKED);
		CheckMenuItem(hEditMenu, IDM_WHITETURN, MF_UNCHECKED);
		}
	TEditBar->Show(SW_NORMAL);
}


void chessWindow::EvMouseMove (uint modKeys, TPoint& point)
{
	 TWindow::EvMouseMove(modKeys, point);

	 // INSERT>> Your code here.

	if (!GotStartSquare || !Dragging)
		return;
	Drag( point );
}



