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
	std::string print_LAN() const;

	uint16_t square_from() const { return from; };
	uint16_t square_to() const { return to; };
	bool is_ep_candidate() const { return ep_candidate == 1; };
	bool is_ep_capture() const { return en_passant == 1; };
	bool is_castling() const { return castling == 1; }
	bool is_kingcapture() const { return king_capture == 1; }

	uint16_t get_castling_rook_square_from() const { return from + ( ( to > from ) ? +3 : -4 ); }; // King / Queen side
	uint16_t get_castling_rook_square_to() const { return to + ( ( to > from ) ? -1 : +1 ); };
	uint16_t get_ep_square() const
	{
		return ( ep_candidate == 1 ) ? to + ( ( color == white ) ? -8 : 8 ) : (uint16_t)-1;
	}
	Piece::eType get_promo_type() const { return Piece::eType( promo_type ); }

	bool check_square_match( uint16_t rhs_from, uint16_t rhs_to ) const
	{
		return ( from == rhs_from ) && ( to == rhs_to );
	}
	bool check_promo_match( Piece::eType rhs_promo_type ) const { return ( promo_type == rhs_promo_type ); }

public:
	class Builder;
	static Builder create( Piece &piece, uint16_t current_square, uint16_t target_square );

private:
	union {
		uint32_t ply;
		struct {
			uint16_t color : 1;
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
			uint16_t reserved : 5;
		};
	};

	Ply( uint16_t current_square, uint16_t target_square, Piece::eType current_type, Piece::eType capture_type,
		 Piece::eType promo_type, eColor color, bool ep_move );
};

class Ply::Builder
{
public:
	Builder( Piece &piece, uint16_t current_square, uint16_t target_square )
		: current_square( current_square ), target_square( target_square ), current_type( piece.get_type() ),
		  current_color( piece.is_color( white ) ? white : black )
	{
	}

	Builder &setCaptureType( Piece::eType type )
	{
		target_type = type;
		return *this;
	}
	Builder &setPromoType( Piece::eType type )
	{
		promo_type = type;
		return *this;
	}
	Builder &setEPMove()
	{
		ep_move = true;
		return *this;
	}

	Ply build() const
	{
		return Ply( current_square, target_square, current_type, target_type, promo_type, current_color, ep_move );
	}

private:
	uint16_t current_square;
	uint16_t target_square;
	Piece::eType current_type;
	Piece::eType target_type = Piece::none;
	Piece::eType promo_type = Piece::none;
	eColor current_color;
	bool ep_move = false;
};

#endif // PLY_H