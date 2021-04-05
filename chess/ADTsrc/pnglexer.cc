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
	char ch;
	char last_ch;
	PNGToken new_token;
	bool eating_comment = false;

	for(;;) {

		is.get( ch );

		if( is.eof() )
			return PNGToken::EOFToken();

		if( eating_comment ) {
			if( ch != '\n' )
				continue;

			eating_comment = false;
			continue;
		}

		if( ch == ';' ) {					// eat line comments
			eating_comment = true;
			continue;
		}

		if( last_ch == '\n' && ch == '\n')		// empty line => end of section
			return PNGToken::EOSToken();

		last_ch = ch;

		if( ch == '\n' )
			continue;

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
void PNGLexer::ParseTagPairSection( ChessGame& game )
{
	std::pair<std::string, std::string>  tag_pair;
	enum eSTAGES { NOTHING, GOT_TAG_START, GOT_TAG, GOT_DATA, } stage = NOTHING;

	for(;;) {

		PNGToken token = get_next_token();

		if( token.type() == PNGToken::FILEEND )		// should never happen
			return;

		if( token.type() == PNGToken::SECTIONEND )		// all tag pairs parsed, go home
			return;

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
			game.add_tag_pair( tag_pair.first, tag_pair.second );
			stage = NOTHING;
			break;
		}
	}
}

