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
#include <array>

#include "pods.h"

#include "chessgame.h"
#include "board.h"

//class STSquare;

/** \brief
 */
class ChessEngine
{
public:
    /** Default constructor */
    ChessEngine();
    ~ChessEngine();

    bool enter_move( STSquare start_square, STSquare end_square );
    void cancel_move();
    void calculate_move();

	bool toggle_multiplayer();
	void do_demo();
	bool get_next_ply( Ply& ply ) { return false; };

	void arranging_start();
	void arranging_clear();
	void arrange_add_piece( STSquare square, char piece );
	void arrange_remove_piece( STSquare square );
	void arrange_turn( eTurns new_turn );
	bool arranging_end( bool canceled );
	bool in_edit_mode() { return is_arranging; }

    STSquare hint();

    void new_game();
    bool can_quit();

    bool open_file( std::string filename );
    bool save_file( );
    bool save_file_as( std::string filename );

	void undo();
	void redo();
	void stop_thinking();

	STInfo get_info() { return info; }
	std::map<STSquare,STPiece> get_piece_positions( );

	std::string arrange_to_fen();

	int get_timed_level_value() { return 120; };
	int get_total_time_level_value() { return 60; };

	bool set_level_easy();
	bool set_level_timed( int timeout );
	bool set_level_total_time( int timeout );
	bool set_level_infinite();
	bool set_level_ply_search();
	bool set_level_mate_search();
	bool set_level_matching();


	void CalculatePawnTable();
	void CalcMaterial();

	std::array<std::string,4> get_colour_values() const { return colours; };
	bool set_colour_values( std::array<std::string,4> new_colours ) { colours = new_colours; return true; };

	std::map<char, int> get_piece_values() const { return piece_values; };
	bool set_piece_values( std::map<char, int> new_values ) { piece_values = new_values; return true; };

private:
	ChessGame game;

	Board current_board;
	Board arrange_board;
    STInfo info;

    std::string game_filename;

    STGameState last_state;
    bool is_arranging;
    bool multi_player;
    eLevels level;
    int level_time;

    std::array<std::string,4> colours;
	std::map<char, int> piece_values;


};

#endif // CHESSENGINE_H
