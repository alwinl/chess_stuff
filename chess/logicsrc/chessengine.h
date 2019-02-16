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

#ifndef CHESSENGINE_H
#define CHESSENGINE_H

#include <string>

class AppModel;
class ChessAppBase;
class STSquare;

//class FilenameChooser;

#include "pods.h"

/** \brief
 */
class ChessEngine
{
public:
    /** Default constructor */
    ChessEngine();
    ~ChessEngine();

    void set_application_pointer( ChessAppBase* app );

    void do_move( STSquare start_square, STSquare end_square );
    void cancel_move();

	void arranging_start();
	void arranging_clear();
	void put_piece_on_square( STSquare square, char piece );
	void arranging_end( bool canceled );

    void advance();

    STSquare hint();

    void new_game();
    bool can_quit();

    bool open_file( std::string name );
    bool save_file( std::string name );

	void undo();
	void redo();
	void stop_thinking();
	char get_piece( STSquare square );
	void change_level( eLevels new_level, int time_parameter );
	void arrange_turn( eTurns new_turn );

	STPieceValues get_piece_values() { return current; };
	void set_piece_values( STPieceValues piece_values );


protected:

private:
	AppModel * model;
	ChessAppBase * app;
    std::string filename;

    STPieceValues current;

    STGameState arrange_state;
    bool is_arranging;
};

#endif // CHESSENGINE_H
