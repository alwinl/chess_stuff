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

extern int* square_tables[];

//std::array<unsigned int,7> Piece::material_value = { 0, 100, 320, 330, 500, 900, 20000 };		//  Tomasz Michniewski
std::array<unsigned int,7> Piece::material_value = { 0, 256, 768, 768, 1280, 2304, 0 };		// Borland


void Piece::set_value( eType type, unsigned int new_value )
{
	if( type != eType::none )
		material_value[type] = new_value;
}

unsigned int Piece::get_value( eType type )
	{ return material_value[type]; }


Piece::Piece( eType type, eColor color  )
{
	this->piece = 0;

	this->color = (color == eColor::white) ? 0 : 1;
	this->type = type;
}

Piece::Piece( char code )
{
	static map<char, pair<Piece::eType,int> > code_to_type {
		{ 'p', {Piece::pawn, 1} }, { 'P', {Piece::pawn, 0} },
		{ 'n', {Piece::knight, 1} }, { 'N', {Piece::knight, 0} },
		{ 'b', {Piece::bishop, 1} }, { 'B', {Piece::bishop, 0} },
		{ 'r', {Piece::rook, 1} }, { 'R', {Piece::rook, 0} },
		{ 'q', {Piece::queen, 1} }, { 'Q', {Piece::queen, 0} },
		{ 'k', {Piece::king, 1} }, { 'K', {Piece::king, 0} }
	};

	piece = 0;

	color = (code_to_type.at( code )).second;
	type = (code_to_type.at( code )).first;
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
	return color ? (std::string(" pnbrqk"))[type] : (std::string(" PNBRQK"))[type];
}

unsigned int Piece::get_score( uint16_t square ) const
{
	if( color )
		square ^= 56;

	return material_value[type] + square_tables[type][square];
}


