/*
 * Copyright 2023 Alwin Leerling <dna.leerling@gmail.com>
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

#ifndef CHESSGAME_H
#define CHESSGAME_H

#include <vector>
#include <string>
#include <utility>

#include "gamestate.h"
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

	void load( std::string pgn_string );
	std::string save();

	void add_tag_pair( std::string tag, std::string value );
	void add_ply( Ply ply ) { plys.push_back( ply ); }

	Ply last_ply() { return plys.back(); }

	void visit_tag_pairs( ChessGameVisitorBase* processor );
	void visit_plys( ChessGameVisitorBase* processor );

private:
    GameState initial;
    std::vector<Ply> plys;

    std::vector<std::pair<std::string, std::string> > tag_pairs;

	void add_ply( eColor color, std::string SAN, GameState& current );
	void set_alternate_starting_position();
};

#endif // CHESSGAME_H
