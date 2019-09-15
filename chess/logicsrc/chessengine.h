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
#include <map>

class ChessGame;
class ChessAppBase;
class STSquare;

//class FilenameChooser;

class PieceValues;

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

	bool toggle_multiplayer();
	void do_demo();

	void arranging_start();
	void arranging_clear();
	void put_piece_on_square( STSquare square, char piece );
	void arrange_turn( eTurns new_turn );
	bool arranging_end( bool canceled );

    void advance();

    STSquare hint();

    void new_game();
    bool can_quit();

    bool open_file( std::string name );
    bool save_file( std::string name );

	void undo();
	void redo();
	void stop_thinking();
	void change_level( eLevels new_level, int time_parameter );

	void init_piece_values();

	void change_piece_values( PieceValues* piece_value );


	void CalculatePawnTable();
	void CalcMaterial();

private:
	ChessGame * model;
	ChessAppBase * app;

	std::map<char,int> piece_values;

    STGameState current_state;
    STGameState arrange_state;
    bool is_arranging;
};

#endif // CHESSENGINE_H
