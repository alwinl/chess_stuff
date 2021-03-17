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

#include "board.h"
#include "ply.h"

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
class ChessGame
{
public:
	ChessGame( );

    int new_game();
    int store_game( const std::string& file_name );
    int load_game( const std::string& file_name );

    void initialise();

    void add_move( );
    void remove_move();

    void advance() { if( current < game_states.size() ) current += 1; };

    std::string get_piece_positions() { return game_states[current].piece_positions; };
    void set_piece_positions( std::string positions ) { game_states[current].piece_positions = positions; };
    //STInfo get_info() { return info; };

    // we insert or remove pieces only from the initial board
    void insert_piece( STSquare square, char piece );
    void remove_piece( STSquare square );       // piece used for checking, not necessary for operation

    Board get_board_at( int PlyNr );	/* if PlyNr is -1 return the board from the current state of play */
    void add_ply( Ply new_ply );

	void add_tag_pair( std::string tag, std::string value );

	void set_level_info( std::string new_info ) {};

private:
    //STInfo info;
    std::vector<STGameState> game_states;
    unsigned int current;
    //std::vector<unsigned int> moves;

    //std::vector<Board> board_states;

    Board initial;
    std::vector<Ply> moves;
    Board current_board;
    std::map<std::string,std::string> tag_pairs;

    bool multi_player;
    eLevels level;
    int level_time;
};

#endif // APPMODEL_H
