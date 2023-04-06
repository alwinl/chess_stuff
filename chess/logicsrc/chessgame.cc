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

using namespace std;


ChessGame::ChessGame( )
{
    initial = Board();
    moves.clear();
    tag_pairs.clear();
}

void ChessGame::add_tag_pair( std::string tag, std::string value )
{
	tag_pairs.insert( std::pair<std::string, std::string>(tag, value) );

	// http://www.saremba.de/chessgml/standards/pgn/pgn-complete.htm chapter 9.7 Alternative starting positions
	if( (tag == "SetUp") || ( tag == "FEN" ) )
		set_alternate_starting_position();
}

void ChessGame::set_alternate_starting_position()
{
	map<std::string,std::string>::iterator SetUp_it = tag_pairs.find( "SetUp" );
	map<std::string,std::string>::iterator FEN_it = tag_pairs.find( "FEN" );

	if( (SetUp_it == tag_pairs.end() ) || (FEN_it == tag_pairs.end() ) )
		return;

	if( (*SetUp_it).second != "1" )
		return;

	initial = Board( (*FEN_it).second );
}


void ChessGame::add_white_move( unsigned int moveno, std::string the_move )
{
	//moves.push_back( Ply( Ply::Colour::WHITE, moveno, the_move ) );
}

void ChessGame::add_black_move( unsigned int moveno, std::string the_move )
{
	//moves.push_back( Ply( Ply::Colour::BLACK, moveno, the_move ) );
}

void ChessGame::add_comment( std::string the_comment )
{
	if( moves.empty() )
		return;		// ignore comments before any movetext at the moment

	//std::vector<Ply>::iterator it = std::prev( moves.end(), 1);

	//(*it).add_comment( the_comment );
}

