// OWLCVT 08/22/97 02:12:07
// ObjectWindows - (C) Copyright 1992 by Borland International

#include <owl\owlpch.h>
#pragma hdrstop

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "wcdefs.h"
#include "externs.h"


struct SAVERESTORE {
	ENUMCOLOR 	Player;
	ENUMCOLOR 	Opponent;
	ENUMCOLOR 	ProgramColor;
	BOOL 		Turned;
	BOOL 		MultiMove;
	BOOL 		AutoPlay;
	BOOL 		SingleStep;
	LEVELTYPE 	Level;
	double 		AverageTime;
	BOOL 		MaxLevel;
	int 		MoveNo;
	double 		ChessClockTotalTime;
	double 		BlackTotalTime;
	double 		WhiteTotalTime;
	int 		PieceValue[7];
};


void SaveGame(char *savefile) {

	if (strlen(savefile) > 0) {
	FILE *gameboard;
	SAVERESTORE *Save;

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


/****************************************************************************/

void RestoreGame(char *loadfile)
{
	 if (strlen(loadfile)) {
		FILE *load;
		SAVERESTORE *Restore;

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
	 } else
		Error("Game not loaded");
}

