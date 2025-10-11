/*
 * gtest_board.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include <gtest/gtest.h>

#include "board.h"
#include "ply.h"

#include <algorithm>
#include <cctype>
#include <vector>

using namespace std;

std::string build_random_fen()
{
	//	<Piece Placement> ::= <rank8>'/'<rank7>'/'<rank6>'/'<rank5>'/'<rank4>'/'<rank3>'/'<rank2>'/'<rank1>
	//	<ranki>       ::= [<digit17>]<piece> {[<digit17>]<piece>} [<digit17>] | '8'
	//	<piece>       ::= <white Piece> | <black Piece>
	//	<digit17>     ::= '1' | '2' | '3' | '4' | '5' | '6' | '7'
	//	<white Piece> ::= 'P' | 'N' | 'B' | 'R' | 'Q' | 'K'
	//	<black Piece> ::= 'p' | 'n' | 'b' | 'r' | 'q' | 'k'

	std::string placement;

	for( int rank = 8; rank > 0; --rank ) {

		unsigned int files_remaining = 8;

		while( files_remaining ) {
			unsigned int blanks = rand() % ( files_remaining + 1 );

			if( blanks != 0 ) {
				placement += std::to_string( blanks );
				files_remaining -= blanks;
			}

			if( files_remaining ) {
				placement += ( std::string( "pnbrqkPNBRQK" ) )[rand() % 12]; // select a piece at random
				--files_remaining;
			}
		}

		placement += '/';
	}

	placement.pop_back();

	return placement;
}

// std::string resolve_blanks( std::string in )
// {
// 	std::string out;

// 	std::for_each( in.begin(), in.end(), [in, &out]( char ch ) {
// 		if( std::isdigit( ch ) ) {
// 			int count = ch - '0';
// 			out += std::string( count, ' ' );
// 		} else
// 			out.push_back( ch );
// 	} );

// 	return out;
// }

TEST( Clic, read_write_FEN )
{
	for( int test_cases = 0; test_cases < 10; ++test_cases ) {
		std::string expected = build_random_fen();
		Board board( expected );

		std::string actual = board.piece_placement();
		// std::string expected = resolve_blanks( placement );

		ASSERT_EQ( expected, actual );
	}
}

TEST( Clic, test_pawn_first_move )
{
	Board board( "8/8/8/8/8/8/4P3/8" );
	vector<Ply> moves = board.generate_legal_plys();

	ASSERT_EQ( 2, (int)moves.size() );

	vector<string> expected = { "e2-e4", "e2-e3" };

	for_each( expected.begin(), expected.end(), [moves]( string &ex_move ) {
		auto iter = find_if( moves.begin(), moves.end(),
							 [ex_move]( const Ply &gen_move ) { return gen_move.print_LAN() == ex_move; } );

		if( iter == moves.end() )
			ASSERT_EQ( ex_move, string( "(none)" ) );
	} );
}

TEST( Clic, test_pawn_capture )
{
	Board board( "8/8/8/8/8/3p1p2/4P3/8" );
	vector<Ply> moves = board.generate_legal_plys();

	ASSERT_EQ( 4, (int)moves.size() );

	vector<string> expected = { "e2-e4", "e2-e3", "e2xd3", "e2xf3" };

	for_each( expected.begin(), expected.end(), [moves]( string &ex_move ) {
		auto iter = find_if( moves.begin(), moves.end(),
							 [ex_move]( const Ply &gen_move ) { return gen_move.print_LAN() == ex_move; } );

		if( iter == moves.end() )
			ASSERT_EQ( ex_move, string( "(none)" ) );
	} );
}

TEST( Clic, test_pawn_promotion )
{
	Board board( "8/P7/8/8/8/8/8/8" );
	vector<Ply> moves = board.generate_legal_plys();

	ASSERT_EQ( 4, (int)moves.size() );

	vector<string> expected = { "a7-a8N", "a7-a8B", "a7-a8R", "a7-a8Q" };

	for_each( expected.begin(), expected.end(), [moves]( string &ex_move ) {
		auto iter = find_if( moves.begin(), moves.end(),
							 [ex_move]( const Ply &gen_move ) { return gen_move.print_LAN() == ex_move; } );

		if( iter == moves.end() )
			ASSERT_EQ( ex_move, string( "(none)" ) );
	} );
}

TEST( Clic, test_promo_match )
{
	Board board( "8/P7/8/8/8/8/8/8" );
	vector<Ply> moves = board.generate_legal_plys();

	ASSERT_EQ( 4, (int)moves.size() );

	vector<string> expected = { "a7-a8N", "a7-a8B", "a7-a8R", "a7-a8Q" };

	for( auto move : moves )
		ASSERT_FALSE( move.check_promo_match( Piece::none ) );
}