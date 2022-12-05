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

#include "testpgntoken.h"

//#include <sstream>
#include <iostream>

#include "../ADTsrc/pgntoken.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TestPNGToken );

TestPNGToken::TestPNGToken()
{
}

TestPNGToken::~TestPNGToken()
{
}

void TestPNGToken::check_EOFToken()
{
	CPPUNIT_ASSERT( PGNToken::EOFToken().type() == PGNToken::FILEEND );
}

void TestPNGToken::check_EOSToken()
{
	CPPUNIT_ASSERT( PGNToken::EOSToken().type() == PGNToken::SECTIONEND );
}

void TestPNGToken::check_self_terminators()
{
	std::string input_data( "[].*()<>\n" );

	std::vector<PGNToken> tokens;
	PGNToken token;

	for( char& ch : input_data ) {

		if( ! token.add_character( ch ) )
			continue;

		tokens.push_back( token );
		token.reset();
	}

	CPPUNIT_ASSERT( tokens.size() == 9 );

	CPPUNIT_ASSERT( tokens[0].type() == PGNToken::TAGSTART );
	CPPUNIT_ASSERT( tokens[1].type() == PGNToken::TAGEND );
	CPPUNIT_ASSERT( tokens[2].type() == PGNToken::MOVENOINDICATOR );
	CPPUNIT_ASSERT( tokens[3].type() == PGNToken::GAMETERMINATOR );
	CPPUNIT_ASSERT( tokens[4].type() == PGNToken::RAVSTART );
	CPPUNIT_ASSERT( tokens[5].type() == PGNToken::RAVEND );
	CPPUNIT_ASSERT( tokens[6].type() == PGNToken::RESERVEDSTART );
	CPPUNIT_ASSERT( tokens[7].type() == PGNToken::RESERVEDEND );
	CPPUNIT_ASSERT( tokens[8].type() == PGNToken::LINETERMINATOR );
}

//STRING, INTEGER, COMMENT

void TestPNGToken::check_symbols()
{
	std::string input_data( "ASimpleSymbol asymbolwith1number 1symbolwithnumber " );

	std::vector<PGNToken> tokens;
	PGNToken token;

	for( char& ch : input_data ) {

		if( ! token.add_character( ch ) )
			continue;

		tokens.push_back( token );
		token.reset();
	}

	CPPUNIT_ASSERT( tokens.size() == 3 );

	CPPUNIT_ASSERT( tokens[0].type() == PGNToken::SYMBOL );
	CPPUNIT_ASSERT( tokens[1].type() == PGNToken::SYMBOL );
	CPPUNIT_ASSERT( tokens[2].type() == PGNToken::SYMBOL );

	CPPUNIT_ASSERT( tokens[0].data() == std::string("ASimpleSymbol") );
	CPPUNIT_ASSERT( tokens[1].data() == std::string("asymbolwith1number") );
	CPPUNIT_ASSERT( tokens[2].data() == std::string("1symbolwithnumber") );
}

void TestPNGToken::check_strings()
{
	std::string input_data( "\"AString\" \"AnotherString\"" );

	std::vector<PGNToken> tokens;
	PGNToken token;

	for( char& ch : input_data ) {

		if( ! token.add_character( ch ) )
			continue;

		tokens.push_back( token );
		token.reset();
	}

	CPPUNIT_ASSERT( tokens.size() == 2 );

	CPPUNIT_ASSERT( tokens[0].type() == PGNToken::STRING );
	CPPUNIT_ASSERT( tokens[1].type() == PGNToken::STRING );

	CPPUNIT_ASSERT( tokens[0].data() == std::string("AString") );
	CPPUNIT_ASSERT( tokens[1].data() == std::string("AnotherString") );
}

void TestPNGToken::check_integers()
{
	std::string input_data( "123 456 " );

	std::vector<PGNToken> tokens;
	PGNToken token;

	for( char& ch : input_data ) {

		if( ! token.add_character( ch ) )
			continue;

		tokens.push_back( token );
		token.reset();
	}

	CPPUNIT_ASSERT( tokens.size() == 2 );

	CPPUNIT_ASSERT( tokens[0].type() == PGNToken::INTEGER );
	CPPUNIT_ASSERT( tokens[1].type() == PGNToken::INTEGER );

	CPPUNIT_ASSERT( std::stoul(tokens[0].data()) == 123 );
	CPPUNIT_ASSERT( std::stoul(tokens[1].data()) == 456 );
}

void TestPNGToken::check_nags()
{
	std::string input_data( "$1 $2" );

	std::vector<PGNToken> tokens;
	PGNToken token;

	input_data.push_back( '\0' );

	for( char& ch : input_data ) {

		if( ! token.add_character( ch ) )
			continue;

		tokens.push_back( token );

		token.reset();
	}

	CPPUNIT_ASSERT_EQUAL( 2, static_cast<int>(tokens.size()) );

	CPPUNIT_ASSERT( tokens[0].type() == PGNToken::NAG );
	CPPUNIT_ASSERT( tokens[1].type() == PGNToken::NAG );

	CPPUNIT_ASSERT( std::stoul(tokens[0].data()) == 1 );
	CPPUNIT_ASSERT( std::stoul(tokens[1].data()) == 2 );
}

void TestPNGToken::check_tagpair()
{
	std::string input_data( "[Event \"Moscow City Championship\"]" );

	std::vector<PGNToken> tokens;
	PGNToken token;

	for( char& ch : input_data ) {

		if( ! token.add_character( ch ) )
			continue;

		tokens.push_back( token );
		token.reset();
	}

	CPPUNIT_ASSERT( tokens.size() == 4 );

	CPPUNIT_ASSERT( tokens[0].type() == PGNToken::TAGSTART );
	CPPUNIT_ASSERT( tokens[1].type() == PGNToken::SYMBOL );
	CPPUNIT_ASSERT( tokens[2].type() == PGNToken::STRING );
	CPPUNIT_ASSERT( tokens[3].type() == PGNToken::TAGEND );

	CPPUNIT_ASSERT( tokens[1].data() == std::string( "Event" ) );
	CPPUNIT_ASSERT( tokens[2].data() == std::string( "Moscow City Championship" ) );
}

void TestPNGToken::check_tagpair_section()
{
	std::string input_data( "[Event \"Moscow City Championship\"]\n[Site \"New York City, NY USA\"]\n\n" );

	std::vector<PGNToken> tokens;
	PGNToken token;

	for( char& ch : input_data ) {

		if( ! token.add_character( ch ) )
			continue;

		tokens.push_back( token );
		token.reset();
	}

	std::cout << std::endl;
	std::cout << "------------------------" << std::endl;
	std::for_each( tokens.begin(), tokens.end(), [](PGNToken& token) { std::cout << token; });
	std::cout << "------------------------" << std::endl;


	CPPUNIT_ASSERT( tokens.size() == 11 );

	CPPUNIT_ASSERT( tokens[0].type() == PGNToken::TAGSTART );
	CPPUNIT_ASSERT( tokens[1].type() == PGNToken::SYMBOL );
	CPPUNIT_ASSERT( tokens[2].type() == PGNToken::STRING );
	CPPUNIT_ASSERT( tokens[3].type() == PGNToken::TAGEND );
	CPPUNIT_ASSERT( tokens[4].type() == PGNToken::LINETERMINATOR );
	CPPUNIT_ASSERT( tokens[5].type() == PGNToken::TAGSTART );
	CPPUNIT_ASSERT( tokens[6].type() == PGNToken::SYMBOL );
	CPPUNIT_ASSERT( tokens[7].type() == PGNToken::STRING );
	CPPUNIT_ASSERT( tokens[8].type() == PGNToken::TAGEND );
	CPPUNIT_ASSERT( tokens[9].type() == PGNToken::LINETERMINATOR );
	CPPUNIT_ASSERT( tokens[10].type() == PGNToken::LINETERMINATOR );

	CPPUNIT_ASSERT( tokens[1].data() == std::string( "Event" ) );
	CPPUNIT_ASSERT( tokens[2].data() == std::string( "Moscow City Championship" ) );
	CPPUNIT_ASSERT( tokens[6].data() == std::string( "Site" ) );
	CPPUNIT_ASSERT( tokens[7].data() == std::string( "New York City, NY USA" ) );

}


