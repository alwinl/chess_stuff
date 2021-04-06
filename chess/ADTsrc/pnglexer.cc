/*
 * Copyright 2019 Alwin Leerling <dna.leerling@gmail.com>
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

#include "pnglexer.h"

#include "chessgame.h"

#include <iostream>

PNGLexer::PNGLexer( std::istream& is_ ) : is(is_)
{
}


PNGToken PNGLexer::get_next_token( )
{
	PNGToken new_token;
	char ch;

	for(;;) {

		is.get( ch );

		if( is.eof() )
            break;

		if( new_token.add_character( ch ) )
			return new_token;
	}

	return PNGToken::EOFToken();
}

void PNGLexer::parse_error( std::string message )
{
	std::cout << message << std::endl;
}

/*
 */
std::map<std::string, std::string> PNGLexer::parse_tag_pair_section(  )
{
    std::map<std::string, std::string> result;
	std::pair<std::string, std::string>  tag_pair;
	enum eSTAGES { NOTHING, GOT_TAG_START, GOT_TAG, GOT_DATA, } stage = NOTHING;
	bool last_token_is_nl = false;

	for(;;) {

		PNGToken token = get_next_token();

		if( token.type() == PNGToken::EOFToken().type() )
            return std::map<std::string, std::string>();      // should never happen, return an empty map

		if( token.type() == PNGToken::LINETERMINATOR ) {
            if( last_token_is_nl )          // Two newlines is a row means an empty line, ergo end of section
                return result;

            last_token_is_nl = true;
		} else
            last_token_is_nl = false;

		switch( stage ) {
		case NOTHING:
			if( token.type() != PNGToken::TAGSTART )
				break;

			stage = GOT_TAG_START;
			break;

		case GOT_TAG_START:
			if( token.type() != PNGToken::SYMBOL ) {
				parse_error( "Expecting a symbol here" );
				stage = NOTHING;
				break;
			}
			tag_pair.first = token.data();
			stage = GOT_TAG;
			break;

		case GOT_TAG:
			if( token.type() != PNGToken::STRING ) {
				parse_error( "Expecting a string here" );
				stage = NOTHING;
				break;
			}
			tag_pair.second = token.data();
			stage = GOT_DATA;
			break;

		case GOT_DATA:
			if( token.type() != PNGToken::TAGEND ) {
				parse_error( "Expecting a tag end here" );
				stage = NOTHING;
				break;
			}
			result.insert( tag_pair );
			stage = NOTHING;
			break;
		}
	}
}

std::vector<Ply> PNGLexer::parse_movetext_section()
{
    std::vector<Ply> result;
    Ply ply;
	enum eSTAGES { NOTHING, GOT_MOVEINDICATOR } stage = NOTHING;
	bool last_token_is_nl = false;

	for(;;) {

		PNGToken token = get_next_token();

		if( token.type() == PNGToken::EOFToken().type() )
            return std::vector<Ply>();      // should never happen, return an empty map

		if( token.type() == PNGToken::LINETERMINATOR ) {
            if( last_token_is_nl )          // Two newlines is a row means an empty line, ergo end of section
                return result;

            last_token_is_nl = true;
		} else
            last_token_is_nl = false;

		switch( stage ) {
        case NOTHING:
			if( token.type() != PNGToken::INTEGER )
				break;

			stage = GOT_MOVEINDICATOR;
			break;

        case GOT_MOVEINDICATOR:
            break;
		}
	}
}

