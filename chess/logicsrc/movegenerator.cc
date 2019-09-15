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

#include "movegenerator.h"

MoveGenerator::MoveGenerator()
{
	//ctor
}

MoveGenerator::~MoveGenerator()
{
	//dtor
}

/*
 *  The move generator.
 *  InitMoves generates all possible moves and places them in a list.
 *
 *  On entry:
 *    Player contains the colour to move.
 *    MovTab[Depth-1] the last performed move.
 *
 *  On exit:
 *    move_list contains the generated moves.
 *
 *    The moves are generated in the order :
 *      Captures
 *      Castlings
 *      Non captures
 *      E.p. captures
 */
void MoveGenerator::InitMoves()
{
#if 0
	move_list.clear();

    /* generate all captures starting with captures of largest pieces */
    for( index = 1; index <= PawnNo[Opponent]; index++ )
        if( PieceTab[Opponent][index].ipiece != no_piece ) {
            Next.new1 = PieceTab[Opponent][index].isquare;
            CapMovGen();
        }
#endif
}

/*
 *  Generates captures of the piece on new1 using PieceTab

 struct STMove
{
    bool is_white;

    char castling;
    char piece_type;

    bool is_capture;
    bool pawn_promo;
    bool attack_king;
    bool check_mate;

    STSquare orig_square;      // first is file, second is rank
    STSquare new_square;      // first is file, second is rank
};



 */

#if 0
static void CapMovGen( void )
{
    int nextsq, sq;
    int i;

    STMove new_move;

    Next.spe = 0;
    Next.content = Board[Next.new1].piece;
    Next.movpiece = pawn;

    nextsq = Next.new1 - ( ( Player == white ) ? 0x10 : -0x10 );

    for( sq = nextsq-1; sq <= nextsq+1; sq+=2 )
        if( VALIDSQUARE( sq ) )
            if( Board[sq].piece == pawn && Board[sq].color == Player ) {
                Next.old = sq;
                if( Next.new1 < 8 || Next.new1 >= 0x70 )
                    PawnPromotionGen( Next.new1, Next.old, Next.content );
                else
                    GeneratedMoves[++GeneratedMoveCount] = Next;        /* Store the move in buffer */
            }

    /*  Other captures, starting with the smallest pieces  */
    for( i = OfficerNo[Player]; i >= 0; i-- )
        if( PieceTab[Player][i].ipiece != no_piece && PieceTab[Player][i].ipiece != pawn )
            if( PieceAttacks( PieceTab[Player][i].ipiece, Player, PieceTab[Player][i].isquare, Next.new1 ) ) {

                Next.old = PieceTab[Player][i].isquare;
                Next.movpiece = PieceTab[Player][i].ipiece;

                GeneratedMoves[++GeneratedMoveCount] = Next;    /* Store the move in buffer */
            }
}
#endif
