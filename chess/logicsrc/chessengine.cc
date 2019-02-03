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
#include "appmodel.h"
#include "chessappbase.h"

using namespace std;


ChessEngine::ChessEngine()
{
    model = new AppModel;
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

void ChessEngine::start_move( STSquare square )
{
    char piece = model->get_piece( square );

    // put in checking code, is there actually a piece on this square and is it the right colour
    // if checking succeeds store the fact that the user has picked up a piece
    app->set_drag_piece( piece );
}

void ChessEngine::do_move( STSquare square )
{
    app->set_drag_piece( ' ' );
}

void ChessEngine::cancel_move( )
{
    app->set_drag_piece( ' ' );

}

void ChessEngine::select_edit_piece( char piece )
{
    app->set_drag_piece( piece );
}


void ChessEngine::arranging_start()
{
	app->start_arranging();
}

void ChessEngine::arranging_clear()
{

}

void ChessEngine::arranging_end( bool canceled )
{
	app->end_arranging();
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
    app->set_piece_positions( model->get_piece_positions(), tmp );
}

void ChessEngine::new_game( )
{
    filename.clear();
    model->initialise();

    STInfo tmp = model->get_info();
    app->set_piece_positions( model->get_piece_positions(), tmp );

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
