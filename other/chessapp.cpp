/*  Project chess
    Naivasha Enterprises
    Copyright © 1995. All Rights Reserved.

    SUBSYSTEM:    chess.exe Application
    FILE:         chessapp.cpp
    AUTHOR:       A.J. Leerling


	 OVERVIEW
	 ========
	 Source file for implementation of chessApp (TApplication).
*/


#include <owl\owlpch.h>
#pragma hdrstop


#include "chessapp.h"
#include "chsswndw.h"                        // Definition of client class.
#include "chssabtd.h"                        // Definition of about dialog.

BYTE WhiteSquareColors[3] = { ~0, ~0, ~0 };  /* white */
BYTE BlackSquareColors[3] = { 0, 128, 0 };   /* green */
BOOL ShowBestLine = TRUE;
TInfoWindow * TInfo;
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

//{{chessApp Implementation}}


//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(chessApp, TApplication)
//{{chessAppRSP_TBL_BEGIN}}
    EV_COMMAND(CM_FILENEW, CmFileNew),
    EV_COMMAND(CM_FILEOPEN, CmFileOpen),
    EV_COMMAND(CM_HELPABOUT, CmHelpAbout),
    EV_COMMAND(CM_FILESAVE, CmFileSave),
    EV_COMMAND(CM_FILESAVEAS, CmFileSaveAs),
//{{chessAppRSP_TBL_END}}
END_RESPONSE_TABLE;


//////////////////////////////////////////////////////////
// chessApp
// =====
//
chessApp::chessApp () : TApplication("Chess")
{

	 // Common file file flags and filters for Open/Save As dialogs.  Filename and directory are
	 // computed in the member functions CmFileOpen, and CmFileSaveAs.
	 FileData.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	 FileData.SetFilter("Chess Files (*.chs)|*.chs|");

	 // INSERT>> Your constructor code here.
}


chessApp::~chessApp ()
{
	 // INSERT>> Your destructor code here.
}


void chessApp::SetupSpeedBar (TDecoratedFrame *frame)
{
	 //
	 // Create default toolbar New and associate toolbar buttons with commands.
	 //
	 TControlBar* cb = new TControlBar(frame);
	 cb->Insert(*new TButtonGadget(CM_FILENEW, CM_FILENEW));
	 cb->Insert(*new TButtonGadget(CM_FILEOPEN, CM_FILEOPEN));
	 cb->Insert(*new TButtonGadget(CM_FILESAVE, CM_FILESAVE));
	 cb->Insert(*new TSeparatorGadget(6));
	 cb->Insert(*new TButtonGadget(CM_EDITCUT, CM_EDITCUT));
	 cb->Insert(*new TButtonGadget(CM_EDITCOPY, CM_EDITCOPY));
	 cb->Insert(*new TButtonGadget(CM_EDITPASTE, CM_EDITPASTE));
	 cb->Insert(*new TSeparatorGadget(6));
	 cb->Insert(*new TButtonGadget(CM_EDITUNDO, CM_EDITUNDO));
	 cb->Insert(*new TSeparatorGadget(6));
	 cb->Insert(*new TButtonGadget(CM_EDITFIND, CM_EDITFIND));
	 cb->Insert(*new TButtonGadget(CM_EDITFINDNEXT, CM_EDITFINDNEXT));

	 // Add fly-over help hints.
	 cb->SetHintMode(TGadgetWindow::EnterHints);

	 frame->Insert(*cb, TDecoratedFrame::Top);
}


//////////////////////////////////////////////////////////
// chessApp
// =====
// Application intialization.
//
void chessApp::InitMainWindow ()
{
	 if (nCmdShow != SW_HIDE)
		  nCmdShow = (nCmdShow != SW_SHOWMINNOACTIVE) ? SW_SHOWNORMAL : nCmdShow;

	 SDIDecFrame *frame = new SDIDecFrame(0, GetName(), 0, true);

	 //
	 // Assign ICON w/ this application.
	 //
	 frame->SetIcon(this, IDI_SDIAPPLICATION);

	 //
	 // Menu associated with window and accelerator table associated with table.
	 //
	 frame->AssignMenu(SDI_MENU);

	 //
	 // Associate with the accelerator table.
	 //
	 frame->Attr.AccelTable = SDI_MENU;

	 SetupSpeedBar(frame);

	 TStatusBar *sb = new TStatusBar(frame, TGadget::Recessed,
												TStatusBar::CapsLock        |
												TStatusBar::NumLock         |
												TStatusBar::ScrollLock      |
												TStatusBar::Overtype);
	 frame->Insert(*sb, TDecoratedFrame::Bottom);

	 SetMainWindow(frame);

	 frame->SetMenuDescr(TMenuDescr(SDI_MENU));

	 //
	 // Borland Windows custom controls.
	 //
	 EnableBWCC();
}


//////////////////////////////////////////////////////////
// chessApp
// ===========
// Menu File New command
void chessApp::CmFileNew ()
{
	::KillTimer(GetMainWindow(), TIMEID);
	TInfo->SetTimerText("");
	EnableMenuItem(hMenu, CM_UNDO, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	EnableMenuItem(hMenu, CM_REDO, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	NewGame = TRUE;
	::NewGame();
	::PrintBoard();
	CurPlayer = Player;
	ComputerColor = Opponent;
}


//////////////////////////////////////////////////////////
// chessApp
// ===========
// Menu File Open command
void chessApp::CmFileOpen ()
{
	 //
	 // Display standard Open dialog box to select a file name.
	 //
	 *FileData.FileName = 0;

	 chessWindow *client = TYPESAFE_DOWNCAST(GetMainWindow()->GetClientWindow(), chessWindow);     // Client window for the frame.
	 if (client->CanClose())
		  if (TFileOpenDialog(this, FileData).Execute() == IDOK)
				OpenFile();

}


void chessApp::OpenFile (const char *fileName)
{
	 if (fileName)
		  strcpy(FileData.FileName, fileName);

	  NewGame = FALSE;
	  ::RestoreGame(FileData.FileName);
	  CurPlayer = Player;
	  ComputerColor = Opponent;
	  EnableMenuItem(hMenu, CM_REDO, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	  EnableMenuItem(hMenu, CM_UNDO, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
}


//{{SDIDecFrame Implementation}}


SDIDecFrame::SDIDecFrame (TWindow *parent, const char far *title, TWindow *clientWnd, bool trackMenuSelection, TModule *module)
	 : TDecoratedFrame(parent, title, clientWnd == 0 ? new chessWindow(0, "") : clientWnd, trackMenuSelection, module)
{
	 // INSERT>> Your constructor code here.

}


SDIDecFrame::~SDIDecFrame ()
{
	 // INSERT>> Your destructor code here.

}


//////////////////////////////////////////////////////////
// chessApp
// ===========
// Menu Help About chess.exe command
void chessApp::CmHelpAbout ()
{
	 //
	 // Show the modal dialog.
	 //
	 chessAboutDlg(MainWindow).Execute();
}


int OwlMain (int , char* [])
{
	 try {
		  chessApp    app;
		  return app.Run();
	 }
	 catch (xmsg& x) {
		  ::MessageBox(0, x.why().c_str(), "Exception", MB_OK);
	 }

	 return -1;
}



void chessApp::CmFileSave ()
{
	 // INSERT>> Your code here.

	if (NewGame == TRUE)
		CmFileSaveAs();
	else
		::SaveGame(FileData.FileName);
}


void chessApp::CmFileSaveAs ()
{
	 // INSERT>> Your code here.

	if (TFileSaveDialog( this, FileData ).Execute() == IDOK) {
		NewGame = FALSE;
		::SaveGame(FileData.FileName);
	}
}


bool chessApp::CanClose ()
{
	 bool result;

	 result = TApplication::CanClose();

	 // INSERT>> Your code here.

	if( result ) {
		if (ComputerThinking || AutoPlay) {
			::PostMessage(this, WM_COMMAND, CM_STOP, 0L);
			::PostMessage(this, WM_COMMAND, IDM_EXIT, 0L);
			result = FALSE;
		} else
			QuitProgram();
	}

	return result;
}

void chessApp::SaveGame( ) {

	FILE *gameboard;
	SAVERESTORE *Save;

  ifstream is(FileData->FileName);

  if (!is) {
		MessageBox("Unable to open file", "File Error", MB_OK | MB_ICONEXCLAMATION);
		return;
  }

	Save = new SAVERESTORE;
	if (Save == NULL) {
		::MessageBox(hWndMain, "Not enough memory to perform operation", "Chess", MB_OK | MB_ICONHAND);
		return;
	}

	if ((gameboard = fopen(savefile, "wb")) == NULL) {
		  sprintf(buf, "Cannot open %s for writing", savefile);
		  ::MessageBox(hWndMain, buf, "Chess", MB_OK | MB_ICONHAND);
		  delete Save;
		  return;
	}

	Save->Player = Player;
	Save->Opponent = Opponent;
	Save->ProgramColor = ProgramColor;
	Save->Turned = Turned;
	Save->MultiMove = MultiMove;
	Save->AutoPlay = AutoPlay;
	Save->SingleStep = SingleStep;
	Save->Level = Level;
	Save->AverageTime = AverageTime;
	Save->MaxLevel = MaxLevel;
	Save->MoveNo = MoveNo;
	Save->ChessClockTotalTime = ChessClock.totaltime;
	Save->BlackTotalTime = ChessTime[black].totaltime;
	Save->WhiteTotalTime = ChessTime[white].totaltime;
	memcpy(Save->PieceValue, PieceValue, 7 * sizeof(int));
	fwrite(Save, sizeof(SAVERESTORE), 1, gameboard);
	fwrite(Board, sizeof(Board), 1, gameboard);
	fclose(gameboard);
	delete Save;

	return;
}

void chessApp::RestoreGame( ) {

		FILE *load;
		SAVERESTORE *Restore;

	 if( strlen( FileData.FileName )) {

		Restore = new SAVERESTORE;

		if (!Restore) {
			::MessageBox(hWndMain, "Not enough memory to perform operation", "Chess", MB_OK | MB_ICONHAND);
			return;
		}

		 if ((load = fopen(loadfile, "rb")) == NULL)
			  Error("File not found");
		 else {
			  fread(Restore, sizeof(SAVERESTORE), 1, load);
			  fread(Board, sizeof(Board), 1, load);
			  fclose(load);
			  Player = Restore->Player;
			  Opponent = Restore->Opponent;
			  ProgramColor = Restore->ProgramColor;
			  Turned = Restore->Turned;
			  MultiMove = Restore->MultiMove;
			  AutoPlay = Restore->AutoPlay;
			  SingleStep = Restore->SingleStep;
			  Level = Restore->Level;
			  AverageTime = Restore->AverageTime;
			  MaxLevel = Restore->MaxLevel;
			  MoveNo = Restore->MoveNo;
			  ChessClock.totaltime = Restore->ChessClockTotalTime;
			  ChessTime[black].totaltime = Restore->BlackTotalTime ;
			  ChessTime[white].totaltime = Restore->WhiteTotalTime;
			  memcpy(PieceValue, Restore->PieceValue, 7 * sizeof(int));
			  ClearDisplay();
			  InitDisplay();
			  ::InvalidateRect(hWndMain, NULL, TRUE);
			  PrintCurLevel();
			  ResetNewPos();
		}
		delete Restore;
	 } else		Error("Game not loaded");
}

