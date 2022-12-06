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

struct ParseTagPairs
{
public:
	ParseTagPairs( PGNParserCollector* adt_ ) : adt(adt_) {};

	void operator()( PGNToken& token );

private:
	PGNParserCollector* adt;

	std::string tag;
	std::string value;

	enum CollectionState { COLLECTED_NONE, COLLECTED_TAG } state = COLLECTED_NONE;
};

struct ParseMoveText
{
public:
	ParseMoveText( PGNParserCollector* adt_ ) : adt(adt_) {};

	void operator()( PGNToken& token );

private:
	PGNParserCollector* adt;

	unsigned int moveno;
	std::string white_move;
	std::string black_move;

	enum CollectionState { COLLECTED_NONE, COLLECTED_MOVENO, COLLECTED_WHITE } state = COLLECTED_NONE;
};

bool PGNParser::do_parse( std::istream& is, PGNParserCollector* collector )
{
	std::vector<PGNToken> tokens;

	tokenise( tokens, is );
	replace_lineterminators_with_sectionend( tokens );
	remove_lineends( tokens );
	replace_three_moveindicators_with_movesubstitute( tokens );

	collector->debug_token_list( tokens );

	process_tokens( tokens, collector );

    return true;
}

void PGNParser::tokenise( std::vector<PGNToken>& tokens, std::istream& is )
{
	PGNToken new_token;
	char ch;

	while( is.good() ) {

		is.get( ch );

		if( is.eof() )
            break;

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

void PGNParser::process_tokens( std::vector<PGNToken> tokens, PGNParserCollector* collector )
{
	auto it = find_if( tokens.begin(), tokens.end(), [](PGNToken token) { return token.type() == PGNToken::SECTIONEND; });

	for_each( tokens.begin(), it, ParseTagPairs( collector ) );
	for_each( it, tokens.end(), ParseMoveText( collector ) );
}

void ParseTagPairs::operator()(PGNToken& token )
{
	switch( state ) {
	case COLLECTED_NONE:
		if( token.type() != PGNToken::SYMBOL )
			break;
		tag = token.data();
		state = COLLECTED_TAG;
		break;
	case COLLECTED_TAG:
		if( token.type() != PGNToken::STRING )
			break;
		value = token.data();
		adt->add_tag_pair( tag, value );
		state = COLLECTED_NONE;
		break;
	}
}

void ParseMoveText::operator()( PGNToken& token )
{
	switch( state ) {
	case COLLECTED_NONE:
		if( token.type() != PGNToken::MOVENOINDICATOR )
			break;
		moveno = std::atoi( token.data().c_str() );
		state = COLLECTED_MOVENO;
		break;
	case COLLECTED_MOVENO:
		if( token.type() != PGNToken::SYMBOL )
			break;
		white_move = token.data();
		state = COLLECTED_WHITE;
		break;

	case COLLECTED_WHITE:
		if( token.type() != PGNToken::SYMBOL )
			break;
		black_move = token.data();
		adt->add_move_text( moveno, white_move, black_move );
		state = COLLECTED_NONE;
		break;
	}
}


