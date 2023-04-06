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

#include <string>
#include <array>
#include <vector>

#include "piece.h"
#include "ply.h"

class Board
{
public:
	Board( std::string PiecePlacement = "" );
	std::string piece_placement() const;

	Board add_piece( uint16_t square, char code );
	Board remove_piece( uint16_t square );
	bool is_valid();

	void update_board( Ply a_ply );

	std::vector<Ply> generate_legal_plys( eColor side, uint16_t ep_square ) const;

	int evaluate() const;
	Board make( Ply a_ply ) const;

	int evaluate_ply( const Ply& ply, int depth_left, eColor color ) const;

	std::array<Piece,64> get_pieces() { return position; }

private:
	std::array<Piece,64> position;

	void process_placement( std::string PiecePlacement );

	bool illegal_move( Ply& a_ply ) const;
	std::vector<Ply> generate_plys( eColor side, uint16_t ep_square ) const;
	int alpha_beta( int alpha, int beta, int depth_left, eColor color ) const;
};

#endif // BOARD_H
