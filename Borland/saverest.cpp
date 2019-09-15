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

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "wcdefs.h"
//#include "externs.h"


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

    if( ! strlen( file_name ) )
        return -1;

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

    CopyBoard( Board, Save->theBoard );

    fwrite( Save, sizeof( SAVERESTORE ), 1, gameboard );

    fclose( gameboard );
    delete Save;

    return 0;
}


int RestoreGame( char *file_name )
{
    if( !strlen( file_name ) )
        return -1;

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

