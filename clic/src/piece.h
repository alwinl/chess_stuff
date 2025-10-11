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

#pragma once

#include <cstdint>

enum class eColor { white, black };

eColor operator!( eColor old_color );

class Piece
{
public:
	enum eType { none, pawn, knight, bishop, rook, queen, king };

	static void set_value( eType type, unsigned int new_value );
	static unsigned int get_value( eType type );

	Piece( eType type = eType::none, eColor color = eColor::white );
	Piece( char code );

	bool is_color( eColor test_color ) const { if(test_color == eColor::white) return (color == 0); else return (color == 1); }
	bool is_sliding() const { return (type == eType::bishop) || (type == eType::rook) || (type == eType::queen); }
	bool is_of_type( eType test_type ) const { return type == test_type; }
	bool has_moved() const { return hasmoved; }

	eColor get_color() const { return (color == 0) ? eColor::white : eColor::black; }
	eType get_type() const { return eType( type ); }
	char get_code() const;

	unsigned int get_score( uint16_t square ) const;
	Piece make_promo_piece( Piece::eType new_type ) const;
	void moved() { hasmoved = true; }

	bool operator<( const Piece rhs ) const { return piece < rhs.piece; }
	bool operator!=( const Piece rhs ) const { return piece != rhs.piece; }

	unsigned int ray_directions() const;
	unsigned int get_ray_offset( unsigned int ray ) const;

protected:
	union {
		uint8_t piece;
		struct {
			uint8_t hasmoved : 1;
			uint8_t color : 1;
			uint8_t type : 3;
			uint8_t reserved : 3;
		};
	};

	void set_color( eColor colour ) { color = (colour == eColor::white) ? 0 : 1; }
};
