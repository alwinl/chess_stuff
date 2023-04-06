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

#include "chessengine.h"
#include "chessgame.h"
#include "pgnparser.h"

#include <chrono>
#include <thread>
#include <regex>

#include <iostream>
#include <fstream>

using namespace std;


ChessEngine::ChessEngine()
{
    is_arranging = false;

	piece_values = {
		{ 'Q', 2304 }, // 9 << 8
		{ 'R', 1280 }, // 5 << 8
		{ 'B', 768 },  // 3 << 8
		{ 'N', 768 },  // 3 << 8
		{ 'P', 256 },  // 1 << 8
		{ 'K', 0 },
		{ 'q', -2304 },
		{ 'r', -1280 },
		{ 'b', -768 },
		{ 'n', -768 },
		{ 'p', -256 },
		{ 'k', 0 },
	};

	colours = {
		"rgb(78,154,6)",
		"rgb(0,0,0)",
		"rgb(238,238,236)",
		"rgb(85,87,83)",
	};
}

ChessEngine::~ChessEngine()
{
}


void ChessEngine::new_game( int game_type )
{
    game = ChessGame();


    info.turn = "white";
    info.black = "";
    info.time = "";
    info.level = "";
    info.value = "";
    info.nodes = "";
    info.n_sec = "";
    info.depth = "";
    info.bestline = "";

    multi_player = false;

    current_board = Board();
}




bool ChessEngine::enter_move( uint16_t start_square, uint16_t end_square )
{
    static int test = 0;

    if( (++test) % 2 ) {

        current_board.update_board( Ply( start_square, end_square ) );
        return true;

    } else {
        return false;
    }
    //return true;
}

uint16_t ChessEngine::hint()
{
    uint16_t square = 3 + 8 * 3;


    return square;
}

void ChessEngine::cancel_move( )
{
}




void ChessEngine::arranging_start()
{
    arrange_board = Board( "8/8/8/8/8/8/8/8" );
    is_arranging = true;
}

void ChessEngine::arranging_clear()
{
    arrange_board = Board( "8/8/8/8/8/8/8/8" );
}

void ChessEngine::arrange_add_piece( uint16_t square, char piece )
{
    arrange_board.add_piece( square, piece );
}

void ChessEngine::arrange_remove_piece(uint16_t square )
{
    arrange_board.remove_piece( square );
}

void ChessEngine::arrange_turn( eTurns new_turn )
{
    //arrange_board.set_white_move( new_turn == TURNWHITE );
}

std::string ChessEngine::arrange_to_fen()
{
	return arrange_board.piece_placement();
}

bool ChessEngine::arranging_end( bool canceled )
{
    if( canceled ) {
        is_arranging = false;
        return true;
    }

    if( !arrange_board.is_valid() )
        return false;

    current_board = arrange_board;

    is_arranging = false;
    return true;
}







std::array<char, 64> ChessEngine::get_piece_positions( )
{
	std::array<char, 64> new_positions;
	std::array<Piece,64> piece_positions = is_arranging ? arrange_board.get_pieces() : current_board.get_pieces();

	for( int i=0; i<64; ++i )
		new_positions[i] = piece_positions[i].get_code();

	return new_positions;
}


bool ChessEngine::open_file( std::string filename )
{
	if( filename.empty() )
		return false;

	std::ifstream is( filename.c_str() );

	if( !is.good() )
		return false;

	game = PGNParser().load( is );

	is.close();
	return true;
}

bool ChessEngine::save_file( )
{
	if( game_filename.empty() )
		return false;

	std::ofstream os( game_filename.c_str() );

	if( !os.good() )
		return false;

	PGNParser().save( os, game );

    os.close();

    return true;
}

bool ChessEngine::save_file_as( std::string filename )
{
	if( filename.empty() )
		return false;

	std::ofstream os( filename.c_str() );

	if( !os.good() )
		return false;

	PGNParser().save( os, game );

    game_filename = filename;

    os.close();

    return true;
}

bool ChessEngine::can_quit( )
{
    return false;
}


void ChessEngine::undo()
{

}

void ChessEngine::redo()
{

}

void ChessEngine::stop_thinking()
{

}

bool ChessEngine::set_level_easy()
{
    if( multi_player )
		return false;

	level = EASY;
	info.level = "Easy";

	return true;
}

bool ChessEngine::set_level_timed( int timeout )
{
    if( multi_player )
		return false;

	level = TIMED;
	level_time = timeout;
	info.level = to_string( timeout ) + " sec/move";

	return true;
}

bool ChessEngine::set_level_total_time( int timeout )
{
    if( multi_player )
		return false;

	level = TOTALTIME;
	level_time = timeout;
    info.level = to_string( timeout ) + " min/game";

	return true;
}

bool ChessEngine::set_level_infinite()
{
    if( multi_player )
		return false;

	level = INFINITE;
    info.level = "Infinite";

	return true;
}

bool ChessEngine::set_level_ply_search()
{
	static unsigned int MaxLevel = 6;

    if( multi_player )
		return false;

	level = PLYSEARCH;
    info.level = "Ply-Depth = " + to_string( MaxLevel );

	return true;
}

bool ChessEngine::set_level_mate_search()
{
    if( multi_player )
		return false;

	level = MATESEARCH;
	info.level = "MateSearch";

	return true;
}

bool ChessEngine::set_level_matching()
{
    if( multi_player )
		return false;

	level = MATCHING;
	info.level = "Match users time";

	return true;
}

std::array<std::pair<std::string,std::string>,10> ChessEngine::get_info()
{
	std::array<std::pair<std::string,std::string>,10> std_info =
	{
		std::pair<std::string,std::string>{"Turn", info.turn },
		std::pair<std::string,std::string>{"White", info.white },
		std::pair<std::string,std::string>{"Black", info.black },
		std::pair<std::string,std::string>{"Time", info.time },
		std::pair<std::string,std::string>{"Level", info.level },
		std::pair<std::string,std::string>{"Value", info.value },
		std::pair<std::string,std::string>{"Nodes", info.nodes },
		std::pair<std::string,std::string>{"N/Sec", info.n_sec },
		std::pair<std::string,std::string>{"Depth", info.depth },
		std::pair<std::string,std::string>{"Bestline", info.bestline }
	};

	return std_info;
}

#include <unistd.h>
void ChessEngine::calculate_move()
{
    unsigned int microsecond = 1000000;

    usleep( 5 * microsecond );
}

bool ChessEngine::toggle_multiplayer()
{
    multi_player = !multi_player;

    if( multi_player )
        info.level = "Two Player";

    return multi_player;
}

void ChessEngine::do_demo()
{

}

#if 0
/*  Calculate attack values of a square (importance of a square)  */
int ChessEngine::calc_attack_value( STSquare square )
{
    const unsigned char squarerankvalue[8]     = { 0, 0, 0, 0, 3, 6, 12, 12 };
    const char    distan[8] = { 3, 2, 1, 0, 0, 1, 2, 3 };

    return ( ( squarerankvalue[ square.rank ] * ( materiallevel + 8 ) ) >> 5 ) /*  Rank importance  */
           + max( 0, 8 - 3 * ( distan[ square.rank ] + distan[ square.file ] ) ); /*  centre importance */
}




int bit_count( unsigned char n )
{
    static const char split_lookup[] = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4 };

    return split_lookup[n&0xF] + split_lookup[n>>4];
}



#endif // 0

