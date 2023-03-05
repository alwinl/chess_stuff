/*
 * Copyright 2023 Alwin Leerling <dna.leerling@gmail.com>
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
 */

#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <array>

#include "piece.h"

class Move;
class Display;

class Board
{
public:
	Board();

	void print_board( Display& display );
	void update_board( Move the_move );
	std::vector<Move> generate_moves( eColor side, uint16_t ep_square );

	bool illegal_move( Move& amove );

	Piece::eType get_type_from_square( unsigned int square ) const { return position[square].get_type(); }

private:
	std::array<Piece, 64> position;
};

#endif // BOARD_H
