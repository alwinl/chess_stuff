// OWLCVT 08/22/97 02:10:30
// ObjectWindows - (C) Copyright 1992 by Borland International

#include <owl\owlpch.h>
#pragma hdrstop

//#include <windows.h>
#include "wcdefs.h"
#include "externs.h"

/*
 *  globals
 */
int OfficerNo[2], PawnNo[2];


/*
 *  Clear the board and initialize the board-module
 */
void ClearBoard()
{
    SQUARETYPE Square;

    for( Square = 0; Square <= 0x77; Square++ ) {
        Board[Square].piece = no_piece;
        Board[Square].color = white;
    }
}


/*
 *  Clears indexes in board and piecetab
 */
static void ClearIndex( void )
{
    SQUARETYPE square;
    COLORTYPE col;
    INDEXTYPE index;

    for( square = 0; square <= 0x77; square++ )
        Board[square].index = 16;

    for( col = white; col <= black; ( ( int )col )++ )
        for( index = 0; index < 16; index++ )
            PieceTab[col][index].ipiece = no_piece;

    OfficerNo[white] = PawnNo[white] = -1;
    OfficerNo[black] = PawnNo[black] = -1;
}


/*
 *  Calculates Piece table from scratch
 */
void CalcPieceTab( void )
{
    SQUARETYPE square;
    PIECETYPE piece;

    ClearIndex();

    for( piece = king; piece <= pawn; ( ( int )piece )++ ) {
        if( piece == pawn ) {
            OfficerNo[white] = PawnNo[white];
            OfficerNo[black] = PawnNo[black];
        }
        square = 0;
        do {
            if( Board[square].piece == piece ) {

                PawnNo[Board[square].color]++;
                PieceTab[Board[square].color][PawnNo[Board[square].color]].ipiece = piece;
                PieceTab[Board[square].color][PawnNo[Board[square].color]].isquare = square;
                Board[square].index = PawnNo[Board[square].color];
            }

            square ^=  0x77;

            if( !( square & 4 ) ) {
                if( square >= 0x70 )
                    square = ( square + 0x11 ) & 0x73;
                else
                    square += 0x10;
            }

        } while( square );
    }
}


/*
 *  move a piece to a new location on the board
 */
static void MovePiece( SQUARETYPE new_square, SQUARETYPE old_square )
{
    // move a piece to a new location on the board
    BOARDTYPE b = Board[new_square];
    Board[new_square] = Board[old_square];
    Board[old_square] = b;

    PieceTab[Board[new_square].color][Board[new_square].index].isquare = new_square;
}


/*
 *  Calculate the squares for the rook move in castling
 */
void GenCastSquare( SQUARETYPE new1, SQUARETYPE *castsquare, SQUARETYPE *cornersquare )
{
    if( ( new1 & 7 ) >= 4 ) {      /* short castle */
        *castsquare = new1 - 1;
        *cornersquare = new1 + 1;
    } else {                           /* long castle */
        *castsquare = new1 + 1;
        *cornersquare = new1 - 2;
    }
}


/*
 *  Take back captures
 */
static void InsertPTabPiece( PIECETYPE inpiece, COLORTYPE incolor, SQUARETYPE insquare )
{
    Board[insquare].piece = inpiece;
    Board[insquare].color = incolor;

    PieceTab[incolor][Board[insquare].index].ipiece = inpiece;
    PieceTab[incolor][Board[insquare].index].isquare = insquare;
}


/*
 *  Used for pawn promotion
 */
static void ChangeType( PIECETYPE newtype, SQUARETYPE insquare )
{
    Board[insquare].piece = newtype;
    PieceTab[Board[insquare].color][Board[insquare].index].ipiece = newtype;

    if( OfficerNo[Board[insquare].color] < Board[insquare].index )
        OfficerNo[Board[insquare].color] = Board[insquare].index;
}

static void perform_forward_move( MOVETYPE *move )
{

    if( !move->spe ) {
        if( move->content != no_piece ) {
            // Delete piece
            Board[move->new1].piece = no_piece;
            PieceTab[Board[move->new1].color][Board[move->new1].index].ipiece = no_piece;
        }

        MovePiece( move->new1, move->old );
        return;
    }

    // specials
    // Castling
    if( move->movpiece == king ) {
        SQUARETYPE castsquare, cornersquare;
        GenCastSquare( move->new1, &castsquare, &cornersquare );
        MovePiece( castsquare, cornersquare );
        return;
    }

    // Capture by pawn
    if( move->movpiece == pawn ) {
        // Delete piece
        SQUARETYPE epsquare = ( move->new1 & 7 ) + ( move->old & 0x70 ); /* E.p. capture */
        Board[epsquare].piece = no_piece;
        PieceTab[Board[epsquare].color][Board[epsquare].index].ipiece = no_piece;
        return;
    }

    // Pawn promotion
    ChangeType( move->movpiece, move->new1 );
}

static void perform_backward_move( MOVETYPE *move )
{
    if( !move->spe ) {
        MovePiece( move->old, move->new1 );
        if( move->content != no_piece )
            InsertPTabPiece( move->content, Opponent, move->new1 );
        return;
    }

    // specials
    // Castling
    if( move->movpiece == king ) {
        SQUARETYPE castsquare, cornersquare;
        GenCastSquare( move->new1, &castsquare, &cornersquare );
        MovePiece( cornersquare, castsquare );
        return;
    }

    // Capture by pawn
    if( move->movpiece == pawn ) {
        SQUARETYPE epsquare = ( move->new1 & 7 ) + ( move->old & 0x70 ); /* E.p. capture */
        InsertPTabPiece( pawn, Opponent, epsquare );
        return;
    }

    // Pawn promotion
    ChangeType( pawn, move->old );
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

void Perform( MOVETYPE *move, BOOL resetmove )
{
    if( resetmove )
        perform_backward_move( move );
    else
        perform_forward_move( move );
}


/*
 * Compare two moves
 */

BOOL EqMove( MOVETYPE *a, MOVETYPE *b )
{
    if( a->movpiece == b->movpiece )
        if( a->new1 == b->new1 )
            if( a->old == b->old )
                if( a->content == b->content )
                    if( a->spe == b->spe )
                        return TRUE;
    return FALSE;
}


