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
#include "gamestate.h"

/** \brief
 */
class ChessEngine
{
public:
    ChessEngine();

    bool human_move( uint16_t start_square, uint16_t end_square, char promo_piece = ' ' );
    void calculate_move();

	bool toggle_multiplayer();
	void do_demo();
	bool get_next_ply( Ply& ply ) { return false; };

	void arranging_start();
	void arrange_add_piece( uint16_t square, char piece ) { if( is_arranging ) arrange_state.set_piece( square, piece ); }
	void arrange_remove_piece(uint16_t square ) { if( is_arranging ) arrange_state.set_piece( square, ' ' ); }
	void arrange_set_turn( eColor active_color ) { if( is_arranging ) arrange_state.set_active_color( active_color ); }
	void arrange_set_castle_rights( std::string castle_rights ) { if( is_arranging ) arrange_state.set_castle_rights( castle_rights ); }
	void arrange_set_ep_square( uint16_t square ) { if( is_arranging ) arrange_state.set_ep_square( square ); }
	void arrange_set_halvemoves( uint16_t number ) { if( is_arranging ) arrange_state.set_halvemoves( number ); }
	void arrange_set_fullmoves( uint16_t number ) { if( is_arranging ) arrange_state.set_fullmoves( number ); }
	std::string arrange_to_fen() { return arrange_state.FEN(); }
	bool arranging_end( bool canceled );
	bool in_edit_mode() { return is_arranging; }

    uint16_t hint();

    void new_game( int game_type );
    bool can_quit();

    bool open_file( std::string filename );
    bool save_file( std::string filename );

	void undo();
	void redo();
	void stop_thinking();

	bool set_level_easy();
	bool set_level_timed( int timeout );
	bool set_level_total_time( int timeout );
	bool set_level_infinite();
	bool set_level_ply_search();
	bool set_level_mate_search();
	bool set_level_matching();

	std::array<std::pair<std::string,std::string>,10> get_info();
	std::array<char, 64> get_piece_positions( );

	std::array<std::string,4> get_colour_values() const { return colours; };
	bool set_colour_values( std::array<std::string,4> new_colours ) { colours = new_colours; return true; };

	std::map<char, int> get_piece_values() const { return piece_values; };
	bool set_piece_values( std::map<char, int> new_values ) { piece_values = new_values; return true; };

private:
	ChessGame game;

	GameState current_state;
	GameState arrange_state;
    STInfo info;

    std::string game_filename;
    bool is_arranging = false;
    bool multi_player = false;
    eLevels level;
    int level_total_time = 60;
    int level_timed = 120;
    std::array<std::string,4> colours;
	std::map<char, int> piece_values;


	int evaluate_ply( const Ply& ply, int depth_left, eColor color ) const;
	int alpha_beta( GameState& state, int alpha, int beta, int depth_left, eColor color ) const;
};

#endif // CHESSENGINE_H
