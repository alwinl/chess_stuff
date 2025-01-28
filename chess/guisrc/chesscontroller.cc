/*
 * Copyright 2022 Alwin Leerling <dna.leerling@gmail.com>
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
 */

#include "chesscontroller.h"

#include "chessboard.h"
#include "dialogs.h"

#include "chessengine.h"

#include <iostream>

int main( int argc, char *argv[] )
{
    return chess_gui::ChessController().run( argc, argv );
}

namespace chess_gui {

ChessController::ChessController( ) : Gtk::Application( "net.dnatechnologies.chess" )
{
	Glib::set_application_name("GTKmm Chess");

	engine = std::make_shared<ChessEngine>();
}

void ChessController::bind_actions()
{
    add_action("new",     sigc::mem_fun( *this, &ChessController::on_action_new ) );
    add_action("restore", sigc::mem_fun( *this, &ChessController::on_action_open ) );
    add_action("save",    sigc::mem_fun( *this, &ChessController::on_action_save ) );
    add_action("saveas",  sigc::mem_fun( *this, &ChessController::on_action_save_as ) );
    add_action("play",    sigc::mem_fun( *this, &ChessController::on_action_play ) );
    add_action("hint",    sigc::mem_fun( *this, &ChessController::on_action_hint ) );
    add_action("quit",    sigc::mem_fun( *this, &ChessController::on_action_quit ) );

    add_action("undo",    sigc::mem_fun( *this, &ChessController::on_action_undo ) );
    add_action("redo",    sigc::mem_fun( *this, &ChessController::on_action_redo ) );
    add_action("arrange", sigc::mem_fun( *this, &ChessController::on_action_arrange ) );

    add_action("level",        sigc::mem_fun( *this, &ChessController::on_action_level ) );
    add_action("piecevalues",  sigc::mem_fun( *this, &ChessController::on_action_piecevalues ) );
    add_action("colours",      sigc::mem_fun( *this, &ChessController::on_action_colours ) );
    add_action("reverse",      sigc::mem_fun( *this, &ChessController::on_action_reverse ) );
    add_action("showbestline", sigc::mem_fun( *this, &ChessController::on_action_showbestline ) );
    add_action("demomode",     sigc::mem_fun( *this, &ChessController::on_action_demomode ) );

    add_action("about", sigc::mem_fun( *this, &ChessController::on_action_help_about ) );

    add_action("arrange_clear",  sigc::mem_fun( *this, &ChessController::on_action_arrange_clear ) );
    add_action("arrange_done",   sigc::mem_fun( *this, &ChessController::on_action_arrange_done ) );
    add_action("arrange_cancel", sigc::mem_fun( *this, &ChessController::on_action_arrange_cancel ) );
	add_action_bool("arrange_setwhite", sigc::mem_fun(*this, &ChessController::on_action_arrange_turn_white), true );
	add_action_bool("arrange_setblack", sigc::mem_fun(*this, &ChessController::on_action_arrange_turn_black), false );
    add_action("arrange_makeFEN", sigc::mem_fun( *this, &ChessController::on_action_arrange_make_fen ) );

    add_action("think_stop", sigc::mem_fun( *this, &ChessController::on_action_thinking_stop ) );


}

void ChessController::connect_signals()
{
	auto mouse_event = Gtk::GestureClick::create();
	auto motion_controller = Gtk::EventControllerMotion::create();

	board->add_controller( mouse_event );
	board->add_controller( motion_controller );

	mouse_event->signal_pressed().connect( sigc::mem_fun(*this, &ChessController::on_drag_start));
	mouse_event->signal_released().connect( sigc::mem_fun(*this, &ChessController::on_drag_done));

	motion_controller->signal_motion().connect( sigc::mem_fun(*this, &ChessController::on_drag_motion));

	slot_move_calculator.connect( sigc::mem_fun(*this, &ChessController::move_calculator_notify) );

    dlgNewGame->signal_close_request().connect( [this]() { on_dialog_response( DlgID::NONE ); dlgNewGame->hide(); return true; }, false );
    dlgNewGame->signal_hide().connect( [this]() { on_dialog_response(DlgID::NEW); } );

    dlgPieceValues->signal_close_request().connect( [this]() { on_dialog_response( DlgID::NONE ); dlgPieceValues->hide(); return true; }, false );
    dlgPieceValues->signal_hide().connect( [this]() { on_dialog_response( DlgID::PIECEVALUES ); } );

    dlgColours->signal_close_request().connect( [this]() { on_dialog_response( DlgID::NONE ); dlgColours->hide(); return true; }, false );
    dlgColours->signal_hide().connect( [this]() { on_dialog_response( DlgID::COLOURS ); } );

    dlgLevel->signal_close_request().connect( [this]() { on_dialog_response( DlgID::NONE ); dlgLevel->hide(); return true; }, false );
    dlgLevel->signal_hide().connect( [this]() { on_dialog_response( DlgID::LEVEL ); } );

    dlgTimeInput->signal_close_request().connect( [this]() { on_dialog_response( DlgID::NONE ); dlgTimeInput->hide(); return true; }, false );
    dlgTimeInput->signal_hide().connect( [this]() { on_dialog_response( DlgID::INPUT ); } );
}

void ChessController::get_widgets()
{
    Glib::RefPtr<Gtk::Builder> ui_model = Gtk::Builder::create_from_resource( "/net/dnatechnologies/chess/app.ui" );

	view       = ui_model->get_widget<Gtk::ApplicationWindow>("main_view" );
    status_bar = ui_model->get_widget<Gtk::Statusbar>( "widStatusBar" );
	dlgAbout   = ui_model->get_widget<Gtk::AboutDialog>( "dlgAbout"   );
    mnuGame    = ui_model->get_widget<Gtk::PopoverMenuBar>( "mnuGame" );
    mnuArrange = ui_model->get_widget<Gtk::PopoverMenuBar>( "mnuArrange" );
    mnuStop    = ui_model->get_widget<Gtk::PopoverMenuBar>( "mnuStop" );

	board          = Gtk::Builder::get_widget_derived<ChessBoard>( ui_model, "canvas" );
	dlgNewGame     = Gtk::Builder::get_widget_derived<DialogNewGame>( ui_model, "dlgNewGame"   );
    dlgPieceValues = Gtk::Builder::get_widget_derived<DialogPieceValues>( ui_model, "dlgPieceValues");
    dlgColours     = Gtk::Builder::get_widget_derived<DialogColours>( ui_model, "dlgColours"   );
    dlgLevel       = Gtk::Builder::get_widget_derived<DialogLevel>( ui_model, "dlgLevel"   );
	dlgTimeInput   = Gtk::Builder::get_widget_derived<DialogInput>( ui_model, "dlgInput"     );

}

void ChessController::on_activate()
{
	bind_actions();
	get_widgets();
	connect_signals();

	auto image = Gdk::Texture::create_from_resource("/net/dnatechnologies/chess/chess.png");
	dlgAbout->set_logo( image );

	add_window( *view );
	view->show();

	status_bar->push( std::string("") );

	board->set_colours( colours );
	board->set_piece_positions( engine->get_piece_positions() );
	board->set_info( engine->get_info() );

	mnuArrange->hide();
	mnuStop->hide();
}

/**-----------------------------------------------------------------------------
 * Menu actions
 */
void ChessController::on_action_new()
{
	status_bar->push( ::std::string("") );

	dlgNewGame->set_visible( true );
}

void ChessController::on_action_open()
{
    auto dlg = Gtk::FileDialog::create();

    auto open_fun = [this,dlg]( std::shared_ptr<Gio::AsyncResult>& async_result )
    {
        auto file  = dlg->open_finish( async_result );

        if( !engine->load_game( file->get_path() ) ) {
            Gtk::AlertDialog::create( "Error opening chess file." )->show(*get_active_window());
            return;
        }

		board->set_piece_positions( engine->get_piece_positions() );
		board->set_info( engine->get_info() );

		status_bar->push( std::string("Opened ") + file->get_path() );
    };

    dlg->set_title( "Open Chess File" );
    dlg->set_modal( true );

    auto filter_list = Gio::ListStore<Gtk::FileFilter>::create();
    auto pgn_filter = Gtk::FileFilter::create();

    pgn_filter->set_name("PGN Files");
    pgn_filter->add_pattern("*.pgn");
    pgn_filter->add_pattern("*.PGN");

    filter_list->append( pgn_filter );
    dlg->set_filters( filter_list );

    dlg->open( *get_active_window(), open_fun );
}

void ChessController::on_action_save()
{
	status_bar->push( std::string("") );

	if( ! engine->save_game( "" ) ) {
		on_action_save_as();
		return;
	}

	status_bar->push( std::string("Saved game") );
}

void ChessController::on_action_save_as()
{
    auto dlg = Gtk::FileDialog::create();

    auto save_fun = [this,dlg]( std::shared_ptr<Gio::AsyncResult>& async_result )
    {
        auto file  = dlg->open_finish( async_result );

        if( !engine->save_game( file->get_path() ) ) {
            Gtk::AlertDialog::create( "Error saving chess file." )->show(*get_active_window());
            return;
        }

		status_bar->push( std::string("Saved ") + file->get_path() );
    };

    dlg->set_title( "Save Chess File" );
    dlg->set_modal( true );

    auto filter_list = Gio::ListStore<Gtk::FileFilter>::create();
    auto pgn_filter = Gtk::FileFilter::create();

    pgn_filter->set_name("PGN Files");
    pgn_filter->add_pattern("*.pgn");
    pgn_filter->add_pattern("*.PGN");

    filter_list->append( pgn_filter );
    dlg->set_filters( filter_list );

    dlg->open( *get_active_window(), save_fun );
}

void ChessController::on_action_quit()
{
	// if( engine->can_quit() )
		quit();

		// Gtk::AlertDialog::create( "This is not a valid board. Please fix." )->show( *view );
	// Gtk::MessageDialog dlg( *view, "Unsaved game. You really want to quit?", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK_CANCEL, true );
	// if( dlg.run() == Gtk::RESPONSE_OK)
	// 	quit();
}

void ChessController::on_action_play()
{
	/// @TODO Need to actually play
	board->set_piece_positions( engine->get_piece_positions() );
	board->set_info( engine->get_info() );
}

void ChessController::on_action_hint()
{
	do_highlight( engine->hint() );
}

void ChessController::on_action_undo()
{
	engine->undo();

	/// @TODO should animate the undo
	board->set_piece_positions( engine->get_piece_positions() );
	board->set_info( engine->get_info() );

	status_bar->push( std::string("Undone") );
}

void ChessController::on_action_redo()
{
	engine->redo();

	/// @TODO should animate the redo
	board->set_piece_positions( engine->get_piece_positions() );
	board->set_info( engine->get_info() );

	status_bar->push( std::string("Redone") );
}

void ChessController::on_action_arrange()
{
	engine->arranging_start();

	board->set_piece_positions( engine->get_piece_positions() );

	mnuGame->hide();
	mnuArrange->show();

	board->set_edit( true );
}

void ChessController::on_dialog_response( DlgID response )
{
	switch( response ) {
	case DlgID::NEW:
		engine->new_game( dlgNewGame->get_choice() );

		board->set_piece_positions( engine->get_piece_positions() );
		board->set_info( engine->get_info() );

		if( ! engine->current_player_is_human() )
			move_calculator_start();

		status_bar->push( std::string("New game") );
		break;

	case DlgID::PIECEVALUES:
		if( ! engine->set_piece_values( dlgPieceValues->get_values() ) ) {
			Gtk::AlertDialog::create( "Error setting piece values." )->show( *view );
			break;
		}

		status_bar->push( std::string("Piece values changed") );
		break;

	case DlgID::INPUT:
		std::cout << "Dialog response: " << dlgTimeInput->get_input() << std::endl;
		break;

	case DlgID::COLOURS:
		colours = dlgColours->get_colours();
		board->set_colours( colours );
		status_bar->push( std::string("Changed colours") );
		break;

	case DlgID::LEVEL:
		// if( ! engine->set_level( dlgLevel->get_level() ) ) {
		// 	Gtk::AlertDialog::create( "Error setting level." )->show( *view );
		// 	break;
		// }

		// status_bar->push( std::string("Level: ") + engine->get_level_string() );
		break;

	case DlgID::NONE:
		break;
	}
}

void ChessController::on_action_level()
{
	dlgLevel->set_visible( true );
}

// void ChessController::on_action_level_easy()
// {
// 	// if( ! chkLevelEasy->get_active() )
// 	// 	return;

// 	// if( ! engine->set_level_easy() ) {
		// Gtk::AlertDialog::create( "Error setting easy level." )->show( *view );
// 	// 	return;
// 	// }

// 	board->set_info( engine->get_info() );
// 	status_bar->push( std::string("Level: Easy") );
// }

// void ChessController::on_action_level_timed()
// {
// 	// if( ! chkLevelTimed->get_active() )
// 	// 	return;

// 	// dlgTimeInput->dlg_setup( "Timed Level", "Seconds per move:", 120 );

// 	// if( dlgTimeInput->run() != Gtk::RESPONSE_OK ) {
// 	// 	dlgTimeInput->hide();
// 	// 	return;
// 	// }

// 	// dlgTimeInput->hide();

// 	// if( ! engine->set_level_timed( dlgTimeInput->get_input() ) ) {
		// Gtk::AlertDialog::create( "Error setting timed level." )->show( *view );
// 	// 	return;
// 	// }

// 	board->set_info( engine->get_info() );
// 	status_bar->push( std::string("Level: Timed") );
// }

// void ChessController::on_action_level_total_time()
// {
// 	// if( ! chkLevelTotalTime->get_active() )
// 	// 	return;

// 	// dlgTimeInput->dlg_setup( "Total Time Level", "Minutes per game:", 60 );

// 	// if( dlgTimeInput->run() != Gtk::RESPONSE_OK ) {
// 	// 	dlgTimeInput->hide();
// 	// 	return;
// 	// }

// 	// dlgTimeInput->hide();

// 	// if( ! engine->set_level_total_time( dlgTimeInput->get_input() ) ) {
		// Gtk::AlertDialog::create( "Error setting total time level." )->show( *view );
// 	// 	return;
// 	// }

// 	board->set_info( engine->get_info() );
// 	status_bar->push( std::string("Level: Total Time") );
// }

// void ChessController::on_action_level_infinite()
// {
// 	// if( ! chkLevelInfinite->get_active() )
// 	// 	return;

// 	// if( ! engine->set_level_infinite() ) {
		// Gtk::AlertDialog::create( "Error setting infinite level." )->show( *view );
// 	// 	return;
// 	// }

// 	board->set_info( engine->get_info() );
// 	status_bar->push( std::string("Level: Inifnite") );
// }

// void ChessController::on_action_level_ply_search()
// {
// 	// if( ! chkLevelPlySearch->get_active() )
// 	// 	return;

// 	// if( ! engine->set_level_ply_search() ) {
		// Gtk::AlertDialog::create( "Error setting ply search level." )->show( *view );
// 	// 	return;
// 	// }

// 	board->set_info( engine->get_info() );
// 	status_bar->push( std::string("Level: Ply Search") );
// }

// void ChessController::on_action_level_mate_search()
// {
// 	// if( ! chkLevelMateSearch->get_active() )
// 	// 	return;

// 	// if( ! engine->set_level_mate_search() ) {
		// Gtk::AlertDialog::create( "Error setting mate search level." )->show( *view );
// 	// 	return;
// 	// }

// 	board->set_info( engine->get_info() );
// 	status_bar->push( std::string("Level: Mate Search") );
// }

// void ChessController::on_action_level_matching()
// {
// 	// if( ! chkLevelMatching->get_active() )
// 	// 	return;

// 	// status_bar->push( std::string("") );

// 	// if( ! engine->set_level_matching() ) {
		// Gtk::AlertDialog::create( "Error setting matching level." )->show( *view );
// 	// 	return;
// 	// }

// 	board->set_info( engine->get_info() );
// 	status_bar->push( std::string("Level: Matching") );
// }

void ChessController::on_action_demomode()
{
	/// @TODO Implement demo. Load a game and start animating it
	status_bar->push( std::string("Demo") );

	is_demo = true;

    mnuGame->hide();
    mnuStop->show();

    do_demo_move();
}

void ChessController::on_action_piecevalues()
{
	status_bar->push( std::string("") );

	dlgPieceValues->set_values( engine->get_piece_values() );

	dlgPieceValues->set_visible( true );
}

void ChessController::on_action_colours()
{
	status_bar->push( std::string("") );

	dlgColours->set_colours( colours );

	dlgColours->set_visible( true );
}

void ChessController::on_action_reverse()
{
	status_bar->push( std::string("") );

	board->toggle_reverse( );

	status_bar->push( std::string("Toggled reverse") );
}

void ChessController::on_action_showbestline()
{
	status_bar->push( std::string("") );

	board->toggle_bestline();

	status_bar->push( std::string("Toggled bestline") );
}

void ChessController::on_action_help_about()
{
	dlgAbout->set_visible( true );
}

void ChessController::on_action_arrange_clear()
{
	engine->arranging_start();

	board->set_piece_positions( engine->get_piece_positions() );
}

void ChessController::on_action_arrange_turn_white()
{
	bool current_state = false;

	get_action_state<bool>( "arrange_setwhite", current_state );

	change_action_state( "arrange_setwhite",Glib::Variant<bool>::create(!current_state) );
    change_action_state("arrange_setblack", Glib::Variant<bool>::create(current_state));

	std::cout << "White turn" << std::endl;
	// engine->arrange_set_turn( eColor::white );
}

void ChessController::on_action_arrange_turn_black()
{
	bool current_state = false;

	get_action_state<bool>( "arrange_setblack", current_state );

	change_action_state( "arrange_setblack",Glib::Variant<bool>::create(!current_state) );
    change_action_state("arrange_setwhite", Glib::Variant<bool>::create(current_state));

	std::cout << "Black turn" << std::endl;
	// engine->arrange_set_turn( eColor::black );
}

void ChessController::on_action_arrange_cancel()
{
	engine->arranging_end( true );

	board->set_piece_positions( engine->get_piece_positions() );

	mnuArrange->hide();
	mnuGame->show();

	board->set_edit( false );
}

void ChessController::on_action_arrange_done()
{
	if( ! engine->arranging_end( false ) ) {
		Gtk::AlertDialog::create( "This is not a valid board. Please fix." )->show( *view );
		return;
	}

	board->set_piece_positions( engine->get_piece_positions() );

	mnuArrange->hide();
	mnuGame->show();

	board->set_edit( false );
}

void ChessController::on_action_arrange_make_fen()
{
	std::string FEN = engine->arrange_to_fen();

	dlgTimeInput->dlg_setup( "Arrange", "FEN Setup:", FEN );
	dlgTimeInput->set_visible( true );
}

void ChessController::on_action_thinking_stop()
{
	if( ! is_demo ) {
		engine->stop_thinking();
		slot_move_calculator.emit();
	} else {
		mnuStop->hide();
		mnuGame->show();
		is_demo = false;
	}
}

/**-----------------------------------------------------------------------------
 * Mouse input
 */
void ChessController::on_drag_start( int n_press, double mouse_x, double mouse_y )
{
	if( is_animating || is_computer_move )		// block user input during animations
		return;

	if( board->point_in_chessboard( {mouse_x, mouse_y} ) ) {

		drag_start_square = board->boardpoint_to_chesssquare( {mouse_x, mouse_y} );
		drag_piece_code = (engine->get_piece_positions())[drag_start_square];

		if( drag_piece_code == ' ' )
			return;

		auto temp_board = engine->get_piece_positions();
		temp_board[drag_start_square] = ' ';
		board->set_piece_positions( temp_board );

		board->drag_start( mouse_x, mouse_y, drag_piece_code );
		is_dragging = true;

		return;
	}

	if( board->point_in_editwindow( {mouse_x, mouse_y} ) ) {

		drag_start_square = (uint16_t)-1;
		drag_piece_code = board->editpoint_to_piececode( {mouse_x, mouse_y} );

		board->drag_start(  mouse_x, mouse_y, drag_piece_code );
		is_dragging = true;

		return;
	}
}

void ChessController::on_drag_motion( double mouse_x, double mouse_y )
{
	if( !is_animating && is_dragging )
		board->drag_continue( mouse_x, mouse_y );
}

void ChessController::on_drag_done( int n_press, double mouse_x, double mouse_y )
{
	if( is_animating || !is_dragging )
		return;

	board->drag_finish();

	if( board->point_in_chessboard( {mouse_x, mouse_y} ) ) {

		uint16_t drag_end_square = board->boardpoint_to_chesssquare( {mouse_x, mouse_y} );

		if( drag_start_square == drag_end_square ) {

			board->set_piece_positions( engine->get_piece_positions() );

		} else if( engine->in_edit_mode() ) {	// Arranging?

			if( drag_start_square != (uint16_t)-1 )
				engine->arrange_remove_piece( drag_start_square );

			if( drag_end_square != (uint16_t)-1 )
				engine->arrange_add_piece( drag_end_square, drag_piece_code );

			board->set_piece_positions( engine->get_piece_positions() );

		} else {

			// regular move, check if this this move can be made
			if( engine->human_move( drag_start_square, drag_end_square ) )
				do_animate( drag_start_square, drag_end_square, drag_piece_code );
			else {
				board->set_piece_positions( engine->get_piece_positions() );
			}
		}

		is_dragging = false;
		return;

	}

	if( engine->in_edit_mode() ) {

		if( drag_start_square != (uint16_t)-1 )						// Piece is dragged of the board
			engine->arrange_remove_piece( drag_start_square );

		board->set_piece_positions( engine->get_piece_positions() );

		is_dragging = false;
		return;
	}
}

/**-----------------------------------------------------------------------------
 * Animation of moves, highlights
 */
void ChessController::do_animate( uint16_t start_square, uint16_t end_square, char piece )
{
	auto temp_board = engine->get_piece_positions();
	temp_board[ start_square ] = ' ';
	temp_board[ end_square ] = ' ';
	board->set_piece_positions( temp_board );
	board->set_info( engine->get_info() );

    board->animate_start( start_square, end_square, piece );

    timeout_counter = 10;
    Glib::signal_timeout().connect( sigc::mem_fun(*this, &ChessController::on_animate_timeout), 100 );
}

bool ChessController::on_animate_timeout()
{
	if( --timeout_counter ) {
		board->animate_continue();
		return true;
	}

	board->animate_finish();

	board->set_piece_positions( engine->get_piece_positions() );
	board->set_info( engine->get_info() );

	// if it is the computers turn here, let the AI calculate a move
	if( ! engine->current_player_is_human() )
		move_calculator_start();

	return false;
}

void ChessController::do_highlight( uint16_t square )
{
	board->highlight_start( square );

	timeout_counter = 10;
	Glib::signal_timeout().connect( sigc::mem_fun(*this, &ChessController::on_highlight_timeout), 100 );
}

bool ChessController::on_highlight_timeout()
{
	if( --timeout_counter ) {
		board->highlight_continue();
		return true;
	}

	board->highlight_finish();

	return false;
}


/**-----------------------------------------------------------------------------
 * AI move calculator
 */
void ChessController::move_calculator_start()
{
	mnuGame->hide();
	mnuStop->show();

	auto thread_lambda = [](ChessController* self) { self->move_calculator_thread(); };
	auto custom_deleter = [](std::thread* t) { std::cout << "Custom deleter\n"; if (t && t->joinable()) t->join(); delete t; };

	thread_move_calculator = ThreadMoveCalcType( new std::thread( thread_lambda, this ), custom_deleter );
}

void ChessController::move_calculator_thread()
{
	engine->AI_move( AI_start_square, AI_end_square, AI_piece );

	slot_move_calculator.emit();
}

void ChessController::move_calculator_notify()
{
	thread_move_calculator.reset();

	mnuStop->hide();
	mnuGame->show();

	do_animate( AI_start_square, AI_end_square, AI_piece );
}


bool ChessController::do_demo_move()
{
	/// \TODO Implement demo move
//    Ply ply;
//
//    if( ! engine->get_next_ply( ply ) ) {
//        mnuStop->hide();
//        mnuGame->show();
//        return false;
//    }
//
//    uint16_t start_square = ply.square_from();
//    uint16_t end_square = ply.square_to();
//
//		auto temp_board = engine->get_piece_positions();
//	char piece = temp_board[start_square];
//	temp_board[start_square] = ' ';
//	board->set_piece_positions( temp_board );
//
//    board->animate_start( start_square, end_square, piece );
//
//    timeout_counter = 10;
//    Glib::signal_timeout().connect( sigc::mem_fun(*this, &ChessController::on_demo_move_timeout), 100 );

	move_calculator_start();

    return false;
}

bool ChessController::on_demo_move_timeout()
{
	if( --timeout_counter ) {
        board->animate_continue();
        return true;
	}

    board->animate_finish();

	board->set_piece_positions( engine->get_piece_positions() );

    Glib::signal_timeout().connect( sigc::mem_fun(*this, &ChessController::do_demo_move), 1000 );	// Wait a second, then initiate the next move

    return false;
}

}