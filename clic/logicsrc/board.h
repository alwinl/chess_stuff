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

#include <array>
#include <string>
#include <vector>

#include "piece.h"

class Ply;

class Board
{
public:
	Board( std::string PiecePlacement = "" );

	void update_board( Ply a_ply );
	Board make( Ply a_ply ) const;

	std::vector<Ply> generate_legal_plys() const;

	int evaluate() const;

	int evaluate_ply( const Ply &ply, int depth_left ) const;

	std::string piece_placement() const;
	Piece get_piece( unsigned int index ) { return position[index]; }

private:
	std::array<Piece, 64> position;
	uint16_t ep_square;
	eColor side_to_move;

	std::vector<Ply> generate_plys() const;

	void process_placement( std::string PiecePlacement );

	int alpha_beta( int alpha, int beta, int depth_left ) const;
};

#endif // BOARD_H