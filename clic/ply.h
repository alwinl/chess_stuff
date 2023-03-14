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

#ifndef PLY_H
#define PLY_H

#include <cstdint>
#include <string>

#include "piece.h"

class Ply
{
public:
	bool operator==( const Ply rhs ) const { return ply == rhs.ply; };
	std::string print_LAN();

	static Ply standard_move( uint16_t current_square, uint16_t target_square, Piece::eType current_type, Piece::eType target_square_type, Piece::eType promo_type );
	static Ply ep_move( uint16_t current_square, uint16_t target_square );

public:
	union {
		uint32_t ply;
		struct {
			uint16_t from : 6;
			uint16_t to : 6;
			uint16_t type : 3;
			uint16_t promo_type : 3;
			uint16_t capture : 1;
			uint16_t castling : 1;
			uint16_t ep_candidate : 1;
			uint16_t en_passant : 1;
			uint16_t king_capture : 1;
			uint16_t check : 1;
			uint16_t checkmate : 1;
			uint16_t flags : 7;
		};
	};

};

#endif // PLY_H
