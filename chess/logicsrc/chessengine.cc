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

void ChessEngine::new_game( int game_type )
{
	switch( game_type ) {
	case 1: is_human = { {eColor::white, true}, {eColor::black, false} }; break;
	case 2: is_human = { {eColor::white, false}, {eColor::black, true} }; break;
	case 3: is_human = { {eColor::white, false}, {eColor::black, false} }; break;
	case 4: is_human = { {eColor::white, true}, {eColor::black, true} }; break;
	}

    game = ChessGame();
    current_state = GameState();
}

bool ChessEngine::load_game( std::string filename )
{
	if( filename.empty() )
		return false;

	std::ifstream is( filename.c_str() );

	if( !is.good() )
		return false;

	game.load( string( istreambuf_iterator<char>(is), istreambuf_iterator<char>() ) );

	is.close();
	return true;
}

bool ChessEngine::save_game( std::string filename )
{
	if( filename.empty() && game_filename.empty() )
		return false;

	string tmp_name( filename.empty() ? game_filename : filename );

	std::ofstream os( tmp_name.c_str() );
	if( !os.good() )
		return false;

	os << game.save();

    os.close();

	game_filename = tmp_name;

    return true;
}

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



bool ChessEngine::human_move( uint16_t start_square, uint16_t end_square, char promo_piece )
{
	vector<Ply> plys = current_state.generate_legal_plys();	// grabs all legal moves

	if( plys.empty() ) {// checkmate
		game_over();
		return false;
	}

	auto ply_it = std::find_if( plys.begin(), plys.end(), [start_square, end_square, promo_piece]( Ply ply ) { return ply.check_match( start_square, end_square, promo_piece ); } );
	if( ply_it == plys.end() )
		return false;

	last_ply[ current_state.get_current_colour() ] = (*ply_it).print_LAN();
	game.add_ply( *ply_it );

	current_state = current_state.make( *ply_it );

	return true;
}

bool ChessEngine::AI_move( uint16_t& AI_start_square, uint16_t& AI_end_square, char& AI_piece )
{
    std::vector<Ply> plys = current_state.generate_legal_plys();

	if( plys.empty() ) {// checkmate
		game_over();
		return false;
	}

	if( current_state.get_current_colour() == eColor::white )
		sort( plys.begin(), plys.end(), [this](const Ply& lhs, const Ply& rhs) { return current_state.evaluate_ply( lhs, 3) > current_state.evaluate_ply( rhs, 3); } );
	else
		sort( plys.begin(), plys.end(), [this](const Ply& lhs, const Ply& rhs) { return current_state.evaluate_ply( lhs, 3) < current_state.evaluate_ply( rhs, 3); } );

	Ply chosen_ply( *(plys.begin()) );

	last_ply[ current_state.get_current_colour() ] = chosen_ply.print_LAN();
	game.add_ply( chosen_ply );

	AI_start_square = chosen_ply.square_from();
	AI_end_square =  chosen_ply.square_to();
	AI_piece = Piece( chosen_ply.get_type(), current_state.get_current_colour()).get_code();

	current_state = current_state.make( chosen_ply );

	return true;
}

bool ChessEngine::can_quit( )
{
    return false;
}

void ChessEngine::game_over()
{

}

void ChessEngine::undo()
{

}

void ChessEngine::redo()
{

}

uint16_t ChessEngine::hint()
{
    uint16_t square = 3 + 8 * 3;


    return square;
}

void ChessEngine::stop_thinking()
{

}





bool ChessEngine::set_level_easy()
{
    if( is_multi_player() )
		return false;

	level = EASY;

	return true;
}

bool ChessEngine::set_level_timed( int timeout )
{
    if( is_multi_player() )
		return false;

	level = TIMED;
	level_timed = timeout;

	return true;
}

bool ChessEngine::set_level_total_time( int timeout )
{
    if( is_multi_player() )
		return false;

	level = TOTALTIME;
	level_total_time = timeout;

	return true;
}

bool ChessEngine::set_level_infinite()
{
    if( is_multi_player() )
		return false;

	level = INFINITE;

	return true;
}

bool ChessEngine::set_level_ply_search()
{
    if( is_multi_player() )
		return false;

	level = PLYSEARCH;

	return true;
}

bool ChessEngine::set_level_mate_search()
{
    if( is_multi_player() )
		return false;

	level = MATESEARCH;

	return true;
}

bool ChessEngine::set_level_matching()
{
    if( is_multi_player() )
		return false;

	level = MATCHING;

	return true;
}

void ChessEngine::do_demo()
{

}



std::array<char, 64> ChessEngine::get_piece_positions( )
{
	return is_arranging ? arrange_state.get_position_codes() : current_state.get_position_codes();
}

std::array<std::pair<std::string,std::string>,10> ChessEngine::get_info()
{
	string level_string;

	if( is_multi_player() )
		level_string = "Two Player";
	else
		switch( level ) {
		case eLevels::EASY      : level_string = "Easy"; break;
		case eLevels::TIMED     : level_string = "Timed, " + to_string( level_timed ) + " sec/move"; break;
		case eLevels::TOTALTIME : level_string = "Total Time, " + to_string( level_total_time ) + " min/game"; break;
		case eLevels::INFINITE  : level_string = "Infinite"; break;
		case eLevels::PLYSEARCH : level_string = "Ply-Depth = " + to_string( ply_depth ); break;
		case eLevels::MATESEARCH: level_string = "Mate Search"; break;
		case eLevels::MATCHING  : level_string = "Match users time"; break;
		default: level_string = "";
		}



	std::array<std::pair<std::string,std::string>,10> std_info =
	{
		std::pair<std::string,std::string>{"Turn", (current_state.get_current_colour() == eColor::white) ? "White" : "Black" },
		std::pair<std::string,std::string>{"White", last_ply[eColor::white] },
		std::pair<std::string,std::string>{"Black", last_ply[eColor::black] },
		std::pair<std::string,std::string>{"Time", info.time },
		std::pair<std::string,std::string>{"Level", level_string },
		std::pair<std::string,std::string>{"Value", info.value },
		std::pair<std::string,std::string>{"Nodes", info.nodes },
		std::pair<std::string,std::string>{"N/Sec", info.n_sec },
		std::pair<std::string,std::string>{"Depth", info.depth },
		std::pair<std::string,std::string>{"Bestline", info.bestline }
	};

	return std_info;
}

std::map<char, int> ChessEngine::get_piece_values() const
{
	std::map<char, int> values = {
		{ 'Q', Piece::get_value( Piece::queen) },
		{ 'R', Piece::get_value( Piece::rook) },
		{ 'B', Piece::get_value( Piece::bishop) },
		{ 'N', Piece::get_value( Piece::knight) },
		{ 'P', Piece::get_value( Piece::pawn) },
	};

	return values;
}

bool ChessEngine::set_piece_values( std::map<char, int> new_values )
{
	Piece::set_value( Piece::queen, new_values.at('Q') );
	Piece::set_value( Piece::rook, new_values.at('R') );
	Piece::set_value( Piece::bishop, new_values.at('B') );
	Piece::set_value( Piece::knight, new_values.at('N') );
	Piece::set_value( Piece::pawn, new_values.at('P') );

	return true;
}



int ChessEngine::evaluate_ply( const Ply& ply, int depth, eColor color ) const
{
	return alpha_beta( current_state.make( ply ), numeric_limits<int>::min(), numeric_limits<int>::max(), depth, color );
}

/*
 *	Alpha is the best value that the maximizer currently can guarantee at that level or above.
 *	Beta is the best value that the minimizer currently can guarantee at that level or below.
 */
int ChessEngine::alpha_beta( GameState state, int alpha, int beta, int depth_left, eColor color ) const
{
	if( ! depth_left )
		return state.evaluate();

	vector<Ply> plys = state.generate_legal_plys();	// grabs all legal moves

	int best_score;

	if( color == eColor::white ) {		// maximiser
		best_score = numeric_limits<int>::min();
		for( Ply& ply : plys ) {
			GameState new_state = state.make(ply);
			best_score = max( best_score, alpha_beta( new_state, alpha, beta, depth_left - 1, eColor::black ) );
			alpha = max( alpha, best_score );

			if( beta <= alpha )
				break;
		}

	} else {					// minimiser
		best_score = numeric_limits<int>::max();
		for( Ply& ply : plys ) {
			GameState new_state = state.make(ply);
			best_score = min( best_score, alpha_beta( new_state, alpha, beta, depth_left - 1, eColor::white ) );
			beta = min( beta, best_score );

			if( beta <= alpha )
				break;
		}
	}

	return best_score;
}

