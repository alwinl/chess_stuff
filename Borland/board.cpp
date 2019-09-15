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

#include "wcdefs.h"
//#include "externs.h"

/*
 *  globals
 */
int OfficerNo[2];
int PawnNo[2];

static void set_piecetab_piece( ENUMCOLOR color, short index, ENUMPIECE piece )
{
    PieceTab[color][index].ipiece = piece;
}

static void set_piecetab_square( ENUMCOLOR color, short index, int square )
{
    PieceTab[color][index].isquare = square;
}

int get_king_square( ENUMCOLOR color )
{
	return PieceTab[color][king].isquare;
}
/*
 *  Clear the board and initialize the board-module
 */
void ClearBoard( void )
{
    for( int Square = 0; Square <= 0x77; Square++ ) {
        Board[Square].piece = no_piece;
        Board[Square].color = white;
    }
}

bool ValidBoard( void )
{
    int KingCount[2]  = { 0, 0 };
    int TotalCount[2] = { 0, 0 };

    for( int square = 0; square < 0x78; square++ ) {

        if( VALIDSQUARE( square ) && ( Board[ square ].piece != no_piece ) ) {

            TotalCount[ Board[ square ].color ]++;

            if( Board[ square ].piece == king )
                KingCount[ Board[ square ].color ]++;
        }
    }

    return ( ( TotalCount[ white ] <= 16 ) && ( KingCount[ white ] == 1 ) && ( TotalCount[ black ] <= 16 ) && ( KingCount[ black ] == 1 ) );
}


void CopyBoard( BOARDTYPE * src, BOARDTYPE * dst )
{
    memcpy( dst, src, sizeof( BOARDTYPE ) * 0x78 );
}

void InsertPiece( ENUMPIECE piece, ENUMCOLOR color, int square )
{
    Board[ square ].piece = piece;
    Board[ square ].color = color;
}

/*
 *  Calculates Piece table from scratch
 */
void CalcPieceTab( void )
{
    /* Clear indexes in board and the piece table */
    for( int square = 0; square <= 0x77; square++ )
        Board[square].index = 16;

    for( int index = 0; index < 16; index++ ) {
        set_piecetab_piece( white, index, no_piece );
        set_piecetab_square( white, index, 0 );
        set_piecetab_piece( black, index, no_piece );
        set_piecetab_square( black, index, 0 );
    }

    PawnNo[white] = -1;
    PawnNo[black] = -1;

    // Start filling the table
    for( ENUMPIECE piece1 = king; piece1 <= pawn; ( ( int )piece1 )++ ) {

        for( int square = 0; square <= 0x77; square++ ) {
            if( Board[square].piece == piece1 ) {

                Board[square].index = ( PawnNo[ Board[square].color ]++ );

                set_piecetab_piece( Board[square].color, Board[square].index, Board[square].piece );
                set_piecetab_square( Board[square].color, Board[square].index, square );
            }
        }

    }

    OfficerNo[white] = PawnNo[white];
    OfficerNo[black] = PawnNo[black];
}

/*
 *  Calculate the squares for the rook move in castling
 */
void GenCastSquare( int newSquare, int *castsquare, int *cornersquare )
{
    bool short_castle = ( ( newSquare & 7 ) >= 4 );

    *castsquare   = short_castle ? newSquare - 1 : newSquare + 1;
    *cornersquare = short_castle ? newSquare + 1 : newSquare - 2;
}

/*
 *  Perform or take back move (takes back if resetmove is true),
 *  and perform the updating of Board and piece table.  Player must
 *  contain the color of the moving player, Opponent the color of the
 *  Opponent.
 *
 *  MovePiece, DeletePiece, InsertPTabPiece and ChangeType are used to update
 *  the Board module.
 */
void SetMove( MOVESTRUCT *amove )
{
    int castsquare, cornersquare, epsquare;
    BOARDTYPE   tmp;

    if( amove->content != no_piece ) { /* a piece is captured, remove it from the board */
        Board[amove->new1].piece = no_piece;
        set_piecetab_piece( Board[amove->new1].color, Board[amove->new1].index, no_piece );
    }

    tmp = Board[amove->new1];
    Board[amove->new1] = Board[amove->old];
    Board[amove->old] = tmp;

    set_piecetab_square( Board[amove->new1].color, Board[amove->new1].index, amove->new1 );

    if( amove->spe ) {
        if( amove->movpiece == king ) {

            GenCastSquare( amove->new1, &castsquare, &cornersquare );

            tmp = Board[castsquare];
            Board[castsquare] = Board[cornersquare];
            Board[cornersquare] = tmp;

            set_piecetab_square( Board[castsquare].color, Board[castsquare].index, castsquare );

        } else if( amove->movpiece == pawn ) {

            epsquare = ( amove->new1 & 7 ) + ( amove->old & 0x70 ); /* E.p. capture */

            Board[epsquare].piece = no_piece;
            set_piecetab_piece( Board[epsquare].color, Board[epsquare].index, no_piece );

        } else {

            Board[amove->new1].piece = amove->movpiece;
            set_piecetab_piece( Board[amove->new1].color, Board[amove->new1].index, amove->movpiece )

            if( amove->movpiece != no_piece && OfficerNo[Board[amove->new1].color] < Board[amove->new1].index )
                OfficerNo[Board[amove->new1].color] = Board[amove->new1].index;

        }
    }
}

void ResetMove( MOVESTRUCT *amove )
{
    int castsquare, cornersquare, epsquare;
    BOARDTYPE   tmp;

    tmp = Board[amove->old];
    Board[amove->old] = Board[amove->new1];
    Board[amove->new1] = tmp;

    set_piecetab_square( Board[amove->old].color, Board[amove->old].index, amove->old );

    if( amove->content != no_piece ) {      /* there is a captured piece that needs to be restored */

        Board[amove->new1].piece = amove->content;
        Board[amove->new1].color = Opponent;

        set_piecetab_piece( Opponent, Board[amove->new1].index, amove->content );
        set_piecetab_square( Opponent, Board[amove->new1].index, amove->new1 );
    }

    if( amove->spe ) {
        if( amove->movpiece == king ) {

            GenCastSquare( amove->new1, &castsquare, &cornersquare );

            tmp = Board[cornersquare];
            Board[cornersquare] = Board[castsquare];
            Board[castsquare] = tmp;

            set_piecetab_square( Board[cornersquare].color, Board[cornersquare].index, cornersquare );

        } else if( amove->movpiece == pawn ) {

            epsquare = ( amove->new1 & 7 ) + ( amove->old & 0x70 ); /* E.p. capture */

            Board[epsquare].piece = pawn;
            Board[epsquare].color = Opponent;

            set_piecetab_piece( Opponent, Board[epsquare].index, pawn );
            set_piecetab_square( Opponent, Board[epsquare].index, epsquare );

        } else {

            Board[amove->old].piece = amove->movpiece;

            set_piecetab_piece( Board[amove->old].color, Board[amove->old].index, amove->movpiece )

            if( amove->movpiece != no_piece && OfficerNo[Board[amove->old].color] < Board[amove->old].index )
                OfficerNo[Board[amove->old].color] = Board[amove->old].index;

        }
    }
}


/*
 * Compare two moves
 */
bool EqMove( MOVESTRUCT *a, MOVESTRUCT *b )
{
    if( a->movpiece == b->movpiece )
        if( a->new1 == b->new1 )
            if( a->old == b->old )
                if( a->content == b->content )
                    if( a->spe == b->spe )
                        return true;
    return false;
}


