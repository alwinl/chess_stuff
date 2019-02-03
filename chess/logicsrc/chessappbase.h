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

class STInfo;
class STSquare;
class STPieceValues;

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

    virtual void set_piece_positions( std::string FEN_string, STInfo& info ) = 0;
    virtual void push_statusbar_text( std::string message ) = 0;
    virtual void message_dialog( std::string message ) = 0;
    virtual STPieceValues edit_piecevalues( STPieceValues& current ) = 0;

    virtual void start_arranging() = 0;
    virtual void end_arranging() = 0;

    virtual std::string open_filename( std::string filename, std::string working_dir ) = 0;
    virtual std::string save_filename( std::string filename, std::string working_dir ) = 0;

    virtual void quit() = 0;

    /* called from GUI */
    void start_move( STSquare square );
    void do_move( STSquare square );
    void cancel_move();
    void select_edit_piece( char piece );
    void advance();

    void arrange_start();
    void arrange_clear();
    void arrange_end( bool canceled );

    void piece_value_changes();
    void new_game();
    void end_app();
    void open_file();
    void save_file();
    void save_as();

private:
	ChessEngine* engine;
};

#endif // CHESSAPPBASE_H
