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

#ifndef BOARD_H
#define BOARD_H

#include <map>

#include "../ADTsrc/pods.h"

class Board
{
public:
	Board();

	Board standard_opening_board();

	Board add_piece( STSquare square, STPiece new_piece );
	Board remove_piece( STSquare square );
	Board clear_all();

	Board move_piece( STSquare old_square, STSquare new_square );
	Board capture_piece( STSquare old_square, STSquare new_square );

	Board set_white_move( bool on );

	bool is_valid();

	std::map<STSquare,STPiece> get_pieces() { return pieces; }

private:
	std::map<STSquare,STPiece> pieces;

    bool is_white_move;             // is it whites next move?

    bool white_can_castle_kingside;
    bool white_can_castle_queenside;
    bool black_can_castle_kingside;
    bool black_can_castle_queenside;

    STSquare en_passant_target;		//  If a pawn has just made a two-square move, this is the position "behind" the pawn

    int halfmove_clock;             // Number of half moves since last capture or pawn advance
    int fullmove_number;             // The number of the move, start at one increment after black move

};

#endif // BOARD_H
