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

#ifndef PIECE_H
#define PIECE_H

#include <cstdint>

enum eColor { white, black };

extern int *square_tables[];

class Piece
{
public:
	enum eType { none, pawn, knight, bishop, rook, queen, king };

	static constexpr unsigned int material_value[] = { 0, 100, 320, 330, 500, 900, 20000 }; //  Tomasz Michniewski

	Piece( eType _type = none, eColor _color = white );
	Piece( char code );

	bool is_color( eColor test_color ) const { return color == test_color; }
	bool is_sliding() const { return ( type == bishop ) || ( type == rook ) || ( type == queen ); }
	bool is_of_type( eType test_type ) const { return type == test_type; }
	bool has_moved() const { return hasmoved; }
	eType get_type() const { return eType( type ); }
	char get_code() const;

	unsigned int get_score( uint16_t square ) const;

	Piece make_promo_piece( Piece::eType new_type ) const;
	void moved() { hasmoved = true; }

	bool operator<( const Piece rhs ) const { return piece < rhs.piece; }
	bool operator!=( const Piece rhs ) const { return piece != rhs.piece; }

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
};

#endif // PIECE_H