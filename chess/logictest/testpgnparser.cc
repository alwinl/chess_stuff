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

#include "testpgnparser.h"

#include "pgnparser.h"
#include "pgntoken.h"

#include <iostream>

CPPUNIT_TEST_SUITE_REGISTRATION( TestPNGParser );

/*
class GameADTTest : public PGNParserCollector
{
public:
	GameADTTest() {};
	virtual ~GameADTTest() {};

	virtual void add_tag_pair( std::string tag, std::string value )
	{
		std::cout << "Tag: " << tag << ", Value: " << value << std::endl;
	}
	virtual void add_move_text( unsigned int moveno, std::string white_move, std::string black_move )
	{
		std::cout << "MoveNo: " << moveno << ", white: '" << white_move <<  "', black: '" << black_move << "'" << std::endl;
	}

	virtual void debug_token_list( std::vector<PGNToken> tokens )
	{
		if( ! debugging )
			return;

		std::cout << std::endl;
		std::cout << "------------------------" << std::endl;
		std::for_each( tokens.begin(), tokens.end(), [](PGNToken& token) { std::cout << token; });
		std::cout << "------------------------" << std::endl;
	}

	bool debugging = false;
};
*/

void TestPNGParser::tokenise()
{
	std::string input_data( "[Event \"Moscow City Championship\"]\n[Site \"New York City, NY USA\"]\n\n1. e4 e5\n\n" );

	std::stringstream is( input_data );
	//GameADTTest collector;

	//collector.debugging = true;

	std::cout << std::endl;
	PGNParser().load( is );
}

void TestPNGParser::parse_it()
{
	std::string input_data( "[Event \"Moscow City Championship\"]\n[Site \"New York City, NY USA\"]\n\n1. e4 e5 2. a3 h6\n\n" );

	std::stringstream is( input_data );
	//GameADTTest collector;

	std::cout << std::endl;
	PGNParser().load( is );
}

void TestPNGParser::ProcessGameFile()
{
	std::string input_data( "\n\n1... e5 2. O-O Nh6x 3... O-O-O\n\n" );

	std::stringstream is( input_data );
	//GameADTTest collector;

	std::cout << std::endl;
	PGNParser().load( is );

}

void TestPNGParser::test_game_data()
{
	std::ifstream is("game.pgn");
	//GameADTTest collector;

	std::cout << std::endl;
    PGNParser().load( is );
}
