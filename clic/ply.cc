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

#include "ply.h"
#include "piece.h"

using namespace std;

std::string Ply::print_LAN( )
{
	if( castling )
		return (( to > from ) ? "O - O" : "O - O - O");

	string result;

	if( type != Piece::pawn )
		result += string("E NBRQK")[type];

	result += (char)('a' + (from % 8));
	result += (char)('1' + (from / 8) );
	result += (capture ? 'x' : '-' );
	result += (char)('a' + (to % 8));
	result += (char)('1' + (to / 8) );

	if( promotion )
		result += string("E NBRQK")[promo_type];

	if( check )
		result += '+'; // "†";

	if( checkmate)
		result += '#'; // "‡";

	return result;
}

