/*
 * Copyright 2017 Alwin Leerling <alwin@jambo>
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

#ifndef CHESSAPPBASE_H
#define CHESSAPPBASE_H

#include <string>

#include "pods.h"

class TimeInputter;
class PieceValues;
class FilenameChooser;

class ChessEngine;


/** \brief
 */
class ChessAppBase
{
public:
	ChessAppBase( ChessEngine* engine_init );
	virtual ~ChessAppBase();

	/* called from main function */;
	virtual int run(  int argc, char *argv[] ) = 0;

	/* Called from logic */
    virtual void set_piece_positions( std::string FEN_string ) = 0;
    virtual void set_info(  STInfo& info ) = 0;
	virtual void animate( STSquare start_square, STSquare end_square, char piece ) = 0;

    /* called from GUI */
    void undo();
    void redo();
    void stop_thinking();
    char get_piece( STSquare square );
    void change_level( eLevels new_level, int time_parameter );
    void arrange_turn( eTurns new_turn );
    void do_move( STSquare start_square, STSquare end_square );
    void cancel_move();
    void advance();
    STSquare hint();
    void arrange_start();
    void arrange_clear();
    void put_piece_on_square( STSquare square, char piece );
    void arrange_end( bool canceled );
    void new_game();
    bool can_quit();
    bool open_file( std::string name );
    bool save_file( std::string name );
	STPieceValues get_piece_values();
	void set_piece_values( STPieceValues piece_values );

private:
	ChessEngine* engine;
};

#endif // CHESSAPPBASE_H
