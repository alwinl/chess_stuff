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
#include "chessgame.h"
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

    init_piece_values();

}

ChessEngine::~ChessEngine()
{
    delete model;
}

void ChessEngine::set_application_pointer( ChessAppBase* app_init )
{
    app = app_init;
}


void ChessEngine::new_game( )
{
	current_state = make_game_state("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    model->initialise();

    STInfo tmp = model->get_info();


    app->set_piece_positions( current_state.piece_positions );
    app->set_info( tmp );
}




void ChessEngine::do_move( STSquare start_square, STSquare end_square )
{
	FENTranslator translator;
	char piece;

	translator.from_FEN( current_state.piece_positions );

	piece = translator.query_square( start_square );
	if( piece == ' ' )
		return;		/* invalid move */

	/*
	 *	We need to do a bunch more checking here
	 */

	translator.remove_from_square( start_square );

    app->set_piece_positions( translator.to_FEN() );


	app->animate( start_square, end_square, piece );

	translator.add_to_square( end_square, piece );
    app->set_piece_positions( translator.to_FEN() );

	current_state.piece_positions = translator.to_FEN();
	current_state.is_white_move = ! current_state.is_white_move;

	//model->push_state( current_state );
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
	arrange_state = STGameState();

    arrange_state.piece_positions = "/8/8/8/8/8/8/8/8";    // Piece placement in FEN.
    arrange_state.is_white_move = true;             // is it whites next move?

    is_arranging = true;

    app->set_piece_positions( arrange_state.piece_positions );
}

void ChessEngine::arranging_clear()
{
    arrange_state.piece_positions = "/8/8/8/8/8/8/8/8";    // Piece placement in FEN.
    arrange_state.is_white_move = true;             // is it whites next move?

    app->set_piece_positions( arrange_state.piece_positions );
}

void ChessEngine::put_piece_on_square( STSquare square, char piece )
{
	FENTranslator translator;

	if( is_arranging )
		translator.from_FEN( arrange_state.piece_positions );
	else
		translator.from_FEN( current_state.piece_positions );

	if( translator.query_square(square) != ' ' )
		translator.remove_from_square( square );

	if( piece != ' ' )
		translator.add_to_square( square, piece );

	if( is_arranging )
		arrange_state.piece_positions = translator.to_FEN();

    app->set_piece_positions( translator.to_FEN() );
}

void ChessEngine::arrange_turn( eTurns new_turn )
{
	if( new_turn == TURNWHITE )
		cout << "processing turn message: it's now whites turn" << endl;
	else
		cout << "processing turn message: it's now black turn" << endl;

	arrange_state.is_white_move = ( new_turn == TURNWHITE );
}

bool ChessEngine::arranging_end( bool canceled )
{
	if( canceled ) {
		is_arranging = false;
		app->set_piece_positions( current_state.piece_positions );
		return true;
	}

	/* check if the board is valid */
	FENTranslator translator;

	map<STSquare,STPiece> pieces = translator.from_FEN( arrange_state.piece_positions );

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

	current_state.piece_positions = arrange_state.piece_positions;
	current_state.is_white_move = arrange_state.is_white_move;

	is_arranging = false;
	app->set_piece_positions( current_state.piece_positions );
	return true;
}









bool ChessEngine::open_file( std::string name )
{
    if( model->load_game( name ) == -1 ) {    // load the file and build the DS in the model_ member
        return false;
    }

    return true;
}

bool ChessEngine::save_file( std::string name )
{
    if( model->store_game( name ) == -1 ) {
        return false;
    }

    return true;
}

void ChessEngine::advance( )
{
    model->advance();

    STInfo tmp = model->get_info();
    app->set_piece_positions( model->get_piece_positions() );
    app->set_info( tmp );
}


void ChessEngine::init_piece_values()
{
	piece_values.insert( pair<char,int>( 'Q',  2304 ) );		// 9 << 8
	piece_values.insert( pair<char,int>( 'R',  1280 ) );		// 5 << 8
	piece_values.insert( pair<char,int>( 'B',   768 ) );		// 3 << 8
	piece_values.insert( pair<char,int>( 'N',   768 ) );		// 3 << 8
	piece_values.insert( pair<char,int>( 'P',   256 ) );		// 1 << 8
	piece_values.insert( pair<char,int>( 'K',     0 ) );
	piece_values.insert( pair<char,int>( 'q', -2304 ) );
	piece_values.insert( pair<char,int>( 'r', -1280 ) );
	piece_values.insert( pair<char,int>( 'b',  -768 ) );
	piece_values.insert( pair<char,int>( 'n',  -768 ) );
	piece_values.insert( pair<char,int>( 'p',  -256 ) );
	piece_values.insert( pair<char,int>( 'k',    0  ) );
}

void ChessEngine::change_piece_values( PieceValues* piece_value )
{
	PieceValues::STPieceValues values;

    values.QueenValue  = piece_values['Q'] >> 4;
    values.RookValue   = piece_values['R'] >> 4;
    values.BishopValue = piece_values['B'] >> 4;
    values.KnightValue = piece_values['N'] >> 4;
    values.PawnValue   = piece_values['P'] >> 4;

    piece_value->init_piece_values( values );

    if( !piece_value->choose_piece_values( ) )
		return;

	values = piece_value->get_piece_values();

	piece_values['Q'] = values.QueenValue << 4;
	piece_values['R'] = values.RookValue << 4;
	piece_values['B'] = values.BishopValue << 4;
	piece_values['N'] = values.KnightValue << 4;
	piece_values['P'] = values.PawnValue << 4;

	piece_values['q'] = -values.QueenValue << 4;
	piece_values['r'] = -values.RookValue << 4;
	piece_values['b'] = -values.BishopValue << 4;
	piece_values['n'] = -values.KnightValue << 4;
	piece_values['p'] = -values.PawnValue << 4;
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

void ChessEngine::change_level( eLevels new_level, int time_parameter )
{

	if( new_level == TIMED ) {
		cout << "Got " << time_parameter << " seconds per move" << endl;
	}

	if( new_level == TOTALTIME ) {
		cout << "Got " << time_parameter << " minutes per game" << endl;
	}
}


void ChessEngine::CalculatePawnTable()
{
    unsigned char PawnTable[2][8];

    /*  Calculate PawnTable (indicates which squares contain pawns )  */
    for( unsigned char rank = 0; rank < 8; rank++ ) {
        PawnTable[ 0 ][ rank ] = 0;
        PawnTable[ 1 ][ rank ] = 0;
    }

	FENTranslator translator;

	map<STSquare,STPiece> pieces = translator.from_FEN( current_state.piece_positions );

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


	FENTranslator translator;

	map<STSquare,STPiece> pieces = translator.from_FEN( current_state.piece_positions );

	int material = 0;
	int totalMaterial = 0;

    for( map<STSquare,STPiece>::iterator it = pieces.begin(); it != pieces.end(); it++ ) {

		STPiece piece = (*it).second;

		material += piece_values[piece.code];
		totalMaterial += abs( piece_values[piece.code] );

    }
}


bool ChessEngine::toggle_multiplayer()
{
	static bool multi_player = false;

	multi_player = !multi_player;

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

