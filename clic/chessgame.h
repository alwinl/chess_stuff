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

#include "move.h"
#include "board.h"
#include "display.h"
#include "piece.h"

class ChessGame
{
public:
	int run( int argc, char *argv[] );

private:
	Display disp;
	eColor current_player = white;
	unsigned int gametype = -1;
	bool is_human[2];
	Board board;
	std::vector<Move> game_moves;

	bool setup();
	bool game_loop();

	bool input_move( eColor player, std::vector<Move> moves );
	bool make_move( std::vector<Move> moves );
	void apply_move( Move the_move );
};

#endif // CHESSGAME_H
