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

//#include "pods.h"

#include "board.h"
#include "ply.h"

class ChessGameVisitorBase
{
public:
	virtual void process_tag_pair( std::string tag, std::string value ) = 0;
	virtual void process_ply( Ply ply ) = 0;
};

class ChessGame
{
public:
	ChessGame( );

	void load_game( std::string pgn_string );
	std::string save_game();

	void add_tag_pair( std::string tag, std::string value );
	void add_ply( eColor color, std::string SAN, Board& current );

	void visit_tag_pairs( ChessGameVisitorBase* processor );
	void visit_plys( ChessGameVisitorBase* processor );

private:
    Board initial;
    std::vector<Ply> plys;

    std::vector<std::pair<std::string, std::string> > tag_pairs;

	void set_alternate_starting_position();

//	eColor current_player = white;
//	bool is_human[2];
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
