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
    bool 		Turned;
    bool 		MultiMove;
    bool 		AutoPlay;
    bool 		SingleStep;
    ENUMLEVEL 	Level;
    double 		AverageTime;
    char 		MaxLevel;
    int 		MoveNo;
    double 		ChessClockTotalTime;
    double 		BlackTotalTime;
    double 		WhiteTotalTime;
    int 		ValueOfPieces[7];
    BOARDTYPE   theBoard[0x78];
};


int SaveGame( char *file_name )
{
    FILE        *gameboard;
    SAVERESTORE *Save;

    if( !strlen( file_name ) > 0 )
        return;

    Save = new SAVERESTORE;
    if( Save == NULL ) {
        return -1;
    }

    if( ( gameboard = fopen( file_name, "wb" ) ) == NULL ) {
        delete Save;
        return -2;
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
    Save->ChessClockTotalTime = getTotalTime( &ChessClock );
    Save->BlackTotalTime = getTotalTime( &ChessTime[black] );
    Save->WhiteTotalTime = getTotalTime( &ChessTime[white] );
    getPieceValues( ValueOfPieces );

    CopyBoard( Board, save->theBoard );

    fwrite( Save, sizeof( SAVERESTORE ), 1, gameboard );

    fclose( gameboard );
    delete Save;

    return 0;
}


int RestoreGame( char *file_name )
{
    if( !strlen( file_name ) )
        return;

    FILE *gameboard;

    if( ( gameboard = fopen( file_name, "rb" ) ) == NULL )
        return -1;

    SAVERESTORE Restore;
    fread( &Restore, sizeof( SAVERESTORE ), 1, gameboard );

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
    setTotalTime( &ChessClock, Restore.ChessClockTotalTime );
    setTotalTime( &ChessTime[black], Restore.BlackTotalTime );
    setTotalTime( &ChessTime[white], Restore.WhiteTotalTime );
    setPieceValues( ValueOfPieces );
    CopyBoard( Restore.theBoard, Board );

    return 0;
}

