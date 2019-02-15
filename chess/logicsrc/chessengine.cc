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

#include "chessengine.h"
#include "appmodel.h"
#include "chessappbase.h"
#include "timeinputter.h"
#include "piecevalues.h"
#include "filenamechooser.h"

#include "fentranslator.h"

#include <iostream>

using namespace std;


ChessEngine::ChessEngine()
{
    model = new AppModel;

    is_arranging = false;
}

ChessEngine::~ChessEngine()
{
    delete model;
}

void ChessEngine::set_application_pointer( ChessAppBase* app_init )
{
    app = app_init;
}





void ChessEngine::do_move( STSquare start_square, STSquare end_square )
{
	FENTranslator translator;
	char piece;

	translator.from_FEN( model->get_piece_positions() );

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

	model->set_piece_positions( translator.to_FEN() );

}

void ChessEngine::hint()
{
	STSquare square = make_square( 3, 3 );

	app->flash_square( square );
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

void ChessEngine::put_piece_on_square( STSquare square, char piece )
{
	FENTranslator translator;

	if( is_arranging )
		translator.from_FEN( arrange_state.piece_positions );
	else
		translator.from_FEN( model->get_piece_positions() );

	if( translator.query_square(square) != ' ' )
		translator.remove_from_square( square );

	if( piece != ' ' )
		translator.add_to_square( square, piece );

	if( is_arranging )
		arrange_state.piece_positions = translator.to_FEN();

    app->set_piece_positions( translator.to_FEN() );
}

void ChessEngine::arranging_clear()
{
    arrange_state.piece_positions = "/8/8/8/8/8/8/8/8";    // Piece placement in FEN.
    arrange_state.is_white_move = true;             // is it whites next move?

    app->set_piece_positions( arrange_state.piece_positions );
}

void ChessEngine::arranging_end( bool canceled )
{
	is_arranging = false;

	app->set_piece_positions( model->get_piece_positions() );
}









std::string ChessEngine::open_file( )
{
	FilenameChooser * open_chooser = app->get_openfile_chooser();

	pair<bool,string> result = open_chooser->get_filename( "", "~/" );

	if( !result.first )
		return "";

    if( model->load_game( result.second ) == -1 ) {    // load the file and build the DS in the model_ member
        return "";
    }

    return result.second;
}

std::string ChessEngine::save_file( )
{
    if( filename.empty() )
        return save_as( );

    if( model->store_game( filename ) == -1 ) {
        return "";
    }

    return filename;
}

std::string ChessEngine::save_as( )
{
	FilenameChooser * save_chooser = app->get_savefile_chooser();

	pair<bool,string> result = save_chooser->get_filename( filename, "~/" );

	if( !result.first )
		return "";

    if( result.second.find( ".chess") == string::npos )     // no .chess added
        result.second += string(".chess");

    if( model->store_game( result.second ) == -1 ) {
        return "";
    }

    filename = result.second;
    return filename;
}

void ChessEngine::advance( )
{
    model->advance();

    STInfo tmp = model->get_info();
    app->set_piece_positions( model->get_piece_positions() );
    app->set_info( tmp );
}

void ChessEngine::new_game( )
{
    filename.clear();
    model->initialise();

    STInfo tmp = model->get_info();
    app->set_piece_positions( model->get_piece_positions() );
    app->set_info( tmp );
}

void ChessEngine::piece_value_changes( )
{

    PieceValues * piece_values = app->get_piece_valuer();

    if( piece_values->get_new_piece_values( current) ) {

		cout << "Values have changed" << endl;
		cout << "Queen: " << current.QueenValue << endl;
		cout << "Rook: " << current.RookValue << endl;
		cout << "Knight: " << current.KnightValue << endl;
		cout << "Bishop: " << current.BishopValue << endl;
		cout << "Pawn: " << current.PawnValue << endl;

    } else
		cout << "same old same old" << endl;

    //app->edit_piecevalues( current );
}

void ChessEngine::quit( )
{
    app->quit();
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

char ChessEngine::get_piece( STSquare square )
{
	FENTranslator translator;

	if( is_arranging)
		translator.from_FEN( arrange_state.piece_positions );
	else
		translator.from_FEN( model->get_piece_positions() );

	return translator.query_square(square);
}

void ChessEngine::change_level( eLevels new_level )
{

	if( new_level == TIMED ) {
	    std::pair<bool,int> retval = app->get_time_inputter()->time_per_move( 120 );

		if( ! retval.first )
			return;

		int sec_per_move = retval.second;
		// now we need to do something with sec_per_move.

		cout << "Got " << sec_per_move << " seconds per move" << endl;
	}

	if( new_level == TOTALTIME ) {
	    std::pair<bool,int> retval = app->get_time_inputter()->total_game_time( 60 );

		if( ! retval.first )
			return;

		int minutes_per_game = retval.second;
		// now we need to do something with sec_per_move.

		cout << "Got " << minutes_per_game << " minutes per game" << endl;

	}
}

void ChessEngine::arrange_turn( eTurns new_turn )
{

}
