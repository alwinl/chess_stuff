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

 /* https://en.wikipedia.org/wiki/Portable_Game_Notation */

 /* http://www.saremba.de/chessgml/standards/pgn/pgn-complete.htm */

#include "pgnparser.h"
#include "pgntoken.h"

#include <istream>
#include <algorithm>

#include "ply.h"

ChessGame PGNParser::do_parse( std::istream& is )
{
	std::vector<PGNToken> tokens = tokenise( is );

	replace_lineterminators_with_sectionend( tokens );
	remove_lineends( tokens );
	replace_three_moveindicators_with_movesubstitute( tokens );

    return build_game( tokens );
}

std::vector<PGNToken> PGNParser::tokenise( std::istream& is )
{
	std::vector<PGNToken> tokens;
	PGNToken new_token;
	char ch;
	bool escaping = false;
	char last_char = '\n';

	while( is.good() ) {

		is.get( ch );

		if( is.eof() )
            break;

		// Escape mechanism: http://www.saremba.de/chessgml/standards/pgn/pgn-complete.htm chapter 6
		if( ch == '%' && last_char == '\n' )	// do we have the '%' as the first character in the first column of a line?
			escaping = true;

		if( escaping ) {
			if( ch != '\n' )
				escaping = false;
			continue;
		}

		last_char = ch;

		if( ! new_token.add_character( ch ) )
			continue;

		tokens.push_back( new_token );

		if( new_token.type() != PGNToken::SYMBOL ) {
			new_token.reset();
			continue;
		}

		new_token.reset();

		if( new_token.add_character( ch ) ) {
			tokens.push_back( new_token );
			new_token.reset();
		}
	}

	if( new_token.add_character( '\0' ) )
		tokens.push_back( new_token );

	tokens.push_back( PGNToken::EOFToken() );

	return tokens;
}

void PGNParser::replace_lineterminators_with_sectionend( std::vector<PGNToken>& tokens )
{
	for( ;; ) {
		std::vector<PGNToken>::iterator it = std::adjacent_find( tokens.begin(), tokens.end(), [](PGNToken token1, PGNToken token2)
							{ return (token1.type() == PGNToken::LINETERMINATOR) && (token2.type() == PGNToken::LINETERMINATOR); } );

		if( it == tokens.end() )
			break;

		it = tokens.insert( it, PGNToken::EOSToken() );
		tokens.erase( it + 1, it + 3 );
	}
}

void PGNParser::replace_three_moveindicators_with_movesubstitute( std::vector<PGNToken>& tokens )
{
	for( ;; ) {
		std::vector<PGNToken>::iterator it = std::adjacent_find( tokens.begin(), tokens.end(), [](PGNToken token1, PGNToken token2)
							{ return (token1.type() == PGNToken::MOVENOINDICATOR) && (token2.type() == PGNToken::MOVENOINDICATOR); } );

		if( it == tokens.end() || std::next(it, 2) == tokens.end() )
			break;

		if( ((*std::next(it, 2)).type() != PGNToken::MOVENOINDICATOR) )	// the third token also has to be a MOVEINDICATOR
			continue;

		it = tokens.insert( it+1, PGNToken( PGNToken::SYMBOL, ".." ) );
		tokens.erase( it + 1, it + 3 );
	}
}

void PGNParser::remove_lineends( std::vector<PGNToken>& tokens )
{
	tokens.erase( std::remove_if( tokens.begin(), tokens.end(), [](PGNToken token) { return token.type() == PGNToken::LINETERMINATOR; } ), tokens.end() );
}

ChessGame PGNParser::build_game( std::vector<PGNToken>& tokens )
{
	ChessGame game;

	CollectionState state = { CollectionState::COLLECTED_NONE };

	std::string tag;
	std::string value;
	unsigned int moveno;
	std::string white_move;
	std::string black_move;

	for_each( tokens.begin(), tokens.end(),
		[&](PGNToken& token)
		{
			switch( state ) {
			case CollectionState::COLLECTED_NONE:
				if( token.type() == PGNToken::SECTIONEND ) {
					state = CollectionState::COLLECTED_TAGPAIRS;
					break;
				}

				if( token.type() != PGNToken::SYMBOL )		// We ignore the TAGSTART and TAGEND tokems
					break;

				tag = token.data();
				state = CollectionState::COLLECTED_TAG;
				break;

			case CollectionState::COLLECTED_TAG:
				if( token.type() != PGNToken::STRING )
					break;

				value = token.data();
				game.add_tag_pair( tag, value );
				state = CollectionState::COLLECTED_NONE;
				break;

			case CollectionState::COLLECTED_TAGPAIRS:
				if( token.type() == PGNToken::COMMENT )
					game.add_comment( token.data() );

				if( token.type() != PGNToken::MOVENOINDICATOR )
					break;
				moveno = std::atoi( token.data().c_str() );
				state = CollectionState::COLLECTED_MOVENO;
				break;

			case CollectionState::COLLECTED_MOVENO:
				if( token.type() != PGNToken::SYMBOL )
					break;
				game.add_white_move( moveno, token.data() );
				state = CollectionState::COLLECTED_WHITE;
				break;

			case CollectionState::COLLECTED_WHITE:
				if( token.type() != PGNToken::SYMBOL )
					break;
				game.add_black_move( moveno, token.data() );
				state = CollectionState::COLLECTED_TAGPAIRS;
				break;
			}

		}
	);

	return game;
}
