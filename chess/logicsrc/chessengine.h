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
class PresentationInterface;
class STSquare;

class TimeInputter;
class FilenameChooser;

class PieceValues;

#include "../ADTsrc/pods.h"
#include "colourchooser.h"

/** \brief
 */
class ChessEngine
{
public:
    /** Default constructor */
    ChessEngine();
    ~ChessEngine();

    void set_presentation_pointer( PresentationInterface* presentation_init );

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

    bool open_file( );
    bool save_file( );
    bool save_file_as( );

	void undo();
	void redo();
	void stop_thinking();
	void change_level( eLevels new_level );

	void init_piece_values();

	void change_piece_values( );




	void CalculatePawnTable();
	void CalcMaterial();

	void init_colour_chooser( ColourChooser * object ) { colour_chooser = object; }
	void init_time_inputter( TimeInputter * object ) { time_inputter = object; }
	void init_piece_value_object( PieceValues * object ) { piece_values_object = object; }
	void init_filename_chooser( FilenameChooser * object ) { filename_chooser = object; }

	ColourChooser::STColours get_colours() { return colour_chooser->get_colours(); };
	bool choose_colours();

private:
	ChessGame * model;
	PresentationInterface* presenter;

    ColourChooser * colour_chooser;
    TimeInputter * time_inputter;
    PieceValues * piece_values_object;
    FilenameChooser * filename_chooser;


	std::map<char,int> piece_values;

    STGameState current_state;
    STGameState arrange_state;
    bool is_arranging;
};

#endif // CHESSENGINE_H
