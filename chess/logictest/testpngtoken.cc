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

#include "testpngtoken.h"

//#include <sstream>
#include <iostream>

#include "../ADTsrc/pngtoken.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TestPNGToken );

TestPNGToken::TestPNGToken()
{
}

TestPNGToken::~TestPNGToken()
{
}

void TestPNGToken::check_EOFToken()
{
	CPPUNIT_ASSERT( PNGToken::EOFToken().type() == PNGToken::FILEEND );
}

void TestPNGToken::check_EOSToken()
{
	CPPUNIT_ASSERT( PNGToken::EOSToken().type() == PNGToken::SECTIONEND );
}

void TestPNGToken::check_self_terminators()
{
	std::string input_data( "[].*()<>\n" );

	std::vector<PNGToken> tokens;
	PNGToken token;

	for( char& ch : input_data ) {

		if( ! token.add_character( ch ) )
			continue;

		tokens.push_back( token );
		token.reset();
	}

	CPPUNIT_ASSERT( tokens.size() == 9 );

	CPPUNIT_ASSERT( tokens[0].type() == PNGToken::TAGSTART );
	CPPUNIT_ASSERT( tokens[1].type() == PNGToken::TAGEND );
	CPPUNIT_ASSERT( tokens[2].type() == PNGToken::MOVENOINDICATOR );
	CPPUNIT_ASSERT( tokens[3].type() == PNGToken::GAMETERMINATOR );
	CPPUNIT_ASSERT( tokens[4].type() == PNGToken::RAVSTART );
	CPPUNIT_ASSERT( tokens[5].type() == PNGToken::RAVEND );
	CPPUNIT_ASSERT( tokens[6].type() == PNGToken::RESERVEDSTART );
	CPPUNIT_ASSERT( tokens[7].type() == PNGToken::RESERVEDEND );
	CPPUNIT_ASSERT( tokens[8].type() == PNGToken::LINETERMINATOR );
}

//STRING, INTEGER, COMMENT

void TestPNGToken::check_symbols()
{
	std::string input_data( "ASimpleSymbol asymbolwith1number 1symbolwithnumber " );

	std::vector<PNGToken> tokens;
	PNGToken token;

	for( char& ch : input_data ) {

		if( ! token.add_character( ch ) )
			continue;

		tokens.push_back( token );
		token.reset();
	}

	CPPUNIT_ASSERT( tokens.size() == 3 );

	CPPUNIT_ASSERT( tokens[0].type() == PNGToken::SYMBOL );
	CPPUNIT_ASSERT( tokens[1].type() == PNGToken::SYMBOL );
	CPPUNIT_ASSERT( tokens[2].type() == PNGToken::SYMBOL );

	CPPUNIT_ASSERT( tokens[0].data() == std::string("ASimpleSymbol") );
	CPPUNIT_ASSERT( tokens[1].data() == std::string("asymbolwith1number") );
	CPPUNIT_ASSERT( tokens[2].data() == std::string("1symbolwithnumber") );
}

void TestPNGToken::check_strings()
{
	std::string input_data( "\"AString\" \"AnotherString\"" );

	std::vector<PNGToken> tokens;
	PNGToken token;

	for( char& ch : input_data ) {

		if( ! token.add_character( ch ) )
			continue;

		tokens.push_back( token );
		token.reset();
	}

	CPPUNIT_ASSERT( tokens.size() == 3 );

	CPPUNIT_ASSERT( tokens[0].type() == PNGToken::STRING );
	CPPUNIT_ASSERT( tokens[1].type() == PNGToken::INVALID );
	CPPUNIT_ASSERT( tokens[2].type() == PNGToken::STRING );

	CPPUNIT_ASSERT( tokens[0].data() == std::string("AString") );
	CPPUNIT_ASSERT( tokens[2].data() == std::string("AnotherString") );
}

void TestPNGToken::check_integers()
{
	std::string input_data( "123 456 " );

	std::vector<PNGToken> tokens;
	PNGToken token;

	for( char& ch : input_data ) {

		if( ! token.add_character( ch ) )
			continue;

		tokens.push_back( token );
		token.reset();
	}

	CPPUNIT_ASSERT( tokens.size() == 2 );

	CPPUNIT_ASSERT( tokens[0].type() == PNGToken::INTEGER );
	CPPUNIT_ASSERT( tokens[1].type() == PNGToken::INTEGER );

	CPPUNIT_ASSERT( std::stoul(tokens[0].data()) == 123 );
	CPPUNIT_ASSERT( std::stoul(tokens[1].data()) == 456 );
}

void TestPNGToken::check_nags()
{
	std::string input_data( "$1 $2a" );

	std::vector<PNGToken> tokens;
	PNGToken token;

	for( char& ch : input_data ) {

		if( ! token.add_character( ch ) )
			continue;

		tokens.push_back( token );
		token.reset();
	}

	CPPUNIT_ASSERT( tokens.size() == 2 );

	CPPUNIT_ASSERT( tokens[0].type() == PNGToken::NAG );
	CPPUNIT_ASSERT( tokens[1].type() == PNGToken::NAG );

	CPPUNIT_ASSERT( std::stoul(tokens[0].data()) == 1 );
	CPPUNIT_ASSERT( std::stoul(tokens[1].data()) == 2 );
}

void TestPNGToken::check_tagpair()
{
	std::string input_data( "[Event \"Moscow City Championship\"]" );

	std::vector<PNGToken> tokens;
	PNGToken token;

	for( char& ch : input_data ) {

		if( ! token.add_character( ch ) )
			continue;

		tokens.push_back( token );
		token.reset();
	}

	CPPUNIT_ASSERT( tokens.size() == 4 );

	CPPUNIT_ASSERT( tokens[0].type() == PNGToken::TAGSTART );
	CPPUNIT_ASSERT( tokens[1].type() == PNGToken::SYMBOL );
	CPPUNIT_ASSERT( tokens[2].type() == PNGToken::STRING );
	CPPUNIT_ASSERT( tokens[3].type() == PNGToken::TAGEND );

	CPPUNIT_ASSERT( tokens[1].data() == std::string( "Event" ) );
	CPPUNIT_ASSERT( tokens[2].data() == std::string( "Moscow City Championship" ) );
}

void TestPNGToken::check_tagpair_section()
{
	std::string input_data( "[Event \"Moscow City Championship\"]\n[Site \"New York City, NY USA\"]\n\n" );

	std::vector<PNGToken> tokens;
	PNGToken token;

	for( char& ch : input_data ) {

		if( ! token.add_character( ch ) )
			continue;

		tokens.push_back( token );
		token.reset();
	}

	CPPUNIT_ASSERT( tokens.size() == 11 );

	CPPUNIT_ASSERT( tokens[0].type() == PNGToken::TAGSTART );
	CPPUNIT_ASSERT( tokens[1].type() == PNGToken::SYMBOL );
	CPPUNIT_ASSERT( tokens[2].type() == PNGToken::STRING );
	CPPUNIT_ASSERT( tokens[3].type() == PNGToken::TAGEND );
	CPPUNIT_ASSERT( tokens[4].type() == PNGToken::LINETERMINATOR );
	CPPUNIT_ASSERT( tokens[5].type() == PNGToken::TAGSTART );
	CPPUNIT_ASSERT( tokens[6].type() == PNGToken::SYMBOL );
	CPPUNIT_ASSERT( tokens[7].type() == PNGToken::STRING );
	CPPUNIT_ASSERT( tokens[8].type() == PNGToken::TAGEND );
	CPPUNIT_ASSERT( tokens[9].type() == PNGToken::LINETERMINATOR );
	CPPUNIT_ASSERT( tokens[10].type() == PNGToken::LINETERMINATOR );

	CPPUNIT_ASSERT( tokens[1].data() == std::string( "Event" ) );
	CPPUNIT_ASSERT( tokens[2].data() == std::string( "Moscow City Championship" ) );
	CPPUNIT_ASSERT( tokens[6].data() == std::string( "Site" ) );
	CPPUNIT_ASSERT( tokens[7].data() == std::string( "New York City, NY USA" ) );

}


