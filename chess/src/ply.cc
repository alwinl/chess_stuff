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

#include <stdexcept>
#include <algorithm>

using namespace std;

Ply::Builder Ply::create( Piece &piece, uint16_t current_square, uint16_t target_square )
{
	return Builder( piece, current_square, target_square );
}

Ply::Ply( uint16_t current_square, uint16_t target_square, Piece::eType current_type, Piece::eType capture_type,
			Piece::eType promo_type, eColor color, bool ep_move )
{
	this->ply = 0;

	this->color = (color == eColor::black);
	this->from = current_square;
	this->to = target_square;
	this->type = current_type;

	this->promo_type = promo_type;
	this->capture = (capture_type != Piece::none) || ep_move;
	this->castling = ((current_type == Piece::king) && (std::abs( current_square - target_square ) == 2 ));
	this->ep_candidate = ((current_type == Piece::pawn) && (std::abs( current_square - target_square ) == 16 ) );
	this->en_passant = ep_move;
	this->king_capture = ( capture_type == Piece::king );
	//		uint16_t check : 1;
	//		uint16_t checkmate : 1;

	if( reserved != 0 )
		throw( domain_error( "Ply with nonzero reserved member") );
}

std::string Ply::print_LAN() const
{
	if( castling )
		return (( to > from ) ? "O-O" : "O-O-O");

	string result;

	if( type != Piece::pawn )
		result += string("E NBRQK")[type];

	// this bit is unique for lan
	result += (char)( 'a' + (from % 8) );
	result += (char)( '1' + (from / 8) );

	result += (capture ? 'x' : '-' );
	// end of lan unique

	result += (char)( 'a' + ( to % 8 ) );
	result += (char)( '1' + ( to / 8 ) );

	if( promo_type != Piece::none )
		result += string("E NBRQK")[promo_type];

	if( check )
		result += '+'; // "†";

	if( checkmate )
		result += '#'; // "‡";

	return result;
}

std::string Ply::print_SAN( vector<Ply>& legal_plys ) const
{
	if( castling )
		return (( to > from ) ? "O-O" : "O-O-O");

	string result;

	auto piece_with_same_target = [&](Ply& ply){ return (type == ply.type) && (to == ply.to) && (promo_type == ply.promo_type); };
	auto same_file_of_departure = [&](Ply& ply) { return (type == ply.type) && (to == ply.to) && (promo_type == ply.promo_type) && ( (from % 8)  == (ply.from % 8)  ); };
	auto same_rank_of_departure = [&](Ply& ply) { return (type == ply.type) && (to == ply.to) && (promo_type == ply.promo_type) && ( (from / 8)  == (ply.from / 8)  ); };

	if( type != Piece::pawn )
		result += string("E NBRQK")[type];

	// this bit is unique to san
	if( type != Piece::pawn ) {

		// insert either file or rank (or both) if there is more than one piece that can make this move
		if( std::count_if( legal_plys.begin(), legal_plys.end(), piece_with_same_target ) != 1 ) {

			if( std::count_if( legal_plys.begin(), legal_plys.end(), same_file_of_departure ) == 1 )			// file inclusion resolves conflict
				result += (char)('a' + (from % 8));
			else if( std::count_if( legal_plys.begin(), legal_plys.end(), same_rank_of_departure ) == 1 )	// rank inclusion resolves conflict
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
	// end of san unique

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