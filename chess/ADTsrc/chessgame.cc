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
}

void ChessGame::add_ply( Ply new_ply )
{
    moves.push_back( new_ply );
}

void ChessGame::add_tag_pair( std::string tag, std::string value )
{
	tag_pairs.insert( std::pair<std::string, std::string>(tag, value) );
}

std::string ChessGame::get_tag_value( std::string key )
{
    map<std::string,std::string>::iterator it = tag_pairs.find( key );

    if( it != tag_pairs.end() )
        return (*it).second;

    return "";
}
