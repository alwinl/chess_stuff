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

#include "testboard.h"

#include "board.h"
#include "ply.h"

#include <vector>

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TestBoard );

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
			unsigned int blanks = rand() % (files_remaining + 1);

			if( blanks != 0 ) {
				placement += std::to_string( blanks );
				files_remaining -= blanks;
			}

			if( files_remaining ) {
				placement += (std::string("pnbrqkPNBRQK"))[rand() % 12];	// select a piece at random
				--files_remaining;
			}
		}

		placement += '/';
	}

	placement.pop_back();

	return placement;
}

void TestBoard::read_write_FEN()
{
	for( int test_cases = 0; test_cases < 10; ++test_cases ) {
		std::string placement = build_random_fen();
		Board board( placement );

		CPPUNIT_ASSERT_EQUAL( placement, board.piece_placement() );
	}
}

void TestBoard::test_pawn_first_move()
{
	Board board("8/8/8/8/8/8/4P3/8");
	vector<Ply> moves = board.generate_legal_plys( white, (uint16_t)-1 );

	CPPUNIT_ASSERT_EQUAL( 2, (int)moves.size() );

	vector<Ply> expected = { {.from = 12, .to = 20, .type = Piece::pawn }, {.from = 12, .to = 28, .type = Piece::pawn, .ep_candidate = 1 } };

	for_each( expected.begin(), expected.end(),
		[moves](Ply& ex_move ) {
			CPPUNIT_ASSERT( find( moves.begin(), moves.end(), ex_move ) != moves.end() );
		}
	);
}

void TestBoard::test_pawn_capture()
{
	Board board("8/8/8/8/8/3p1p2/4P3/8");
	vector<Ply> moves = board.generate_legal_plys( white, (uint16_t)-1 );

	CPPUNIT_ASSERT_EQUAL( 4, (int)moves.size() );

	vector<Ply> expected = { {.from = 12, .to = 20, .type = Piece::pawn }, {.from = 12, .to = 28, .type = Piece::pawn, .ep_candidate = 1 },
							 {.from = 12, .to = 19, .type = Piece::pawn, .capture = 1 }, {.from = 12, .to = 21, .type = Piece::pawn, .capture = 1 } };

	for_each( expected.begin(), expected.end(),
		[moves](Ply& ex_move ) {
			CPPUNIT_ASSERT( find( moves.begin(), moves.end(), ex_move ) != moves.end() );
		}
	);
}
