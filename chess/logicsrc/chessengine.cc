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

#include "fentranslator.h"

using namespace std;


ChessEngine::ChessEngine()
{
    model = new AppModel;

    is_arranging = false;
    //ctor
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

	app->start_arranging();

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

	app->end_arranging();

	app->set_piece_positions( model->get_piece_positions() );
}









void ChessEngine::open_file( )
{
    string filename = app->open_filename( "", "~/" );

    if( filename.empty() )
        return;

    if( model->load_game( filename ) == -1 ) {    // load the file and build the DS in the model_ member
        app->message_dialog( "Error restoring game." );
        return;
    }

    app->push_statusbar_text( string("Opened ") + filename );
}

void ChessEngine::save_file( )
{
    if( filename.empty() ) {
        save_as( );
        return;
    }

    if( model->store_game( filename ) == -1 ) {
        app->message_dialog( "Error saving game. Try Save As" );
        return;
    }

    app->push_statusbar_text( string( "Saved " ) + filename );
}

void ChessEngine::save_as( )
{
    string temp_name = app->save_filename( filename, "~/" );

    if( temp_name.find( ".chess") == string::npos )     // no .chess added
        temp_name += string(".chess");

    if( model->store_game( temp_name ) == -1 ) {
        app->message_dialog( "Error saving game." );
        return;
    }

    filename = temp_name;
    app->push_statusbar_text( string( "Saved " ) + filename );
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

    app->push_statusbar_text( string("New game") );
}

void ChessEngine::piece_value_changes( )
{
    STPieceValues current;

    app->edit_piecevalues( current );
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

}

void ChessEngine::arrange_turn( eTurns new_turn )
{

}
