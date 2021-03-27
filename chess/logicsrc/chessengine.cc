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

 //#include <unistd.h>

#include <chrono>
#include <thread>
#include <regex>

#include "chessengine.h"
#include "../ADTsrc/chessgame.h"
#include "chessappbase.h"
#include "timeinputter.h"
#include "piecevalues.h"
#include "filenamechooser.h"

#include "fentranslator.h"

#include <iostream>

using namespace std;


ChessEngine::ChessEngine()
{
    model = new ChessGame;

    is_arranging = false;

    colour_chooser = nullptr;
    time_inputter = nullptr;
    piece_values_object = nullptr;
    filename_chooser = nullptr;
}

ChessEngine::~ChessEngine()
{
    delete model;

}


void ChessEngine::new_game( )
{
	filename_chooser->new_file();

	//current_state = make_game_state("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    model->initialise();


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

	current_board.standard_opening_board();
}




bool ChessEngine::enter_move( STSquare start_square, STSquare end_square )
{
	return true;
}

STSquare ChessEngine::hint()
{
	STSquare square = make_square( 3, 3 );


	return square;
}

void ChessEngine::cancel_move( )
{
}

void ChessEngine::arranging_start()
{
	arrange_board.clear_all();
	//arrange_state = STGameState();

    //arrange_state.piece_positions = "/8/8/8/8/8/8/8/8";    // Piece placement in FEN.
    //arrange_state.is_white_move = true;             // is it whites next move?

    is_arranging = true;
}

void ChessEngine::arranging_clear()
{
	arrange_board.clear_all();
    //arrange_state.piece_positions = "/8/8/8/8/8/8/8/8";    // Piece placement in FEN.
    //arrange_state.is_white_move = true;             // is it whites next move?
}

void ChessEngine::put_piece_on_square( STSquare square, char piece )
{

	STPiece new_piece;

	new_piece.code = piece;
	new_piece.is_dragging = false;
	new_piece.is_white = true; /// not true but I don't think its used t the moment

	if( is_arranging )
		arrange_board.add_piece( square, new_piece );
	else
		current_board.add_piece( square, new_piece );

	#if 0
	FENTranslator translator;

	std::map<STSquare,STPiece> position;

	if( is_arranging )
		FENTranslator::from_FEN( arrange_state.piece_positions );
	else
		FENTranslator::from_FEN( current_state.piece_positions );

	last_state.piece_positions = translator.to_FEN();

	if( translator.query_square(square) != ' ' )
		translator.remove_from_square( square );

	if( piece != ' ' )
		translator.add_to_square( square, piece );

	if( is_arranging )
		arrange_state.piece_positions = translator.to_FEN();
	else
		current_state.piece_positions = translator.to_FEN();
	#endif // 0
}

void ChessEngine::arrange_turn( eTurns new_turn )
{
	if( new_turn == TURNWHITE )
		cout << "processing turn message: it's now whites turn" << endl;
	else
		cout << "processing turn message: it's now black turn" << endl;

	//arrange_state.is_white_move = ( new_turn == TURNWHITE );
	arrange_board.set_white_move( new_turn == TURNWHITE );
}

bool ChessEngine::arranging_end( bool canceled )
{
	if( canceled ) {
		is_arranging = false;
		return true;
	}

	/* check if the board is valid */
	//FENTranslator translator;

	map<STSquare,STPiece> pieces = arrange_board.get_pieces();
	//map<STSquare,STPiece> pieces = translator.from_FEN( arrange_state.piece_positions );

    int KingCount[2]  = { 0, 0 };
    int TotalCount[2] = { 0, 0 };

    for( map<STSquare,STPiece>::iterator it = pieces.begin(); it != pieces.end(); it++ ) {

		int colour_idx = (*it).second.is_white ? 0 : 1;

		TotalCount[ colour_idx ]++;

		if( string("Kk").find( (*it).second.code ) != std::string::npos )
			KingCount[ colour_idx ]++;
    }

    bool valid = ( TotalCount[ 0 ] <= 16 ) && ( KingCount[ 0 ] == 1 )
			&& ( TotalCount[ 1 ] <= 16 ) && ( KingCount[ 1 ] == 1 );

	if( !valid )
		return false;

	//current_state.piece_positions = arrange_state.piece_positions;
	//current_state.is_white_move = arrange_state.is_white_move;

	current_board = arrange_board;

	is_arranging = false;
	return true;
}







std::map<STSquare,STPiece> ChessEngine::get_piece_positions( )
{
	return is_arranging ? arrange_board.get_pieces() : current_board.get_pieces();


	//FENTranslator::from_FEN(arrange_state.piece_positions) : FENTranslator::from_FEN(current_state.piece_positions);
}


bool ChessEngine::open_file( )
{

	std::string open_name = filename_chooser->load_file();

	if( open_name.empty() )
		return false;

    if( model->load_game( open_name ) == -1 ) {    // load the file and build the DS in the model_ member
        return false;
    }

    return true;
}

bool ChessEngine::save_file( )
{
	std::string save_name = filename_chooser->save_file();

	if( save_name.empty() )
		return false;

    if( model->store_game( save_name ) == -1 ) {
        return false;
    }

    return true;
}

bool ChessEngine::save_file_as( )
{
	std::string save_name = filename_chooser->save_file_as();

	if( save_name.empty() )
		return false;

    if( model->store_game( save_name ) == -1 ) {
        return false;
    }

    return true;
}

void ChessEngine::advance( )
{
    model->advance();
}

void ChessEngine::change_piece_values( )
{
    piece_values_object->choose_piece_values( );
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

void ChessEngine::change_level( eLevels new_level )
{
	if( multi_player ) {
		model->set_level_info( "Two Player" );
		return;
	}

	switch( new_level ) {
	case EASY:
		model->set_level_info( "Easy" );
		break;

	case TIMED:
		if( !time_inputter->time_per_move( 120 ) )
			return;

		level_time = time_inputter->get_time();

		// model->set_level_info(      "%1.0f sec / move", level_time
		break;

	case TOTALTIME:
	    if( !time_inputter->total_game_time( 60 ) )
			return;

		level_time = time_inputter->get_time();
		// model->set_level_info(      "%2.2f min / game", AverageTime )
		break;

	case INFINITE:
		model->set_level_info( "Infinite" );
		break;

	case PLAYSEARCH:
		//sprintf( info_string, "Ply-Depth = %d", MaxLevel );
		break;

	case MATESEARCH:
		model->set_level_info( "MateSearch" );
		break;

	case MATCHING:
		model->set_level_info( "Match users time" );
		break;

	default:
		break;

	}

	if( new_level)
	level = new_level;

	// new level is (eLevels)level


/*
	model->set_level_info(
    if( MultiMove ) {
        TInfo->SetLevelText( "Two Player" );
        return;
    }

    char info_string[80];

    switch( Level ) {
    case normal       : sprintf( info_string, "%1.0f sec / move", AverageTime ); break;
    case fullgametime : sprintf( info_string, "%2.2f min / game", AverageTime ); break;
    case easygame     : strcpy( info_string, "Easy" );                           break;
    case infinite     : strcpy( info_string, "Infinite" );                       break;
    case plysearch    : sprintf( info_string, "Ply-Depth = %d", MaxLevel );      break;
    case matesearch   : strcpy( info_string, "MateSearch" );                     break;
    case matching     : strcpy( info_string, "Match users time" );               break;
    }

    TInfo->SetLevelText( info_string );
*/

}

bool ChessEngine::choose_colours()
{
    return colour_chooser->choose_colours( );

}


void ChessEngine::CalculatePawnTable()
{
    unsigned char PawnTable[2][8];

    /*  Calculate PawnTable (indicates which squares contain pawns )  */
    for( unsigned char rank = 0; rank < 8; rank++ ) {
        PawnTable[ 0 ][ rank ] = 0;
        PawnTable[ 1 ][ rank ] = 0;
    }

	//FENTranslator translator;

	//map<STSquare,STPiece> pieces = translator.from_FEN( current_state.piece_positions );
	map<STSquare,STPiece> pieces = current_board.get_pieces();

    for( map<STSquare,STPiece>::iterator it = pieces.begin(); it != pieces.end(); it++ ) {

    	STSquare square = (*it).first;
		STPiece piece = (*it).second;

    	if( piece.code == 'P' ) {		// white pawn
    		PawnTable[0][square.rank] |= (1 << square.file);
    	} else if( piece.code == 'p' ) {		// black pawn
    		PawnTable[1][7- square.rank] |= (1 << square.file);
    	}
    }

}

void ChessEngine::CalcMaterial()
{


	//FENTranslator translator;

	//map<STSquare,STPiece> pieces = translator.from_FEN( current_state.piece_positions );
	map<STSquare,STPiece> pieces = current_board.get_pieces();

	int material = 0;
	int totalMaterial = 0;

    for( map<STSquare,STPiece>::iterator it = pieces.begin(); it != pieces.end(); it++ ) {

		STPiece piece = (*it).second;



		material += piece_values_object->get_piece_value( piece.code );
		totalMaterial += abs( piece_values_object->get_piece_value( piece.code ) );

    }
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
		model->set_level_info( "Two Player" );

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

