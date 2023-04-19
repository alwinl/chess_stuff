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

#include <stdexcept>
#include <algorithm>

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
	//		uint16_t en_passant : 1;
	this->king_capture = ( target_square_type == Piece::king );
	//		uint16_t check : 1;
	//		uint16_t checkmate : 1;

	if( flags != 0 )
		throw( domain_error( "Ply with nonzero reserved member") );
}

Ply::Ply( EnPassant input )
{
	this->ply = 0;

	this->from  = input.current_square;
	this->to  = input.target_square;
	this->type = Piece::pawn;

	this->promo_type = Piece::none;
	this->capture  = true;
	this->castling  = false;
	this->en_passant = true;
	this->king_capture = false;
	//		uint16_t check : 1;
	//		uint16_t checkmate : 1;
}

uint16_t Ply::get_ep_square( ) const
{
	if( (type != Piece::pawn) || (std::abs( from - to) != 16 ) )
		return (uint16_t)-1;

	return to + ( (from < to) ? -8 : 8 );
}

bool Ply::check_match( uint16_t from_square, uint16_t to_square, char promo_piece )
{
	return (from_square == from) && (to_square == to) && ( (promo_piece == ' ') || (promo_type == Piece(promo_piece).get_type() ) );
}


bool Ply::check_san_match( Ply rhs ) const
{
	return (to == rhs.to) && (type == rhs.type) && (promo_type == rhs.promo_type) && (capture == rhs.capture);
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

std::string Ply::print_SAN( std::vector<Ply>& legal_plys ) const
{
	if( castling )
		return (( to > from ) ? "O-O" : "O-O-O");

	string result;

	auto piece_with_same_target = [&](Ply& ply){ return (type == ply.type) && (to == ply.to) && (promo_type == ply.promo_type); };
	auto same_file_of_departure = [&](Ply& ply) { return (type == ply.type) && (to == ply.to) && (promo_type == ply.promo_type) && ( (from % 8)  == (ply.from % 8)  ); };
	auto same_rank_of_departure = [&](Ply& ply) { return (type == ply.type) && (to == ply.to) && (promo_type == ply.promo_type) && ( (from / 8)  == (ply.from / 8)  ); };

	if( type != Piece::pawn ) {
		result += string("E NBRQK")[type];

		// insert either file or rank (or both) if there is more than one piece that can make this move
		if( count_if( legal_plys.begin(), legal_plys.end(), piece_with_same_target ) != 1 ) {

			if( count_if( legal_plys.begin(), legal_plys.end(), same_file_of_departure ) == 1 )			// file inclusion resolves conflict
				result += (char)('a' + (from % 8));
			else if( count_if( legal_plys.begin(), legal_plys.end(), same_rank_of_departure ) == 1 )	// rank inclusion resolves conflict
				result += (char)('1' + (from / 8) );
			else {																						// need both file and rank to resolve conflict
				result += (char)('a' + (from % 8));
				result += (char)('1' + (from / 8) );
			}
		}
	}

	if( capture ) {
		if( type == Piece::pawn )
			result += (char)('a' + (from % 8));
		result += 'x';
	}

	result += (char)('a' + (to % 8));
	result += (char)('1' + (to / 8) );

	if( promo_type != Piece::none )
		result += "=" + string("E NBRQK")[promo_type];

	if( check )
		result += '+'; // "†";

	if( checkmate)
		result += '#'; // "‡";

	return result;
}


