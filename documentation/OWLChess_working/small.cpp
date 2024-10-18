// ObjectWindows - (C) Copyright 1992 by Borland International

#include <owl\owlpch.h>
#pragma hdrstop

#include <time.h>
#include <stdio.h>
#include <string.h>
#include "wcdefs.h"
#include "wchess.h"
#include "externs.h"

/*
 *  Global Variables
 */
MOVESTRUCT ZeroMove = { 8, 8, 0, no_piece, no_piece };
MOVESTRUCT KeyMove;
bool Running;
ENUMCOLOR RunColor;
ENUMCOLOR   ProgramColor;
bool Analysis, Opan;
double WantedTime;
bool GameOver = false;
char EndGameMessage[80];


/*
 *  Initialize the chess clocks
 */
void InitChessTime()
{
    InitTime( &ChessTime[white] );
    InitTime( &ChessTime[black] );
}

void StopChessTime()
{
    if( Running ) {
        StopTime( &ChessTime[RunColor] );
        stop_timer();
        Running = false;
    }
}

/*
 *  Stop the running chess clock and start the clock for color
 */
void StartChessTime( ENUMCOLOR color )
{
    RunColor = color;
    Running = true;
    StartTime( &ChessTime[RunColor] );
    start_timer();
}

/*
 *  reset MovTab
 */
void ResetMoves()
{
    Depth = -1;
    MovTab[-1] = ZeroMove;
}


/*
 *  Test if the move is legal for Programcolor == player in the
 *  given position
 */
bool IllegalMove( MOVESTRUCT *amove )
{
    bool illegal;

    SetMove( amove );
    illegal = Attacks( Opponent, get_king_square(Player) );	// By doing this move will the king be attacked?
    ResetMove( amove );

    return illegal;
}

/*
 *  Make move for programcolor = player and updates variables
 */
void MakeMove( MOVESTRUCT *amove )
{
    Depth++;
    MoveNo++;
    SetMove( amove );
    ProgramColor = Opponent;
    Opponent = Player;
    Player = ProgramColor;
}

/*
 *  Prints comment to the game (check, mate, draw, resign)
 */
void PrintComment( void )
{
    extern char buf[];
    short check, possiblemove, checkmate;
    int nummoves;

    TInfo_SetMessageText( "" );
    checkmate = 0;
    Depth++;
    possiblemove = 0;
    InitMovGen();

    MOVESTRUCT test_move;
    do {
        test_move = MovGen();
        if( (test_move.movpiece != no_piece ) && ( !IllegalMove( &test_move ) )
			possiblemove = 1;

    } while( test_move.movpiece != no_piece && !possiblemove );

    Depth--;
    check = Attacks( Opponent, get_king_square(Player) ); /* calculate check */

    /*  No possible move means checkmate or stalemate  */
	if( !possiblemove && check ) {
		checkmate = 1;
		strcpy( EndGameMessage, "CheckMate" );
		GameOver = true;
	}

	if( !possiblemove && !check ) {
		strcpy( EndGameMessage, "Stalemate!" );
		GameOver = true;
	}

    if( possiblemove && ( HintEvalu >= MATEVALUE - DEPTHFACTOR * 16 ) ) {
        nummoves = ( MATEVALUE - HintEvalu + 0x40 ) / ( DEPTHFACTOR * 2 );
        char mate_message[40];
        sprintf( mate_message, "Mate in %d Move%c", nummoves, ( nummoves > 1 ) ? 's!':'!' );
        TInfo_SetMessageText( mate_message );
    }

    if( possiblemove && check ) {
        TInfo_SetMessageText( "Check!" );
        return;
    }

	if( FiftyMoveCnt() >= 100 ) {
		strcpy( EndGameMessage, "50 Move rule" );
		GameOver = true;
		return;
	}

	if( Repetition( 0 ) >= 3 ) {
		strcpy( EndGameMessage, "3 fold Repetition" );
		GameOver = true;
		return;
	}

	/*  Resign if the position is hopeless  */
	if( -25500 < HintEvalu && HintEvalu < -0x880 ) {
		strcpy( EndGameMessage, (Opponent == white) ? " White resigns" : " Black resigns" );
		GameOver = true;
	}
}

void EnterMove( MOVESTRUCT *amove )
{
    StopChessTime();
    TInfo_PrintMove( MoveNo, ProgramColor, amove, getTotalTime( &ChessTime[RunColor] ) );

    MakeMove( amove );

    UpdateBoard();
    PrintComment();
    StartChessTime( ProgramColor );
}

void RemoveMove( MOVESTRUCT *amove )
{
    StopChessTime();
    TInfo_PrintMove( MoveNo, ProgramColor, amove, getTotalTime( &ChessTime[RunColor] ) );

    TakeBackMove( amove );

    UpdateBoard();
    PrintComment();
    StartChessTime( ProgramColor );
}

/*
 *  Clear HintLine
 */
static void ClearHint()
{
    HintLine[0] = ZeroMove;
    HintEvalu = 0;
}

/*
 *  perform the move entered by the user
 */
void EnterKeyMove( void )
{
    MovTab[Depth+1] = KeyMove;
    PlayerMove = KeyMove;
    ClearHint();
    DragEnd( true );
    EnterMove( &MovTab[Depth+1] );
}

/*
 *  move movtab to depth = -1
 */
void AdjustMoves()
{
    int i;

    for( i = Depth; i>= BACK; i-- )
        MovTab[i - ( Depth+1 )] = MovTab[i];
    Depth = -1;
}

/*
 *  Move movtab one move BACK
 */
void StoreMoves( void )
{
    Depth--;

    for( int i = BACK; i <= Depth; i++ )
        MovTab[i] = MovTab[i+1];

    MovTab[BACK] = ZeroMove;
}

/*
 *  Check to see if the input move is legal
 */
bool MoveCheck( int startsq, int endsq )
{
    Depth++;
    KeyMove = ZeroMove;
    InitMovGen();

    MOVESTRUCT test_move;
    do {
        test_move = MovGen();
        if( test_move.new1 == endsq && test_move.old == startsq ) {
            KeyMove = test_move;
            break;
        }
    } while( test_move.movpiece != no_piece );

    if( KeyMove.movpiece == no_piece ) {
        Warning( "Impossible move" );
        Depth--;
        return false;
    }

    if( IllegalMove( &KeyMove ) ) {
        Warning( "Illegal move. Check!" );
        Depth--;
        return false;
    }

    Depth--;

    if( !ComputerThinking ) {
        AdjustMoves();
        EnterKeyMove();
        StoreMoves();
    }

    return true;
}

/*
 *  calculate the WANTED response time
 */
void StartAnalysis()
{
    int timecontrol;

    Analysis = 1;
    Opan = false;

    switch( Level ) {
    case easygame :
    case normal :
        /*  Divides the Time left till nest time control between moves left.  There is a margin of
            4 moves to make sure that the program does not lose on time  */
        timecontrol = ( ( ( MoveNo >> 1 ) + 20 ) / 20 ) * 20;
        if( timecontrol <= 40 )
            timecontrol = 40;

        WantedTime = ( AverageTime * timecontrol - getTotalTime( &ChessTime[ProgramColor] ) ) / ( timecontrol + 4 - ( MoveNo >> 1 ) );

        /*  In the beginning of the game the program thinks around twice as long, since the early middle
            game is normally the most crucial part of a game  */

        if( ( MoveNo >> 1 ) <= 40 )
            WantedTime = 5.0 + ( WantedTime - 5.0 ) * ( ( 80 - ( MoveNo >> 1 ) ) /40 );

        break;

    case fullgametime :
        /*  Assumes that the game will last for around 40 moves and divides the time left accordingly  */
        WantedTime = ( AverageTime * 60.0 - getTotalTime( &ChessTime[ProgramColor] ) ) / 44;

        /*  In the beginning of the game the program thinks around twice as long, since the early middle
            game is normally the most crucial part of a game  */
        if( ( MoveNo >> 1 ) <= 40 )
            WantedTime = 5.0 + ( WantedTime - 5.0 ) * ( ( 80 - ( MoveNo >> 1 ) ) /40 );

        break;

    case matching :
        /*  Spend as much time as the Opponent does  */
        if( MoveNo >= 2 )
            WantedTime = getTotalTime( &ChessTime[Opponent] ) / ( MoveNo >> 1 );
        else
            WantedTime = 5.0;

        WantedTime += ( getTotalTime( &ChessTime[Opponent] ) - getTotalTime( &ChessTime[ProgramColor] ) ) * 0.25;
        break;

    default :
        WantedTime = 1000000.0;
    }
}

/*
 *  take BACK move and update variables
 */
void TakeBackMove( MOVESTRUCT *amove )
{
    ProgramColor = Opponent;
    Opponent = Player;
    Player = ProgramColor;
    ResetMove( amove );
    MoveNo--;
    Depth--;
}

void Wait( int tenths )
{
    /* two ticks == approx. 1/10 second, since 18.2 clocks is approx a second */
    clock_t NumTicksToWait = ( tenths * 2 ) + clock();

    while( NumTicksToWait > clock() )
        ;
}

/*
 *  Flash a move once on the screen
 */
void FlashMove( MOVESTRUCT *amove )
{
    MakeMove( amove );
    UpdateBoard();

    Wait( 4 );

    TakeBackMove( amove );
    UpdateBoard();

    Wait( 4 );
}

void DoSlideMove( MOVESTRUCT &amove )
{
    int castsquare, cornersquare;

    SlidePiece( amove.new1, amove.old );

    if( amove.spe && ( amove.movpiece == king ) ) {
        GenCastSquare( amove.new1, &castsquare, &cornersquare );
        SlidePiece( castsquare, cornersquare );
    }
}

