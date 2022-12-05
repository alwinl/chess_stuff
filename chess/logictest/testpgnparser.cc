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

#include <sstream>

#include "../ADTsrc/pgnparser.h"
#include "../ADTsrc/pods.h"

#include "pgntoken.h"

#include <iostream>

CPPUNIT_TEST_SUITE_REGISTRATION( TestPNGParser );

TestPNGParser::TestPNGParser()
{
}

TestPNGParser::~TestPNGParser()
{
}


void TestPNGParser::create_test()
{
    std::string test_data = "[key \"value\"]";
    std::istringstream stm(test_data);

    //ChessGame game = PNGParser().do_parse(stm);

    CPPUNIT_ASSERT( true );
}

void TestPNGParser::tokenise()
{
	std::string input_data( "[Event \"Moscow City Championship\"]\n[Site \"New York City, NY USA\"]\n\n1. e4 e5\n\n" ); //1. e4 e5\n\n

	std::stringstream is( input_data );

	std::vector<PGNToken> tokens = PGNParser().Tokenise( is );

	std::cout << std::endl;
	std::cout << "------------------------" << std::endl;
	std::for_each( tokens.begin(), tokens.end(), [](PGNToken& token) { std::cout << token; });
	std::cout << "------------------------" << std::endl;

	CPPUNIT_ASSERT_EQUAL( 15, static_cast<int>(tokens.size()) );

}

