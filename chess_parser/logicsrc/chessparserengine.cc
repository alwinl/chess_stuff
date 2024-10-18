/*
 * ChessParser Copyright 2022 Alwin Leerling dna.leerling@gmail.com
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

#include "chessparserengine.h"

#include <fstream>

#include <regex>

using namespace std;

bool ChessParserEngine::open_file( std::string filename )
{
	tag_pairs.clear();
	movetexts.clear();

	if( filename.empty() )
		return false;

	ifstream is( filename.c_str() );

	if( !is.good() )
		return false;

	load( string( istreambuf_iterator<char>(is), istreambuf_iterator<char>() ) );

	is.close();
	return true;
}

void ChessParserEngine::load( std::string pgn_string )
{
	regex re_tvpair( "\\[(.*) \"(.*)\"\\]\\n" );
	smatch tvpair_match;

	while( regex_search( pgn_string, tvpair_match, re_tvpair ) ) {
		tag_pairs.push_back( make_pair( tvpair_match[1], tvpair_match[2]) );
		pgn_string = tvpair_match.suffix();
	}

	regex re_movetext( "(\\d*)\\.\\s(\\S*)\\s(\\S*)?" );
	smatch movetext_match;

	while( regex_search( pgn_string, movetext_match, re_movetext ) ) {
		movetexts.push_back( make_pair( movetext_match[2], movetext_match[3]) );
		pgn_string = movetext_match.suffix();
	}
}

void ChessParserEngine::visit_tag_pairs( ParserVisitorBase* processor )
{
	for( auto tag_pair : tag_pairs )
		processor->process_tag_pair( tag_pair.first, tag_pair.second );
}

void ChessParserEngine::visit_movetext( ParserVisitorBase* processor )
{
	for( auto movetext : movetexts )
		processor->process_movetext( movetext.first, movetext.second );
}

