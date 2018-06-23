// OWLCVT 08/22/97 02:10:30
// ObjectWindows - (C) Copyright 1992 by Borland International

#include <owl\owlpch.h>
#pragma hdrstop

#include "wcdefs.h"
#include "externs.h"

/*
 *  globals
 */

int OfficerNo[2];
int PawnNo[2];


/*
 *  Clear the board and initialize the board-module
 */

void ClearBoard()
{
	int Square;

	for( Square = 0; Square <= 0x77; Square++ ) {
		Board[ Square ].piece = empty;
		Board[ Square ].color = white;
	}
}


/*
 *  Calcualates Piece table from scratch
 */

void CalcPieceTab(void)
{
    ENUMPIECE piece1;
	ENUMCOLOR color;
	int       index;
    int       square;

	// Clear indexes
    for( square = 0; square <= 0x77; square++ )
        Board[ square ].index = 16;

    for( index = 0; index < 16; index++ )
        PieceTab[ white ][ index ].ipiece = empty;

    for( index = 0; index < 16; index++ )
        PieceTab[ black ][ index ].ipiece = empty;

    OfficerNo[ white ] = PawnNo[ white ] = -1;
    OfficerNo[ black ] = PawnNo[ black ] = -1;

	// Start filling the table
    for( piece1 = king; piece1 <= pawn; ((int)piece1)++ ) {

        if( piece1 == pawn ) {
            OfficerNo[ white ] = PawnNo[ white ];
            OfficerNo[ black ] = PawnNo[ black ];
        }

        square = 0;
        do {
            if( Board[ square ].piece == piece1 ) {

				color = Board[ square ].color;
                index = ( PawnNo[ color ]++ );

                PieceTab[ color ][ index ].ipiece  = piece1;
                PieceTab[ color ][ index ].isquare = square;

                Board[ square ].index = index;

            }

            square ^=  0x77;

            if( !(square & 4) ) {
                if( square >= 0x70 )
                    square = (square + 0x11) & 0x73;
                else
                    square += 0x10;
            }
        } while( square );
    }
}


/*
 *  move a piece to a new location on the board
 */

inline void MovePiece( int newSquare, int oldSquare )
{
    BOARDTYPE SwapBoard;

    SaveBoard          = Board[ newSquare ];
    Board[ newSquare ] = Board[ oldSquare ];
    Board[ oldSquare ] = SaveBoard;

    PieceTab[ Board[ newSquare ].color ][ Board[ newSquare ].index ].isquare = newSquare;
}


/*
 *  Calculate the squares for the rook move in castling
 */

void GenCastSquare( int newSquare, int *castsquare, int *cornersquare )
{
    if( (newSquare & 7) >= 4 ) {         /* short castle */
        *castsquare   = newSquare - 1;
        *cornersquare = newSquare + 1;
    } else {                             /* long castle */
        *castsquare   = newSquare + 1;
        *cornersquare = newSquare - 2;
    }
}


/*
 *  This function used in captures.  insquare must not be empty.
 */

inline void DeletePiece(int insquare)
{
    Board[ insquare ].piece = empty;

    PieceTab[ Board[ insquare ].color ][ Board[ insquare ].index ].ipiece = empty;
}


/*
 *  Take back captures
 */

inline void InsertPTabPiece( ENUMPIECE inpiece, ENUMCOLOR incolor, int insquare )
{
    Board[ insquare ].piece                                = 
	PieceTab[incolor][Board[insquare].index].ipiece        = inpiece;

    Board[ insquare ].color                                = incolor;
    PieceTab[ incolor ][ Board[ insquare ].index ].isquare = insquare;
}


/*
 *  Used for pawn promotion
 */

inline void ChangeType( ENUMPIECE newtype, int insquare )
{
    Board[ insquare ].piece                                               = 
	PieceTab[ Board[ insquare ].color ][ Board[ insquare ].index ].ipiece = newtype;

    if( OfficerNo[ Board[ insquare ].color ] < Board[ insquare ].index )
        OfficerNo[ Board[ insquare ].color ] = Board[ insquare ].index;
}



/*
 *  Perform or take back move (takes back if resetmove is true),
 *  and perform the updating of Board and PieceTab.  Player must
 *  contain the color of the moving player, Opponent the color of the
 *  Opponent.
 *
 *  MovePiece, DeletePiece, InsertPTabPiece and ChangeType are used to update
 *  the Board module.
 */

void Perform( MOVESTRUCT *move, BOOL resetmove )
{
    int castsquare;
	int cornersquare;
	int epsquare;

    if( resetmove ) {
        MovePiece( move->old, move->new1 );
        if( move->content != empty )
            InsertPTabPiece( move->content, Opponent, move->new1 );
    } else {
        if( move->content != empty )
            DeletePiece( move->new1 );

        MovePiece( move->new1, move->old );
    }

    if( move->spe ) {
        if( move->movpiece == king ) {
            GenCastSquare( move->new1, &castsquare, &cornersquare );
            if( resetmove ) {
                MovePiece( cornersquare, castsquare );
            } else {
                MovePiece( castsquare, cornersquare );
            }
        } else {
            if( move->movpiece == pawn ) {
                epsquare = ( move->new1 & 7 ) + ( move->old & 0x70 ); /* E.p. capture */
                if( resetmove )
                    InsertPTabPiece( pawn, Opponent, epsquare );
                else
                    DeletePiece( epsquare );
            } else {
                if( resetmove )
                    ChangeType( pawn, move->old );
                else
                    ChangeType( move->movpiece,move->new1 );
            }
        }
    }
}


/*
 * Compare two moves
 */

BOOL EqMove(MOVESTRUCT *a, MOVESTRUCT *b)
{
    if( a->movpiece == b->movpiece)
        if( a->new1 == b->new1)
            if( a->old == b->old)
                if( a->content == b->content)
                    if( a->spe == b->spe)
                        return TRUE;
    return FALSE;
}


