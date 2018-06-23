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
	COLORTYPE 	Player;
	COLORTYPE 	Opponent;
	COLORTYPE 	ProgramColor;
	bool 		Turned;
	bool 		MultiMove;
	bool 		AutoPlay;
	bool 		SingleStep;
	LEVELTYPE 	Level;
	double 		AverageTime;
	bool 		MaxLevel;
	int 		MoveNo;
	double 		ChessClockTotalTime;
	double 		BlackTotalTime;
	double 		WhiteTotalTime;
	int 		PieceValue[7];
};


void SaveGame( char *file_name )
{
	if( ! strlen(file_name) )
	    return;

	FILE *gameboard;

	if( (gameboard = fopen( file_name, "wb" )) == NULL ) {
        sprintf( buf, "Cannot open %s for writing", file_name );
        ::MessageBox( hWndMain, buf, "Chess", MB_OK | MB_ICONHAND );
        return;
	}

	SAVERESTORE Save;

	Save.Player = Player;
	Save.Opponent = Opponent;
	Save.ProgramColor = ProgramColor;
	Save.Turned = Turned;
	Save.MultiMove = MultiMove;
	Save.AutoPlay = AutoPlay;
	Save.SingleStep = SingleStep;
	Save.Level = Level;
	Save.AverageTime = AverageTime;
	Save.MaxLevel = MaxLevel;
	Save.MoveNo = MoveNo;
	Save.ChessClockTotalTime = ChessClock.totaltime;
	Save.BlackTotalTime = ChessTime[black].totaltime;
	Save.WhiteTotalTime = ChessTime[white].totaltime;
	memcpy( Save.PieceValue, PieceValue, 7 * sizeof(int) );

	fwrite( &Save, sizeof(SAVERESTORE), 1, gameboard );
	fwrite( Board, sizeof(Board), 1, gameboard );

	fclose(gameboard);
}


/****************************************************************************/

void RestoreGame( char *file_name )
{
    if( !strlen(file_name) )
        return;

    FILE *gameboard;
    SAVERESTORE Restore;

    if( (gameboard = fopen( file_name, "rb" )) == NULL ) {
        delete Restore;
        sprintf(buf, "Cannot open %s for reading", file_name);
        ::MessageBox( hWndMain, buf, "Chess", MB_OK | MB_ICONHAND );
        return;
    }

    fread( &Restore, sizeof(SAVERESTORE), 1, gameboard );
    fread( Board, sizeof(Board), 1, gameboard );

    fclose( gameboard );

    Player = Restore.Player;
    Opponent = Restore.Opponent;
    ProgramColor = Restore.ProgramColor;
    Turned = Restore.Turned;
    MultiMove = Restore.MultiMove;
    AutoPlay = Restore.AutoPlay;
    SingleStep = Restore.SingleStep;
    Level = Restore.Level;
    AverageTime = Restore.AverageTime;
    MaxLevel = Restore.MaxLevel;
    MoveNo = Restore.MoveNo;
    ChessClock.totaltime = Restore.ChessClockTotalTime;
    ChessTime[black].totaltime = Restore.BlackTotalTime ;
    ChessTime[white].totaltime = Restore.WhiteTotalTime;
    memcpy(PieceValue, Restore.PieceValue, 7 * sizeof(int));
}

