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

enum eColor { white, black};

class Piece
{
public:
	enum eType { none, pawn, knight, bishop, rook, queen, king };

	Piece( eType _type = none, eColor _color = white  ) { hasmoved = false; color = _color; type = _type; }
	Piece( char code );

	bool is_color( eColor test_color ) const { return color == test_color; }
	bool is_sliding() const {	return (type == bishop) || (type == rook) || (type == queen); }
	bool is_of_type( eType test_type ) const { return type == test_type; }
	bool has_moved() const { return hasmoved; }
	eType get_type() const { return eType( type ); }
	char get_code() const;

	unsigned int ray_directions() const;
	unsigned int get_ray_offset( unsigned int ray ) const;

	void promote_pawn( eType new_type ) { type = new_type; }
	void moved() { hasmoved = true; }

	bool operator<( const Piece rhs ) const { return piece < rhs.piece; }
	bool operator!=( const Piece rhs ) const { return piece != rhs.piece; }

protected:
	union {
		uint16_t piece;
		struct {
			uint16_t hasmoved:1;
			uint16_t color:1;
			uint16_t type:3;
			uint16_t reserved:11;
		};
	};
};

#endif // PIECE_H
