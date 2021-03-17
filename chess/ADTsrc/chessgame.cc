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

//#include <sstream>
//#include <vector>
//#include <iostream>

using namespace std;


/** \brief
 *
 * \param
 *
 */
ChessGame::ChessGame( )
{
    initialise();
}

/** \brief
 *
 * \return void
 *
 */
void ChessGame::initialise()
{
    moves.clear();
    game_states.push_back( make_game_state("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") );
    current = 0;

}

/** \brief
 *
 * \param file_name std::string
 * \return int
 *
 */
int ChessGame::store_game( const std::string& file_name )
{
    return 0;
}

int ChessGame::load_game( const std::string& file_name )
{
    initialise();


    game_states.push_back( make_game_state("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1") );
    game_states.push_back( make_game_state("rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2") );
    game_states.push_back( make_game_state("rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2") );

    return 0;
}

void ChessGame::add_tag_pair( std::string tag, std::string value )
{
	//tag_pairs.insert( make_pair<std::string,std::string>(tag, value) );
}



