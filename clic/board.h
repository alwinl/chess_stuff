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

class Ply;
class Display;

class Board
{
public:
	Board( std::string PiecePlacement = "" );

	void print_board( Display& display ) const;
	void update_board( Ply a_ply );

	std::string piece_placement() const;
	Piece::eType get_type_from_square( unsigned int square ) const { return position[square].get_type(); }

	std::vector<Ply> generate_legal_plys( eColor side, uint16_t ep_square ) const;

	int evaluate();
	Board make( Ply a_ply );

	int evaluate_ply( Ply& ply, int depth_left, eColor color );

private:
	std::array<Piece, 64> position;

	bool illegal_move( Ply& a_ply ) const;
	std::vector<Ply> generate_plys( eColor side, uint16_t ep_square ) const;

	void process_placement( std::string PiecePlacement );

	int alpha_beta( int alpha, int beta, int depth_left, eColor color );

};

#endif // BOARD_H
