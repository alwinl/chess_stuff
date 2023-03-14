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
	Ply( uint16_t current_square, uint16_t target_square, Piece::eType current_type = Piece::none, Piece::eType target_square_type = Piece::none, Piece::eType promo_type = Piece::none );
	bool operator==( const Ply rhs ) const { return ply == rhs.ply; };
	std::string print_LAN() const;

	static Ply ep_move( uint16_t current_square, uint16_t target_square );

	uint16_t square_from() const { return from; };
	uint16_t square_to() const { return to; };
	bool is_ep_candidate() const { return ep_candidate == 1; };
	bool is_ep_capture() const { return en_passant == 1; };
	bool is_castling() const { return castling == 1; }
	bool is_kingcapture() const { return king_capture == 1; }

	uint16_t get_castling_rook_square_from( ) const { return from + ( ( to > from ) ? +3 : -4 ); }; // King / Queen side
	uint16_t get_castling_rook_square_to( )   const { return to + (  ( to > from ) ? -1 : +1 ); };
	uint16_t get_ep_square( bool is_white )   const { return to + ( is_white ? -8 : 8 ); }
	Piece::eType get_promo_type() const { return Piece::eType( promo_type ); }

	bool check_square_match( Ply rhs ) const { return (from == rhs.from) && (to == rhs.to); }
	bool check_promo_match( Ply rhs ) const { return (promo_type == rhs.promo_type); }
	bool check_promo_match( Piece::eType rhs_promo_type ) const { return (promo_type == rhs_promo_type); }

private:
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
