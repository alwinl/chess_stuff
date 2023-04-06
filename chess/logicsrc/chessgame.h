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
#include <map>

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

	void add_tag_pair( std::string tag, std::string value );
    void add_white_move( unsigned int moveno, std::string the_move );
    void add_black_move( unsigned int moveno, std::string the_move );
    void add_comment( std::string the_comment );


	//std::string get_tag_value( std::string key );

private:
    Board initial;
    std::vector<Ply> moves;
	eColor current_player = white;
	bool is_human[2];

    std::map<std::string, std::string> tag_pairs;

	void set_alternate_starting_position();

//    bool is_white_move;             // is it whites next move?
//
//    bool white_can_castle_kingside;
//    bool white_can_castle_queenside;
//    bool black_can_castle_kingside;
//    bool black_can_castle_queenside;
//
//    STSquare en_passant_target;		//  If a pawn has just made a two-square move, this is the position "behind" the pawn
//
//    int halfmove_clock;             // Number of half moves since last capture or pawn advance
//    int fullmove_number;             // The number of the move, start at one increment after black move


};

#endif // APPMODEL_H
