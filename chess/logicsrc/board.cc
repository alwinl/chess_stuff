/*
 * Copyright 2017 Alwin Leerling <alwin@jambo>
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

#include "board.h"
#include "fentranslator.h"
#include "pods.h"

Board::Board( std::string PiecePlacement )
{
	if( PiecePlacement.empty() )
		PiecePlacement = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";

	pieces = FENTranslator().from_FEN( PiecePlacement );
}

std::string Board::piece_placement() const
{
	return FENTranslator().to_FEN( pieces );
}

Board Board::add_piece( STSquare square, STPiece new_piece )
{
	if( square.file != -1 )
		pieces.insert( std::pair<STSquare, STPiece>( square, new_piece) );

	return *this;
}

Board Board::remove_piece( STSquare square )
{
	std::map<STSquare,STPiece>::iterator it = pieces.find( square );
	if( it != pieces.end() )
		pieces.erase( it );

	return *this;
}

Board Board::clear_all()
{
	pieces.clear();

    is_white_move = true;             // is it whites next move?

    white_can_castle_kingside =
    white_can_castle_queenside =
    black_can_castle_kingside =
    black_can_castle_queenside = true;

    //STSquare en_passant_target = make_square( -1, -1 );		//  If a pawn has just made a two-square move, this is the position "behind" the pawn

    halfmove_clock = 0;             // Number of half moves since last capture or pawn advance
    fullmove_number = 0;             // The number of the move, start at one increment after black move

	return *this;
}


Board Board::move_piece( STSquare old_square, STSquare new_square )
{
	std::map<STSquare,STPiece>::iterator it_from = pieces.find( old_square );
	std::map<STSquare,STPiece>::iterator it_to = pieces.find( new_square );

	if( (it_from == pieces.end()) || (it_to != pieces.end()) )
		return *this;		// cannot move, throw an exception

	// this really should be transactional maybe use swap
	pieces.insert( std::pair<STSquare, STPiece>( new_square, (*it_from).second ) );
	pieces.erase( it_from );

	return *this;
}

Board Board::capture_piece( STSquare old_square, STSquare new_square )
{
	std::map<STSquare,STPiece>::iterator it_from = pieces.find( old_square );
	std::map<STSquare,STPiece>::iterator it_to = pieces.find( new_square );

	if( (it_from == pieces.end()) || (it_to == pieces.end()) )
		return *this;		// cannot capture, throw an exception

	// this really should be transactional maybe use swap
	pieces.erase( it_to );
	pieces.insert( std::pair<STSquare, STPiece>( new_square, (*it_from).second ) );
	pieces.erase( it_from );

	return *this;
}

Board Board::set_white_move( bool on )
{
	is_white_move = on;

	return *this;
}

bool Board::is_valid()
{
    int KingCount[2]  = { 0, 0 };
    int TotalCount[2] = { 0, 0 };

    for( std::map<STSquare,STPiece>::iterator it = pieces.begin(); it != pieces.end(); it++ ) {

        int colour_idx = (*it).second.is_white ? 0 : 1;

        TotalCount[ colour_idx ]++;

        if( std::string("Kk").find( (*it).second.code ) != std::string::npos )
            KingCount[ colour_idx ]++;
    }

    return ( TotalCount[ 0 ] <= 16 ) && ( KingCount[ 0 ] == 1 )
                 && ( TotalCount[ 1 ] <= 16 ) && ( KingCount[ 1 ] == 1 );
}

