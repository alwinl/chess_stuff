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

#include <chrono>
#include <thread>
#include <regex>

#include <iostream>
#include <fstream>

using namespace std;


ChessEngine::ChessEngine()
{
    is_arranging = false;
 // 9 << 8 // 5 << 8  // 3 << 8  // 3 << 8  // 1 << 8
	piece_values = {
		{ 'Q', 2304 },
		{ 'R', 1280 },
		{ 'B', 768 },
		{ 'N', 768 },
		{ 'P', 256 },
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

    current_state = GameState();
}




bool ChessEngine::human_move( uint16_t start_square, uint16_t end_square, char promo_piece )
{
	vector<Ply> plys = current_state.generate_legal_plys();	// grabs all legal moves

	if( plys.empty() ) // checkmate
		return false;

	auto ply_it = std::find_if( plys.begin(), plys.end(), [start_square, end_square, promo_piece]( Ply ply ) { return ply.check_match( start_square, end_square, promo_piece ); } );
	if( ply_it == plys.end() )
		return false;

	current_state = current_state.make( *ply_it );

	return true;
}

uint16_t ChessEngine::hint()
{
    uint16_t square = 3 + 8 * 3;


    return square;
}


/**-----------------------------------------------------------------------------
 *	State editing (create a custom state)
 */
void ChessEngine::arranging_start()
{
    arrange_state = GameState( "8/8/8/8/8/8/8/8 w KQkq - 0 1" );
    is_arranging = true;
}

bool ChessEngine::arranging_end( bool canceled )
{
    if( canceled ) {
        is_arranging = false;
        return true;
    }

    if( !arrange_state.is_valid() )
        return false;

    current_state = arrange_state;

    is_arranging = false;
    return true;
}

/**-----------------------------------------------------------------------------
 *	Persistance
 */
bool ChessEngine::open_file( std::string filename )
{
	if( filename.empty() )
		return false;

	std::ifstream is( filename.c_str() );

	if( !is.good() )
		return false;

	game.load_game( string( istreambuf_iterator<char>(is), istreambuf_iterator<char>() ) );

	is.close();
	return true;
}

bool ChessEngine::save_file( std::string filename )
{
	if( filename.empty() && game_filename.empty() )
		return false;

	string tmp_name( filename.empty() ? game_filename : filename );

	std::ofstream os( tmp_name.c_str() );
	if( !os.good() )
		return false;

	os << game.save_game();

    os.close();

	game_filename = tmp_name;

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
	level_timed = timeout;
	info.level = to_string( timeout ) + " sec/move";

	return true;
}

bool ChessEngine::set_level_total_time( int timeout )
{
    if( multi_player )
		return false;

	level = TOTALTIME;
	level_total_time = timeout;
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



std::array<char, 64> ChessEngine::get_piece_positions( )
{
	std::array<char, 64> new_positions;
	std::array<Piece,64> piece_positions = is_arranging ? arrange_state.get_pieces() : current_state.get_pieces();

	for( int i=0; i<64; ++i )
		new_positions[i] = piece_positions[i].get_code();

	return new_positions;
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


/*
 *	Alpha is the best value that the maximizer currently can guarantee at that level or above.
 *	Beta is the best value that the minimizer currently can guarantee at that level or below.
 */
int ChessEngine::alpha_beta( GameState& state, int alpha, int beta, int depth_left, eColor color ) const
{
	if( ! depth_left )
		return state.evaluate();

	vector<Ply> plys = state.generate_legal_plys();	// grabs all legal moves

	int best_score;

	if( color == white ) {		// maximiser
		best_score = numeric_limits<int>::min();
		for( Ply& ply : plys ) {
			GameState new_state = state.make(ply);
			best_score = max( best_score, alpha_beta( new_state, alpha, beta, depth_left - 1, black ) );
			alpha = max( alpha, best_score );

			if( beta <= alpha )
				break;
		}

	} else {					// minimiser
		best_score = numeric_limits<int>::max();
		for( Ply& ply : plys ) {
			GameState new_state = state.make(ply);
			best_score = min( best_score, alpha_beta( new_state, alpha, beta, depth_left - 1, white ) );
			beta = min( beta, best_score );

			if( beta <= alpha )
				break;
		}
	}

	return best_score;
}

int ChessEngine::evaluate_ply( const Ply& ply, int depth, eColor color ) const
{
	GameState state_to_evaluate = current_state.make( ply );

	return alpha_beta( state_to_evaluate, numeric_limits<int>::min(), numeric_limits<int>::max(), depth, color );
}
