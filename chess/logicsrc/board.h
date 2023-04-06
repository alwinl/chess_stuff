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


	std::array<Piece,64> get_pieces() { return position; }

private:
	std::array<Piece,64> position;

	void process_placement( std::string PiecePlacement );
};

#endif // BOARD_H
