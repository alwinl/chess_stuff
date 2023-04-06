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

using namespace std;

Board::Board( std::string PiecePlacement )
{
	if( PiecePlacement.empty() )
		PiecePlacement = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";

	process_placement( PiecePlacement );
}

void Board::process_placement( std::string PiecePlacement )
{
	unsigned int rank = 7;
	unsigned int file = 0;

	position.fill( Piece( Piece::none ) );

    for( char& code : PiecePlacement ) {

        if( (code > '0') && (code < '9') ) {       // Character can either be a number (squares to skip)
           file += (code - '0' );
        } else if( code == '/' ) {                      // ... or an end of rank indicator
            --rank;
            file = 0;
        } else {								  // or an piece designator
        	position[rank * 8 + file] = Piece( code );
            ++file;
        }
    }
}

std::string Board::piece_placement() const
{
	string placement;

    for( int rank=7; rank >= 0; --rank ) {

        int blanks = 0;

        for( int file=0; file < 8; ++file ) {

			if( ! position[rank * 8 + file].is_of_type( Piece::none ) ) {
                if( blanks ) {
					placement += to_string( blanks );
                    blanks = 0;
                }
                placement += position[rank * 8 + file].get_code();
			} else
                ++blanks;
        }

        if( blanks )
			placement += to_string( blanks );

        if( rank )
            placement += "/";
    }

    return placement;
}

Board Board::add_piece( uint16_t square, char code )
{
	position[ square ] = Piece( code );
	return *this;
}

Board Board::remove_piece( uint16_t square )
{
	position[ square ] = Piece::none;
	return *this;
}

void Board::update_board( Ply a_ply )
{
	uint16_t to = a_ply.square_to();
	uint16_t from = a_ply.square_from();

	if( a_ply.is_ep_capture() ) {

		uint16_t ep_square = a_ply.get_ep_square( position[from].is_color( white ) );

		position[to] = position[from];
		position[from] = Piece( Piece::none );

		position[ ep_square ] = Piece( Piece::none );

	} else if( ! a_ply.check_promo_match( Piece::none ) ) {

		position[to] = position[from].make_promo_piece( a_ply.get_promo_type() );
		position[from] = Piece( Piece::none );

	} else if( a_ply.is_castling() ) {

		// move the king
		position[to] = position[from];
		position[from] = Piece( Piece::none );

		position[to].moved();

		// reuse for rook
		to = a_ply.get_castling_rook_square_to();
		from = a_ply.get_castling_rook_square_from();

		// move the rook
		position[to] = position[from];
		position[from] = Piece( Piece::none );

		position[to].moved();

	} else {
		position[to] = position[from];
		position[from] = Piece( Piece::none );

		position[to].moved();
	}
}

bool Board::is_valid()
{
    int KingCount[2]  = { 0, 0 };
    int TotalCount[2] = { 0, 0 };

    for( auto it = position.begin(); it != position.end(); it++ ) {

        int colour_idx = (*it).is_color( white ) ? 0 : 1;

        TotalCount[ colour_idx ]++;

        if( (*it).is_of_type( Piece::king ) )
            KingCount[ colour_idx ]++;
    }

    return ( TotalCount[ 0 ] <= 16 ) && ( KingCount[ 0 ] == 1 )
                 && ( TotalCount[ 1 ] <= 16 ) && ( KingCount[ 1 ] == 1 );
}
