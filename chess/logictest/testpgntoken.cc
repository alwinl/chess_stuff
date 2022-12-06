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

#include "pgntoken.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TestPNGToken );

void TestPNGToken::check_EOFToken()
{
	CPPUNIT_ASSERT_EQUAL( PGNToken::FILEEND, PGNToken::EOFToken().type() );
}

void TestPNGToken::check_EOSToken()
{
	CPPUNIT_ASSERT_EQUAL( PGNToken::SECTIONEND, PGNToken::EOSToken().type() );
}

std::vector<PGNToken> TestPNGToken::process_string( std::string input_data )
{
	std::vector<PGNToken> tokens;
	PGNToken token;

	for( char& ch : input_data ) {

		if( ! token.add_character( ch ) )
			continue;

		tokens.push_back( token );
		token.reset();
	}

	if( token.add_character( '\0' ) )
		tokens.push_back( token );

	return tokens;
}

void TestPNGToken::check_self_terminators()
{
	std::string input_data( "[].*()<>\n" );

	std::vector<PGNToken> tokens = process_string( input_data );

	CPPUNIT_ASSERT_EQUAL( 9ul, tokens.size() );

	CPPUNIT_ASSERT_EQUAL( PGNToken::TAGSTART, tokens[0].type() );
	CPPUNIT_ASSERT_EQUAL( PGNToken::TAGEND, tokens[1].type() );
	CPPUNIT_ASSERT_EQUAL( PGNToken::MOVENOINDICATOR, tokens[2].type() );
	CPPUNIT_ASSERT_EQUAL( PGNToken::GAMETERMINATOR, tokens[3].type() );
	CPPUNIT_ASSERT_EQUAL( PGNToken::RAVSTART, tokens[4].type() );
	CPPUNIT_ASSERT_EQUAL( PGNToken::RAVEND, tokens[5].type() );
	CPPUNIT_ASSERT_EQUAL( PGNToken::RESERVEDSTART, tokens[6].type() );
	CPPUNIT_ASSERT_EQUAL( PGNToken::RESERVEDEND, tokens[7].type() );
	CPPUNIT_ASSERT_EQUAL( PGNToken::LINETERMINATOR, tokens[8].type() );
}

//STRING, INTEGER, COMMENT

void TestPNGToken::check_symbols()
{
	std::string input_data( "ASimpleSymbol asymbolwith1number 1symbolwithnumber" );

	std::vector<PGNToken> tokens = process_string( input_data );

	CPPUNIT_ASSERT_EQUAL( 3ul, tokens.size() );

	CPPUNIT_ASSERT_EQUAL( PGNToken::SYMBOL, tokens[0].type() );
	CPPUNIT_ASSERT_EQUAL( PGNToken::SYMBOL, tokens[1].type() );
	CPPUNIT_ASSERT_EQUAL( PGNToken::SYMBOL, tokens[2].type() );

	CPPUNIT_ASSERT_EQUAL( std::string("ASimpleSymbol"), tokens[0].data() );
	CPPUNIT_ASSERT_EQUAL( std::string("asymbolwith1number"), tokens[1].data() );
	CPPUNIT_ASSERT_EQUAL( std::string("1symbolwithnumber"), tokens[2].data() );
}

void TestPNGToken::check_strings()
{
	std::string input_data( "\"AString\" \"AnotherString\"" );

	std::vector<PGNToken> tokens = process_string( input_data );

	CPPUNIT_ASSERT_EQUAL( 2ul, tokens.size() );

	CPPUNIT_ASSERT_EQUAL( PGNToken::STRING, tokens[0].type() );
	CPPUNIT_ASSERT_EQUAL( PGNToken::STRING, tokens[1].type() );

	CPPUNIT_ASSERT_EQUAL( std::string("AString"), tokens[0].data() );
	CPPUNIT_ASSERT_EQUAL( std::string("AnotherString"), tokens[1].data() );
}

void TestPNGToken::check_integers()
{
	std::string input_data( "123 456" );

	std::vector<PGNToken> tokens = process_string( input_data );

	CPPUNIT_ASSERT_EQUAL( 2ul, tokens.size() );

	CPPUNIT_ASSERT_EQUAL( PGNToken::INTEGER, tokens[0].type() );
	CPPUNIT_ASSERT_EQUAL( PGNToken::INTEGER, tokens[1].type() );

	CPPUNIT_ASSERT_EQUAL( 123, std::stoi(tokens[0].data()) );
	CPPUNIT_ASSERT_EQUAL( 456, std::stoi(tokens[1].data()) );
}

void TestPNGToken::check_nags()
{
	std::string input_data( "$1 $2" );

	std::vector<PGNToken> tokens = process_string( input_data );

	CPPUNIT_ASSERT_EQUAL( 2ul, tokens.size() );

	CPPUNIT_ASSERT_EQUAL( PGNToken::NAG, tokens[0].type()  );
	CPPUNIT_ASSERT_EQUAL( PGNToken::NAG, tokens[1].type() );

	CPPUNIT_ASSERT_EQUAL( 1, std::stoi(tokens[0].data()) );
	CPPUNIT_ASSERT_EQUAL( 2, std::stoi(tokens[1].data()) );
}

void TestPNGToken::check_tagpair()
{
	std::string input_data( "[Event \"Moscow City Championship\"]" );

	std::vector<PGNToken> tokens = process_string( input_data );

	CPPUNIT_ASSERT_EQUAL( 4ul, tokens.size() );

	CPPUNIT_ASSERT_EQUAL( PGNToken::TAGSTART, tokens[0].type() );
	CPPUNIT_ASSERT_EQUAL( PGNToken::SYMBOL, tokens[1].type() );
	CPPUNIT_ASSERT_EQUAL( PGNToken::STRING, tokens[2].type() );
	CPPUNIT_ASSERT_EQUAL( PGNToken::TAGEND, tokens[3].type() );

	CPPUNIT_ASSERT_EQUAL( std::string( "Event" ), tokens[1].data()  );
	CPPUNIT_ASSERT_EQUAL( std::string( "Moscow City Championship" ), tokens[2].data() );
}

void TestPNGToken::check_tagpair_section()
{
	std::string input_data( "[Event \"Moscow City Championship\"]\n[Site \"New York City, NY USA\"]\n\n" );

	std::vector<PGNToken> tokens = process_string( input_data );

	CPPUNIT_ASSERT_EQUAL(  11ul, tokens.size() );

	CPPUNIT_ASSERT_EQUAL( PGNToken::TAGSTART, tokens[0].type() );
	CPPUNIT_ASSERT_EQUAL( PGNToken::SYMBOL, tokens[1].type() );
	CPPUNIT_ASSERT_EQUAL( PGNToken::STRING, tokens[2].type() );
	CPPUNIT_ASSERT_EQUAL( PGNToken::TAGEND, tokens[3].type() );
	CPPUNIT_ASSERT_EQUAL( PGNToken::LINETERMINATOR, tokens[4].type() );
	CPPUNIT_ASSERT_EQUAL( PGNToken::TAGSTART, tokens[5].type() );
	CPPUNIT_ASSERT_EQUAL( PGNToken::SYMBOL, tokens[6].type() );
	CPPUNIT_ASSERT_EQUAL( PGNToken::STRING, tokens[7].type() );
	CPPUNIT_ASSERT_EQUAL( PGNToken::TAGEND, tokens[8].type() );
	CPPUNIT_ASSERT_EQUAL( PGNToken::LINETERMINATOR, tokens[9].type() );
	CPPUNIT_ASSERT_EQUAL( PGNToken::LINETERMINATOR, tokens[10].type() );

	CPPUNIT_ASSERT_EQUAL( std::string( "Event" ), tokens[1].data() );
	CPPUNIT_ASSERT_EQUAL( std::string( "Moscow City Championship" ), tokens[2].data() );
	CPPUNIT_ASSERT_EQUAL( std::string( "Site" ), tokens[6].data() );
	CPPUNIT_ASSERT_EQUAL( std::string( "New York City, NY USA" ), tokens[7].data() );
}


