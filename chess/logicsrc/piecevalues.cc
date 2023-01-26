/*
 * Copyright 2014 Alwin Leerling <alwin@jambo>
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

#include "piecevalues.h"

#include <utility>

using namespace std;

PieceValues::PieceValues( )
{
	values.insert( pair<ePieceIdx,int>( QUEEN,  2304 ) );		// 9 << 8
	values.insert( pair<ePieceIdx,int>( ROOK,  1280 ) );		// 5 << 8
	values.insert( pair<ePieceIdx,int>( BISHOP,   768 ) );		// 3 << 8
	values.insert( pair<ePieceIdx,int>( KNIGHT,   768 ) );		// 3 << 8
	values.insert( pair<ePieceIdx,int>( PAWN,   256 ) );		// 1 << 8
	values.insert( pair<ePieceIdx,int>( KING,   0 ) );			// 0 << 8
}

bool PieceValues::choose_piece_values( )
{
	setup( );

	bool first = manipulate( );
	if( first )
		result( );

	return first;
}


int PieceValues::get_piece_value( char index ) const
{
	PieceValues::ePieceIdx idx;
	PieceValues::ePieceColour colour;

	// ugly ugly ugly, but this code needs to be chopped anyway
	switch( index ) {
	case 'Q' : idx = QUEEN; colour = WHITE; break;
	case 'R' : idx = ROOK; colour = WHITE; break;
	case 'B' : idx = BISHOP; colour = WHITE; break;
	case 'N' : idx = KNIGHT; colour = WHITE; break;
	case 'P' : idx = PAWN; colour = WHITE; break;
	case 'K' : idx = KING; colour = WHITE; break;
	case 'q' : idx = QUEEN; colour = BLACK; break;
	case 'r' : idx = ROOK; colour = BLACK; break;
	case 'b' : idx = BISHOP; colour = BLACK; break;
	case 'n' : idx = KNIGHT; colour = BLACK; break;
	case 'p' : idx = PAWN; colour = BLACK; break;
	case 'k' : idx = KING; colour = BLACK; break;
	}

	return get_piece_value( idx, colour );
}

int PieceValues::get_piece_value( PieceValues::ePieceIdx index, PieceValues::ePieceColour colour ) const
{
	std::map<ePieceIdx,int>::const_iterator it = values.find(index);

	if( it == values.end() )
		return 0;

	return ( colour == BLACK ) ? -it->second : it->second;
}


