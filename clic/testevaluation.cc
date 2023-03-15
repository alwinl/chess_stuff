/*
 * Copyright 2022 Alwin Leerling <dna.leerling@gmail.com>
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

#include "testevaluation.h"

#include <set>
#include <tuple>
#include <iomanip>

#include "piece.h"
#include "board.h"
#include "ply.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TestEvaluation );

void TestEvaluation::test_square_parsing()
{
//	cout << endl;
//
//	for( unsigned char rank = '8'; rank >= '1'; --rank ) {
//		for( unsigned char file = 'a'; file <= 'h'; ++file ) {
//			string SAN;
//			SAN.push_back( file );
//			SAN.push_back( rank );
//			cout << SAN << '=' << setw(2) << Board::parse_square( SAN ) << ' ';
//		}
//		cout << endl;
//	}
}

void TestEvaluation::piece_scores_depend_on_color()
{
	for( unsigned int i=0; i<6; ++i ) {

		Piece white_piece( string("PNBRQK")[i] );
		Piece black_piece( string("pnbrqk")[i] );

		unsigned int expected_square = 0;

		for( unsigned char rank = '1'; rank <= 'h'; ++rank ) {
			for( unsigned char file = 'a'; file <= 'h'; ++file ) {
				string SAN;
				SAN.push_back( file );
				SAN.push_back( rank );

				unsigned int square = Board::parse_square( SAN );
				CPPUNIT_ASSERT_EQUAL_MESSAGE( "Calculated square does not match parsed square", expected_square, square );
				++expected_square;

				CPPUNIT_ASSERT_EQUAL( white_piece.get_score( square ), black_piece.get_score( square ^ 56 ) );
			};
		};
	}
}

void TestEvaluation::square_table_values()
{
	enum { piece_code, square, value };

	set<tuple<char, string, unsigned int>> test_set = {
		{ 'P', "e2", 80 },
		{ 'p', "e2", 150 },
		{ 'K', "e1", 20000 },
		{ 'K', "g1", 20030 },
		{ 'K', "c1", 20010 },
		{ 'q', "h4", 895 },
		{ 'Q', "h4", 895 },
		{ 'q', "a4", 895 },
		{ 'Q', "a4", 900 },
	};

	for( auto entry : test_set )
		CPPUNIT_ASSERT_EQUAL( get<value>(entry), Piece( get<piece_code>(entry) ).get_score( Board::parse_square( get<square>(entry) )));
}

template<typename T>
std::ostream& operator<<( std::ostream& os, const std::vector<T> t )
{
    os << "[";
    for( std::size_t i = 0; i < t.size(); ++i )
        os << t[i] << (i == t.size() - 1 ? "" : ",");

    return os << "]";
}

// there are only 3 asymmetric lines: queen table ranks 2, 3 and 4 (black 7, 6 and 5)
void TestEvaluation::test_queen_asymmetry()
{
	vector<string> ranks = {
		"[890,900,905,900,900,900,900,890]",
		"[890,905,905,905,905,905,900,890]",
		"[900,900,905,905,905,905,900,895]"
	};

	for( unsigned int index=0; index < 3; ++index ) {

		vector<int> white_result;
		vector<int> black_result;

		for( unsigned char file = 'a'; file <= 'h'; ++file ) {
			string SAN;
			SAN.push_back( file );
			SAN.push_back( '2' + index );

			white_result.push_back( Piece('Q').get_score( Board::parse_square( SAN ) ) );

			SAN.pop_back();
			SAN.push_back( '7' - index );

			black_result.push_back( Piece('q').get_score( Board::parse_square( SAN ) ) );
		}

		stringstream ss;
		ss << white_result;

		CPPUNIT_ASSERT_EQUAL_MESSAGE( "White", ranks[index], ss.str() );

		ss.str( "" );
		ss << black_result;

		CPPUNIT_ASSERT_EQUAL_MESSAGE( "Black", ranks[index], ss.str() );
	}
}

void TestEvaluation::standard_board_eval_is_zero()
{
	Board board;

	CPPUNIT_ASSERT_EQUAL( board.evaluate(white), 0);
	CPPUNIT_ASSERT_EQUAL( board.evaluate(black), 0);
}

void TestEvaluation::check_all_first_moves()
{
	Board board;
	eColor color = white;

	vector<Ply> plys = board.generate_legal_plys( color, (uint16_t)-1 );

	sort( plys.begin(), plys.end(), [&board, color](const Ply& lhs, const Ply& rhs){ return board.make( lhs ).evaluate( color ) > board.make( rhs ).evaluate( color ); } );

	for_each( plys.begin(), plys.end(), [&board, color](Ply& ply) { cout << ply.print_LAN() << ": score " << board.make( ply ).evaluate( color ) << endl; } );
}

void TestEvaluation::test_alpha_beta()
{
	Board board;
	eColor color = white;

	vector<Ply> plys = board.generate_legal_plys( color, (uint16_t)-1 );

	sort( plys.begin(), plys.end(), [&board, color](const Ply& lhs, const Ply& rhs){ return board.make( lhs ).evaluate( color ) > board.make( rhs ).evaluate( color ); } );

	cout << endl;
	for( Ply& ply: plys ) {
		cout << ply.print_LAN() << ": " << board.search_ply( ply, 1, black ) << ", "  << board.search_ply( ply, 2, black ) << ", "  << board.search_ply( ply, 3, black ) << endl;
	}
	cout << endl;
}
