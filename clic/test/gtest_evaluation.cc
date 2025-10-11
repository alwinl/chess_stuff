/*
 * gtest_evaluation.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include <algorithm>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include "board.h"
#include "piece.h"
#include "ply.h"

using namespace std;

uint16_t parse_square( std::string SAN )
{
    return ( SAN[1] - '1' ) * 8 + ( SAN[0] - 'a' );
};


TEST( Clic, test_square_parsing )
{
	unsigned int expected_square = 0;

	for( unsigned char rank = '1'; rank <= '8'; ++rank ) {
		for( unsigned char file = 'a'; file <= 'h'; ++file ) {
			string SAN;
			SAN.push_back( file );
			SAN.push_back( rank );

			unsigned int square = parse_square( SAN );
			ASSERT_EQ( expected_square, square ) << "Calculated square does not match parsed square";
			++expected_square;
		}
	}
}

#define REVERSE_RANK_MASK 0b00111000

TEST( Clic, piece_scores_depend_on_color )
{
	for( unsigned int i = 0; i < 6; ++i ) {

		Piece white_piece( string( "PNBRQK" )[i] );
		Piece black_piece( string( "pnbrqk" )[i] );

		for( unsigned char rank = '1'; rank <= '8'; ++rank ) {
			for( unsigned char file = 'a'; file <= 'h'; ++file ) {
				string SAN;
				SAN.push_back( file );
				SAN.push_back( rank );

				unsigned int square = parse_square( SAN );

				ASSERT_EQ( white_piece.get_score( square ),
									  black_piece.get_score( square ^ REVERSE_RANK_MASK ) );
			};
		};
	}
}

template <typename T> std::ostream &operator<<( std::ostream &os, const std::vector<T> t )
{
	os << "[";

	for( size_t i = 0; i < t.size(); ++i ) {
		os << t[i];
		os << ( i == t.size() - 1 ? "" : "," );
	};

	return os << "]";
}

TEST( Clic, square_table_values )
{
	enum { piece_code, square, value };

	vector<tuple<char, string, unsigned int>> test_set = {
		// clang-format off
		{ 'P', "e2", 80 },
		{ 'p', "e2", 150 },
		{ 'K', "e1", 20000 },
		{ 'K', "g1", 20030 },
		{ 'K', "c1", 20010 },
		{ 'q', "h4", 895 },
		{ 'Q', "h4", 895 },
		{ 'q', "a4", 895 },
		{ 'Q', "a4", 900 },
		// clang-format on
	};

	vector<int> expected;
	vector<int> actual;

	for( auto entry : test_set ) {
		expected.push_back( get<value>( entry ) );
		actual.push_back( Piece( get<piece_code>( entry ) ).get_score( parse_square( get<square>( entry ) ) ) );
	}

	stringstream expected_string;
	stringstream actual_string;

	expected_string << expected;
	actual_string << actual;

	ASSERT_EQ( expected_string.str(), actual_string.str() );
}

// there are only 3 asymmetric lines: queen table ranks 2, 3 and 4 (black 7, 6 and 5)
TEST( Clic, test_queen_asymmetry )
{
	vector<string> ranks = {
		// clang-format off
		"[890,900,905,900,900,900,900,890]",
		"[890,905,905,905,905,905,900,890]",
		"[900,900,905,905,905,905,900,895]"
		// clang-format on
	};

	for( unsigned int index = 0; index < 3; ++index ) {

		vector<int> white_result;
		vector<int> black_result;

		for( unsigned char file = 'a'; file <= 'h'; ++file ) {
			string SAN;
			SAN.push_back( file );
			SAN.push_back( '2' + index );

			white_result.push_back( Piece( 'Q' ).get_score( parse_square( SAN ) ) );

			SAN.pop_back();
			SAN.push_back( '7' - index );

			black_result.push_back( Piece( 'q' ).get_score( parse_square( SAN ) ) );
		}

		stringstream ss;
		ss << white_result;

		ASSERT_EQ( ranks[index], ss.str() ) << "White";

		ss.str( "" );
		ss << black_result;

		ASSERT_EQ( ranks[index], ss.str() ) << "Black";
	}
}

TEST( Clic, standard_board_eval_is_zero )
{
	Board board;

	ASSERT_EQ( board.evaluate(), 0 );
}

TEST( Clic, check_all_first_moves )
{
	Board board;

	vector<Ply> plys = board.generate_legal_plys();

	sort( plys.begin(), plys.end(), [&board]( const Ply &lhs, const Ply &rhs ) {
		return board.evaluate_ply( lhs, 0 ) > board.evaluate_ply( rhs, 0 );
	} );

	// for_each( plys.begin(), plys.end(),
	// 	[&board](Ply& ply)
	// 	{
	// 		cout << ply.print_LAN() << ": score " << board.evaluate_ply( ply, 0, white ) << endl;
	// 	}
	// );
}

TEST( Clic, test_alpha_beta )
{
	Board board;

	vector<Ply> plys = board.generate_legal_plys();

	sort( plys.begin(), plys.end(), [&board]( const Ply &lhs, const Ply &rhs ) {
		return board.evaluate_ply( lhs, 3 ) > board.evaluate_ply( rhs, 3 );
	} );

	// cout << endl;
	// for( Ply& ply: plys ) {
	// 	cout << ply.print_LAN() << ": "
	// 		<< board.evaluate_ply( ply, 0, black ) << ", "
	// 		<< board.evaluate_ply( ply, 1, black ) << ", "
	// 		<< board.evaluate_ply( ply, 2, black ) << ", "
	// 		<< board.evaluate_ply( ply, 3, black ) << endl;
	// }
	// cout << endl;
}

TEST( Clic, queen_should_not_capture_rook )
{
	Board board( "4k3/8/6p1/5r2/8/3Q4/8/4K3" );

	vector<Ply> plys = board.generate_legal_plys();

	sort( plys.begin(), plys.end(), [&board]( const Ply &lhs, const Ply &rhs ) {
		return board.evaluate_ply( lhs, 3 ) > board.evaluate_ply( rhs, 3 );
	} );

	// cout << endl;
	// for( Ply& ply: plys ) {
	// 	cout << ply.print_LAN() << ": "
	// 		<< board.evaluate_ply( ply, 1, black ) << ", "
	// 		<< board.evaluate_ply( ply, 2, black ) << ", "
	// 		<< board.evaluate_ply( ply, 3, black ) << endl;
	// }
	// cout << endl;
}