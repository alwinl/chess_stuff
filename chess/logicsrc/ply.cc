/*
 * Copyright 2019 Alwin Leerling <dna.leerling@gmail.com>
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

#include "ply.h"
#include "piece.h"

using namespace std;


Ply::Ply( uint16_t current_square, uint16_t target_square, Piece::eType current_type, Piece::eType target_square_type, Piece::eType promo_type )
{
	this->ply = 0;

	this->from  = current_square;
	this->to  = target_square;
	this->type = current_type;

	this->promo_type = promo_type;
	this->capture  = ( target_square_type != Piece::none );
	this->castling  = ((current_type == Piece::king) && (std::abs( current_square - target_square) == 2 ));
	this->ep_candidate =  ((current_type == Piece::pawn) && (std::abs( current_square - target_square) == 16 ));
	//		uint16_t en_passant : 1;
	this->king_capture = ( target_square_type == Piece::king );
	//		uint16_t check : 1;
	//		uint16_t checkmate : 1;
}

//Ply::Ply( LAN input )
//{
//	this->ply = 0;
//
//	std::string lan = input.str();
//
//}
//
//Ply::Ply( SAN input )
//{
//	this->ply = 0;
//}


Ply Ply::ep_move( uint16_t current_square, uint16_t target_square )
{
	Ply new_ply( current_square, target_square, Piece::pawn );

	new_ply.capture  = true;
	new_ply.en_passant = true;

	return new_ply;
}


std::string Ply::print_LAN( ) const
{
	if( castling )
		return (( to > from ) ? "O-O" : "O-O-O");

	string result;

	if( type != Piece::pawn )
		result += string("E NBRQK")[type];

	result += (char)('a' + (from % 8));
	result += (char)('1' + (from / 8) );
	result += (capture ? 'x' : '-' );
	result += (char)('a' + (to % 8));
	result += (char)('1' + (to / 8) );

	if( promo_type != Piece::none )
		result += string("E NBRQK")[promo_type];

	if( check )
		result += '+'; // "†";

	if( checkmate)
		result += '#'; // "‡";

	return result;
}

