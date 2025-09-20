/*
 * gtest_parserengine.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include "pgn_parser_engine.h"

class ParserVisitor : public ParserVisitorBase
{
public:
	virtual void process_tag_pair( std::string tag, std::string value ) {
		std::cout << "Tag: " << tag << ", value: " << value << "\n";
	}
	virtual void process_movetext( std::string white_move, std::string black_move ) {
		std::cout << "White: " << white_move << ", Black: " << black_move << "\n";
	}

};

TEST( TestChessParserEngine, a_test )
{
	PGNParserEngine engine;
	ParserVisitor visitor;

	engine.open_file( "./game1.pgn" );

	engine.visit_tag_pairs( &visitor );
	engine.visit_movetext( &visitor );

	ASSERT_TRUE( true );
}
