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

#include <string>
#include <vector>
#include <array>

#include "piece.h"

class Move;
class Display;

class Board
{
public:
	Board( std::string PiecePlacement = "" );

	void print_board( Display& display ) const;
	void update_board( Move the_move );

	std::string piece_placement() const;
	Piece::eType get_type_from_square( unsigned int square ) const { return position[square].get_type(); }

	std::vector<Move> generate_legal_moves( eColor side, uint16_t ep_square ) const;

private:
	std::array<Piece, 64> position;

	bool illegal_move( Move& amove ) const;
	std::vector<Move> generate_moves( eColor side, uint16_t ep_square ) const;

	void process_placement( std::string PiecePlacement );
};

#endif // BOARD_H
