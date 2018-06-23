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

#ifndef APPMODEL_H
#define APPMODEL_H

#include <vector>
#include <string>

#include <utility>

#include "pods.h"

/**-----------------------------------------------------------------------------
 * \brief Top level application model (ADT)
 *
 * The model consists of a list of game states described in FEN
 * For convenience the model also keeps a list of moves in LAN
 * This class encapsulates the storage requirements of the application
 *
 * We need to be able to to the following:
 *
 * Commands (in):
 *
 * reset/initialise for a standard/new game
 * load a game from permanent storage.
 * store a game to permanent storage.
 *
 * reset/initialise a completely empty board (for editing, custom set-up)
 * insert a piece at a given position (for editing)     Validation required
 * remove a piece from a given position (for editing)
 *
 * add a move.                  We have to validate the move to add
 * remove last added move.
 *
 * Queries (out):
 * Return current piece positions/ single piece position
 * Return info/state information
 *
 */
class AppModel
{
public:
	AppModel( );

    int new_game();
    int store_game( const std::string& file_name );
    int load_game( const std::string& file_name );

    void initialise();
    void insert_piece( STSquare square, char piece );
    void remove_piece( STSquare square, char piece );       // piece used for checking, not necessary for operation

    void add_move( );
    void remove_move();

    void advance() { if( current < game_states.size() ) current += 1; };

    std::string get_working_folder() { return "~/Documents"; };
    std::string get_piece_positions() { return game_states[current].piece_positions; };
    char get_piece( STSquare square );
    STInfo get_info() { return info; };

private:

    STInfo info;


    std::vector<STGameState> game_states;
    unsigned int current;
    std::vector<unsigned int> moves;
};

#endif // APPMODEL_H
