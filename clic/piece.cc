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

#include "piece.h"

#include <map>
#include <string>

using namespace std;

Piece::Piece( char code )
{
	static map<char,Piece::eType> code_to_type {
		{ 'p', Piece::pawn }, { 'P', Piece::pawn },
		{ 'n', Piece::knight }, { 'N', Piece::knight },
		{ 'b', Piece::bishop }, { 'B', Piece::bishop },
		{ 'r', Piece::rook }, { 'R', Piece::rook },
		{ 'q', Piece::queen }, { 'Q', Piece::queen },
		{ 'k', Piece::king }, { 'K', Piece::king }
	};

	hasmoved = false;
	color = ((string("KQRBNP").find( code ) != string::npos) ? white : black );
	type = code_to_type.at( code );
}

Piece Piece::make_promo_piece( Piece::eType new_type ) const
{
	Piece new_piece( *this );

	new_piece.type = new_type;

	return new_piece;
}

unsigned int Piece::ray_directions() const
{
	static unsigned int directions[] = { 0, 0, 8, 4, 4, 8, 8 };

	return directions[type];
}

unsigned int Piece::get_ray_offset( unsigned int ray ) const
{
	static int offset[7][8] = {
		{   0,   0,  0,  0, 0,  0,  0,  0 }, /* none */
		{   0,   0,  0,  0, 0,  0,  0,  0 }, /* pawn */
		{ -21, -19,-12, -8, 8, 12, 19, 21 }, /* knight */
		{ -11,  -9,  9, 11, 0,  0,  0,  0 }, /* bishop */
		{ -10,  -1,  1, 10, 0,  0,  0,  0 }, /* rook */
		{ -11, -10, -9, -1, 1,  9, 10, 11 }, /* queen */
		{ -11, -10, -9, -1, 1,  9, 10, 11 }  /* king */
	};

	return offset[type][ray];
}

char Piece::get_code() const
{
	return ( color == white ) ? (std::string(" PNBRQK"))[type] : (std::string(" pnbrqk"))[type];
}

unsigned int Piece::get_score( uint16_t square ) const
{
	if( color == white )
		square ^= 56;

	return material_value[type] + square_tables[type][square];
}


