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

#include "chessgame.h"

#include <regex>

using namespace std;


ChessGame::ChessGame( )
{
    initial = Board();
    plys.clear();
    tag_pairs.clear();
}

void ChessGame::load_game( std::string pgn_string )
{
	regex re_tvpair( "\\[(.*) \"(.*)\"\\]\\n" );
	smatch tvpair_match;

	tag_pairs.clear();
	plys.clear();
    initial = Board();

	while( regex_search( pgn_string, tvpair_match, re_tvpair ) ) {
		add_tag_pair( tvpair_match[1], tvpair_match[2] );
		pgn_string = tvpair_match.suffix();
	}

	regex re_movetext( "(\\d*)\\.\\s(\\S*)\\s(\\S*)?" );
	smatch movetext_match;

	Board current;

	while( regex_search( pgn_string, movetext_match, re_movetext ) ) {

		if( movetext_match[2] != "..." )
			add_ply( white, movetext_match[2], current );

		if( movetext_match[3] != "" )
			add_ply( black, movetext_match[3], current );

		pgn_string = movetext_match.suffix();
	}
}

void ChessGame::add_ply( eColor color, std::string SAN, Board& current )
{

}

void ChessGame::add_tag_pair( std::string tag, std::string value )
{
	tag_pairs.push_back( make_pair(tag, value) );

	// http://www.saremba.de/chessgml/standards/pgn/pgn-complete.htm chapter 9.7 Alternative starting positions
	if( (tag == "SetUp") || ( tag == "FEN" ) )
		set_alternate_starting_position();
}

void ChessGame::set_alternate_starting_position()
{
	auto SetUp_it = find_if( tag_pairs.begin(), tag_pairs.end(), []( auto tag_pair ) { return tag_pair.first == "SetUp";} );
	auto FEN_it = find_if( tag_pairs.begin(), tag_pairs.end(), []( auto tag_pair ) { return tag_pair.first == "FEN";} );

	if( (SetUp_it == tag_pairs.end() ) || (FEN_it == tag_pairs.end() ) )
		return;

	if( (*SetUp_it).second != "1" )
		return;

	initial = Board( (*FEN_it).second );
}

std::string ChessGame::save_game()
{
	string result;

	for( auto tag_pair : tag_pairs )
		result += "[" + tag_pair.first + " \"" + tag_pair.second + "\"]\n";

	result += "\n";

//	for( auto ply : plys ) {
//	}

	return result;
}

void ChessGame::visit_tag_pairs( ChessGameVisitorBase* processor )
{
	for( auto tag_pair : tag_pairs )
		processor->process_tag_pair( tag_pair.first, tag_pair.second );
}

void ChessGame::visit_plys( ChessGameVisitorBase* processor )
{
	for( auto ply : plys )
		processor->process_ply( ply );
}
