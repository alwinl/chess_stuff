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

#include "../logicsrc/pods.h"
//class STInfo;
//class STSquare;
//class STPieceValues;

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
    virtual STPieceValues edit_piecevalues( STPieceValues& current ) = 0;
	virtual void animate( STSquare start_square, STSquare end_square, char piece ) = 0;
	virtual void flash_square( STSquare square ) = 0;

	virtual TimeInputter* get_time_inputter() = 0;
	virtual PieceValues * get_piece_valuer() = 0;

	virtual FilenameChooser * get_openfile_chooser() = 0;
	virtual FilenameChooser * get_savefile_chooser() = 0;

    virtual void quit() = 0;

    /* called from GUI */
    void undo();
    void redo();
    void stop_thinking();
    char get_piece( STSquare square );
    void change_level( eLevels new_level );
    void arrange_turn( eTurns new_turn );

    void do_move( STSquare start_square, STSquare end_square );
    void cancel_move();
    void advance();
    void hint();

    void arrange_start();
    void arrange_clear();
//    void remove_piece_from_square( STSquare square );
    void put_piece_on_square( STSquare square, char piece );
    void arrange_end( bool canceled );

    void piece_value_changes();
    void new_game();
    void end_app();

    std::string open_file();
    std::string save_file();
    std::string save_as();

private:
	ChessEngine* engine;
};

#endif // CHESSAPPBASE_H
