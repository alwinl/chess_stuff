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

static void push_positions( ChessBoard* board, std::map<STSquare,STPiece> positions )
{
	std::map<STSquare,char> new_map;

	for( auto position : positions )
		new_map.insert( std::make_pair( position.first, position.second.code ) );

	board->set_piece_positions( new_map );
}

static void push_info( ChessBoard* board, STInfo the_info )
{
	std::array<std::pair<std::string,std::string>,10> info =
	{
		std::pair<std::string,std::string>{"Turn", the_info.turn },
		std::pair<std::string,std::string>{"White", the_info.white },
		std::pair<std::string,std::string>{"Black", the_info.black },
		std::pair<std::string,std::string>{"Time", the_info.time },
		std::pair<std::string,std::string>{"Level", the_info.level },
		std::pair<std::string,std::string>{"Value", the_info.value },
		std::pair<std::string,std::string>{"Nodes", the_info.nodes },
		std::pair<std::string,std::string>{"N/Sec", the_info.n_sec },
		std::pair<std::string,std::string>{"Depth", the_info.depth },
		std::pair<std::string,std::string>{"Bestline", the_info.bestline }
	};

	board->set_info( info );
}


/**-----------------------------------------------------------------------------
 * Application window
 */
class ChessWindow : public Gtk::ApplicationWindow
{
public:
	ChessWindow( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model ) : Gtk::ApplicationWindow(cobject) {};
};


/**-----------------------------------------------------------------------------
 * Construction / Destruction
 */
ChessController::ChessController( ) : Gtk::Application( "net.dnatechnologies.chess" )
{
	Glib::set_application_name("GTKmm Chess");

	engine = new ChessEngine;

	thread_move_calculator = nullptr;
}

ChessController::~ChessController( )
{
	delete engine;

	if( thread_move_calculator && thread_move_calculator->joinable() )
		thread_move_calculator->join();
}

/**-----------------------------------------------------------------------------
 * Setup
 */
void ChessController::on_startup()
{
    Gtk::Application::on_startup();

	bind_actions();
	get_widgets();
	connect_signals();
}

void ChessController::bind_actions()
{
    add_action("new",     sigc::mem_fun( *this, &ChessController::on_action_new ) );
    add_action("restore", sigc::mem_fun( *this, &ChessController::on_action_open ) );
    add_action("save",    sigc::mem_fun( *this, &ChessController::on_action_save ) );
    add_action("saveas",  sigc::mem_fun( *this, &ChessController::on_action_save_as ) );
    add_action("quit",    sigc::mem_fun( *this, &ChessController::on_action_quit ) );
    add_action("play",    sigc::mem_fun( *this, &ChessController::on_action_play ) );
    add_action("hint",    sigc::mem_fun( *this, &ChessController::on_action_hint ) );

    add_action("undo",    sigc::mem_fun( *this, &ChessController::on_action_undo ) );
    add_action("redo",    sigc::mem_fun( *this, &ChessController::on_action_redo ) );
    add_action("arrange", sigc::mem_fun( *this, &ChessController::on_action_arrange ) );

    add_action("twoplayer",   sigc::mem_fun( *this, &ChessController::on_action_twoplayer ) );
    add_action("demomode",    sigc::mem_fun( *this, &ChessController::on_action_demomode ) );
    add_action("piecevalues", sigc::mem_fun( *this, &ChessController::on_action_piecevalues ) );

    add_action("colours",      sigc::mem_fun( *this, &ChessController::on_action_colours ) );
    add_action("reverse",      sigc::mem_fun( *this, &ChessController::on_action_reverse ) );
    add_action("showbestline", sigc::mem_fun( *this, &ChessController::on_action_showbestline ) );

    add_action("about", sigc::mem_fun( *this, &ChessController::on_action_help_about ) );

    add_action("arrange_done",   sigc::mem_fun( *this, &ChessController::on_action_arrange_done ) );
    add_action("arrange_clear",  sigc::mem_fun( *this, &ChessController::on_action_arrange_clear ) );
    add_action("arrange_cancel", sigc::mem_fun( *this, &ChessController::on_action_arrange_cancel ) );

    add_action("think_stop", sigc::mem_fun( *this, &ChessController::on_action_thinking_stop ) );
}

void ChessController::connect_signals()
{
 	chkLevelEasy->signal_activate().connect( sigc::mem_fun(*this, &ChessController::on_action_level_easy ) );
	chkLevelTimed->signal_activate().connect( sigc::mem_fun(*this, &ChessController::on_action_level_timed ) );
	chkLevelTotalTime->signal_activate().connect( sigc::mem_fun(*this, &ChessController::on_action_level_total_time) );
	chkLevelInfinite->signal_activate().connect( sigc::mem_fun(*this, &ChessController::on_action_level_infinite) );
	chkLevelPlySearch->signal_activate().connect( sigc::mem_fun(*this, &ChessController::on_action_level_ply_search) );
	chkLevelMateSearch->signal_activate().connect( sigc::mem_fun(*this, &ChessController::on_action_level_mate_search) );
	chkLevelMatching->signal_activate().connect( sigc::mem_fun(*this, &ChessController::on_action_level_matching) );

	chkTurnWhite->signal_activate().connect( sigc::mem_fun(*this, &ChessController::on_action_arrange_turn_white) );
	chkTurnBlack->signal_activate().connect( sigc::mem_fun(*this, &ChessController::on_action_arrange_turn_black) );

	board->signal_button_press_event().connect( sigc::mem_fun(*this, &ChessController::on_drag_start) );
	board->signal_button_release_event().connect( sigc::mem_fun(*this, &ChessController::on_drag_done) );

	slot_move_calculator.connect( sigc::mem_fun(*this, &ChessController::on_move_calculator_notify) );
}

void ChessController::get_widgets()
{
    Glib::RefPtr<Gtk::Builder> ui_model = Gtk::Builder::create_from_resource( "/net/dnatechnologies/chess/appui.glade" );

	ui_model->get_widget_derived("main_view", view );
    ui_model->get_widget( "widStatusBar", status_bar );
	ui_model->get_widget_derived("canvas", board );

    ui_model->get_widget_derived( "dlgPieceValue", dlgPieceValues, *view );
    ui_model->get_widget_derived( "dlgColours", dlgColours, *view );
	ui_model->get_widget_derived("dlgInput", dlgTimeInput, *view );

	ui_model->get_widget( "chkLevelEasy", chkLevelEasy );
	ui_model->get_widget( "chkLevelTimed", chkLevelTimed );
	ui_model->get_widget( "chkLevelTotalTime", chkLevelTotalTime );
	ui_model->get_widget( "chkLevelInfinite", chkLevelInfinite );
	ui_model->get_widget( "chkLevelPlySearch", chkLevelPlySearch );
	ui_model->get_widget( "chkLevelMateSearch", chkLevelMateSearch );
	ui_model->get_widget( "chkLevelMatching", chkLevelMatching );

    ui_model->get_widget( "mnuTwoplayer", mnuTwoplayer );

	ui_model->get_widget( "chkOptionSound", chkSound );

    ui_model->get_widget( "mnuGame", mnuGame );
    ui_model->get_widget( "mnuArrange", mnuArrange );
    ui_model->get_widget( "mnuStop", mnuStop );

    ui_model->get_widget( "chkTurnWhite", chkTurnWhite );
    ui_model->get_widget( "chkTurnBlack", chkTurnBlack );
}

void ChessController::on_activate()
{
	view->set_default_size( 640,480 );
	view->set_icon( Gdk::Pixbuf::create_from_resource("/net/dnatechnologies/chess/chess.png") );

    add_window( *view );
    view->show();

	board->set_colours( engine->get_colour_values() );

	chkLevelEasy->set_active();
	chkTurnWhite->set_active();

	mnuArrange->hide();
	mnuStop->hide();

    on_action_new();
}


/**-----------------------------------------------------------------------------
 * Menu actions
 */
void ChessController::on_action_new()
{
	status_bar->push( std::string("") );

	engine->new_game(  );

	push_positions( board, engine->get_piece_positions() );
	push_info( board, engine->get_info() );

	status_bar->push( std::string("New game") );
}

void ChessController::on_action_open()
{
    Gtk::FileChooserDialog dlg( *view, "Open Chess File", Gtk::FILE_CHOOSER_ACTION_OPEN );

    dlg.add_button( "Cancel", Gtk::RESPONSE_CANCEL );
    dlg.add_button( "Open", Gtk::RESPONSE_ACCEPT );

    Glib::RefPtr<Gtk::FileFilter> filter_pgn = Gtk::FileFilter::create();

    filter_pgn->set_name("PGN Files");
    filter_pgn->add_pattern("*.pgn");
    filter_pgn->add_pattern("*.PGN");

    dlg.add_filter( filter_pgn );
    dlg.set_filter( filter_pgn );

    if( dlg.run() != Gtk::RESPONSE_ACCEPT ) {
		dlg.hide();
        return;
    }

    dlg.hide();

    if( ! engine->open_file( dlg.get_filename() ) ) {

		if( chkSound->get_active() )
			Gdk::Display::get_default()->beep();

        Gtk::MessageDialog( *view, "Error opening chess file.", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true).run();
        return;
    }

	push_positions( board, engine->get_piece_positions() );
	push_info( board, engine->get_info() );

    status_bar->push( std::string("Opened ") + dlg.get_filename() );
}

void ChessController::on_action_save()
{
	status_bar->push( std::string("") );

	if( ! engine->save_file( ) ) {

		if( chkSound->get_active() )
			Gdk::Display::get_default()->beep();

		Gtk::MessageDialog( *view, "Error saving game. Try Save As.", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true).run();
		return;
	}

	status_bar->push( std::string("Saved game") );
}

void ChessController::on_action_save_as()
{
    Gtk::FileChooserDialog dlg( *view, "Save Chess File", Gtk::FILE_CHOOSER_ACTION_SAVE );

    dlg.add_button( "Cancel", Gtk::RESPONSE_CANCEL );
    dlg.add_button( "Save", Gtk::RESPONSE_OK );

    Glib::RefPtr<Gtk::FileFilter> filter_pgn = Gtk::FileFilter::create();
    filter_pgn->set_name( "PGN Files" );
    filter_pgn->add_pattern("*.pgn");
    filter_pgn->add_pattern("*.PGN");

    dlg.add_filter( filter_pgn );
    dlg.set_filter( filter_pgn );

    if( dlg.run() != Gtk::RESPONSE_OK ) {
		dlg.hide();
        return;
    }

    dlg.hide();

    if( ! engine->save_file_as( dlg.get_filename() ) ) {

		if( chkSound->get_active() )
			Gdk::Display::get_default()->beep();

        Gtk::MessageDialog( *view, "Error saving game.", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true).run();
        return;
   }

    status_bar->push( std::string( "Saved " ) + dlg.get_filename() );
}

void ChessController::on_action_quit()
{
	if( engine->can_quit() )
		quit();

	Gtk::MessageDialog dlg( *view, "Unsaved game. You really want to quit?", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK_CANCEL, true );
	if( dlg.run() == Gtk::RESPONSE_OK)
		quit();
}

void ChessController::on_action_play()
{
	push_positions( board, engine->get_piece_positions() );
	push_info( board, engine->get_info() );
}

void ChessController::on_action_hint()
{
	do_highlight( engine->hint() );

}

void ChessController::on_action_undo()
{
	engine->undo();

	push_positions( board, engine->get_piece_positions() );
	push_info( board, engine->get_info() );

	status_bar->push( std::string("Undone") );
}

void ChessController::on_action_redo()
{
	engine->redo();

	push_positions( board, engine->get_piece_positions() );
	push_info( board, engine->get_info() );

	status_bar->push( std::string("Redone") );
}

void ChessController::on_action_arrange()
{
	engine->arranging_start();

	push_positions( board, engine->get_piece_positions() );

	mnuGame->hide();
	mnuArrange->show();

	board->set_edit( true );
}

void ChessController::on_action_level_easy()
{
	if( ! chkLevelEasy->get_active() )
		return;

	if( ! engine->set_level_easy() ) {

		if( chkSound->get_active() )
			Gdk::Display::get_default()->beep();

		Gtk::MessageDialog( *view, "Error setting easy level.", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true).run();
		return;
	}

	status_bar->push( std::string("Level: Easy") );
}

void ChessController::on_action_level_timed()
{
	if( ! chkLevelTimed->get_active() )
		return;

	dlgTimeInput->dlg_setup( "Timed Level", "Seconds per move:", engine->get_timed_level_value() );

	if( dlgTimeInput->run() != Gtk::RESPONSE_OK ) {
		dlgTimeInput->hide();
		return;
	}

	dlgTimeInput->hide();

	if( ! engine->set_level_timed( dlgTimeInput->get_input() ) ) {

		if( chkSound->get_active() )
			Gdk::Display::get_default()->beep();

		Gtk::MessageDialog( *view, "Error setting timed level.", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true).run();
		return;
	}

	status_bar->push( std::string("Level: Timed") );
}

void ChessController::on_action_level_total_time()
{
	if( ! chkLevelTotalTime->get_active() )
		return;

	dlgTimeInput->dlg_setup( "Total Time Level", "Minutes per game:", engine->get_total_time_level_value() );

	if( dlgTimeInput->run() != Gtk::RESPONSE_OK ) {
		dlgTimeInput->hide();
		return;
	}

	dlgTimeInput->hide();

	if( ! engine->set_level_total_time( dlgTimeInput->get_input() ) ) {

		if( chkSound->get_active() )
			Gdk::Display::get_default()->beep();

		Gtk::MessageDialog( *view, "Error setting total time level.", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true).run();
		return;
	}

	status_bar->push( std::string("Level: Total Time") );
}

void ChessController::on_action_level_infinite()
{
	if( ! chkLevelInfinite->get_active() )
		return;

	if( ! engine->set_level_infinite() ) {

		if( chkSound->get_active() )
			Gdk::Display::get_default()->beep();

		Gtk::MessageDialog( *view, "Error setting infinite level.", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true).run();
		return;
	}

	status_bar->push( std::string("Level: Inifnite") );
}

void ChessController::on_action_level_ply_search()
{
	if( ! chkLevelPlySearch->get_active() )
		return;

	if( ! engine->set_level_ply_search() ) {

		if( chkSound->get_active() )
			Gdk::Display::get_default()->beep();

		Gtk::MessageDialog( *view, "Error setting ply search level.", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true).run();
		return;
	}

	status_bar->push( std::string("Level: Ply Search") );
}

void ChessController::on_action_level_mate_search()
{
	if( ! chkLevelMateSearch->get_active() )
		return;

	if( ! engine->set_level_mate_search() ) {

		if( chkSound->get_active() )
			Gdk::Display::get_default()->beep();

		Gtk::MessageDialog( *view, "Error setting mate search level.", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true).run();
		return;
	}

	status_bar->push( std::string("Level: Mate Search") );
}

void ChessController::on_action_level_matching()
{
	if( ! chkLevelMatching->get_active() )
		return;

	status_bar->push( std::string("") );

	if( ! engine->set_level_matching() ) {

		if( chkSound->get_active() )
			Gdk::Display::get_default()->beep();

		Gtk::MessageDialog( *view, "Error setting matching level.", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true).run();
		return;
	}

	status_bar->push( std::string("Level: Matching") );
}

void ChessController::on_action_twoplayer()
{
	mnuTwoplayer->set_label( engine->toggle_multiplayer() ? "_Single Player" : "_Two Player" );
}

void ChessController::on_action_demomode()
{
	status_bar->push( std::string("Demo") );

    mnuGame->hide();
    mnuStop->show();

    do_demo_move();
}

void ChessController::on_action_piecevalues()
{
	status_bar->push( std::string("") );

	dlgPieceValues->set_values( engine->get_piece_values() );

    if( dlgPieceValues->run() != Gtk::RESPONSE_OK ) {
		dlgPieceValues->hide();
		return;
    }

    dlgPieceValues->hide();

	if( ! engine->set_piece_values( dlgPieceValues->get_values() ) ) {

		if( chkSound->get_active() )
			Gdk::Display::get_default()->beep();

		Gtk::MessageDialog( *view, "Error setting piece values.", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true).run();
		return;
	}

	status_bar->push( std::string("Piece values changed") );
}

void ChessController::on_action_colours()
{
	status_bar->push( std::string("") );

	dlgColours->set_colours( engine->get_colour_values() );

    if( dlgColours->run() != Gtk::RESPONSE_OK ) {
		dlgColours->hide();
		return;
    }

    dlgColours->hide();

	if( ! engine->set_colour_values( dlgColours->get_colours() ) ) {

		if( chkSound->get_active() )
			Gdk::Display::get_default()->beep();

		Gtk::MessageDialog( *view, "Error changing colours.", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true).run();
		return;
	}

	board->set_colours( dlgColours->get_colours() );

	status_bar->push( std::string("Changed colours") );
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
    Gtk::AboutDialog dlg;

    dlg.set_transient_for( *view ) ;
    dlg.set_name("chess") ;
    dlg.set_logo( Gdk::Pixbuf::create_from_resource("/net/dnatechnologies/chess/chess.png") ) ;
    dlg.set_version( "0.01" ) ;
    dlg.set_comments("A GtkMM translation of Borland Chess") ;
    dlg.set_copyright( "Copyright © 2017 Alwin Leerling" ) ;
    dlg.set_website( "http://github" ) ;

    dlg.run();
}

void ChessController::on_action_arrange_clear()
{
	engine->arranging_clear();

	push_positions( board, engine->get_piece_positions() );
}

void ChessController::on_action_arrange_turn_white()
{
	if( !chkTurnWhite->get_active() )
		return;

	engine->arrange_turn( TURNWHITE );
}

void ChessController::on_action_arrange_turn_black()
{
	if( !chkTurnBlack->get_active() )
		return;

	engine->arrange_turn( TURNBLACK );
}

void ChessController::on_action_arrange_cancel()
{
	engine->arranging_end( true );

	push_positions( board, engine->get_piece_positions() );

	mnuArrange->hide();
	mnuGame->show();

	board->set_edit( false );
}

void ChessController::on_action_arrange_done()
{
	if( ! engine->arranging_end( false ) ) {
		if( chkSound->get_active() )
			Gdk::Display::get_default()->beep();

		Gtk::MessageDialog( *view, "This is not a valid board. Please fix.", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_CANCEL, true ).run();
		return;
	}

	push_positions( board, engine->get_piece_positions() );

	mnuArrange->hide();
	mnuGame->show();

	board->set_edit( false );
}

void ChessController::on_action_thinking_stop()
{
	engine->stop_thinking();
}

/**-----------------------------------------------------------------------------
 * Mouse input
 */
bool ChessController::on_drag_start( GdkEventButton* button_event )
{
	drag_start_square.file = button_event->x;
	drag_start_square.rank = button_event->y;
	drag_piece_code        = button_event->state;

	std::map<STSquare,STPiece> save_board = engine->get_piece_positions();
	save_board.erase( drag_start_square );
	push_positions( board, save_board );

	return true;
}

bool ChessController::on_drag_done( GdkEventButton* button_event )
{
	drag_end_square.file = button_event->x;
	drag_end_square.rank = button_event->y;

	if( drag_start_square == drag_end_square ) {
		push_positions( board, engine->get_piece_positions() );
		return true;
	}

	// Arranging?
	if( engine->in_edit_mode() ) {

        engine->arrange_remove_piece( drag_start_square );
		engine->arrange_add_piece( drag_end_square, drag_piece_code );

		push_positions( board, engine->get_piece_positions() );
		return true;
	}

	std::map<STSquare,STPiece> save_board = engine->get_piece_positions();

	// regular move, check if this this move can be made
	if( engine->enter_move( drag_start_square, drag_end_square ) ) {

		// take the piece of the start square:
		save_board.erase( drag_start_square );
		push_positions( board, save_board );

		do_animate( drag_start_square, drag_end_square, drag_piece_code );

		return true;
	}

	push_positions( board, save_board );
	return true;
}


/**-----------------------------------------------------------------------------
 * AI move calculator
 */
void ChessController::move_calculator_start()
{
	mnuGame->hide();
	mnuStop->show();

	board->computer_is_thinking( true );

	thread_move_calculator = new std::thread( sigc::mem_fun(*this, &ChessController::move_calculator_thread) );
}

void ChessController::move_calculator_thread()
{
	engine->calculate_move();

	slot_move_calculator.emit();
}

void ChessController::on_move_calculator_notify()
{
	if( thread_move_calculator->joinable() )
		thread_move_calculator->join();

	thread_move_calculator = nullptr;

	push_positions( board, engine->get_piece_positions() );
	push_info( board, engine->get_info() );

	mnuStop->hide();
	mnuGame->show();

	board->computer_is_thinking( false );
}


/**-----------------------------------------------------------------------------
 * Animation of moves, highlights and demo
 */
void ChessController::do_animate( STSquare start_square, STSquare end_square, char piece )
{
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

	push_positions( board, engine->get_piece_positions() );

	// if it is the computers turn here, let the AI calculate a move
	move_calculator_start();

	return false;
}

void ChessController::do_highlight( STSquare square )
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

bool ChessController::do_demo_move()
{
    Ply ply;

    if( ! engine->get_next_ply( ply ) ) {
        mnuStop->hide();
        mnuGame->show();
        return false;
    }

    STSquare start_square = ply.get_start_square();
    STSquare end_square = ply.get_end_square();

	std::map<STSquare,STPiece> save_board = engine->get_piece_positions();
	// extract the piece code
	char piece = save_board.at(start_square).code;
	// take the piece of the start square:
	save_board.erase( start_square );
	push_positions( board, save_board );

    board->animate_start( start_square, end_square, piece );

    timeout_counter = 10;
    Glib::signal_timeout().connect( sigc::mem_fun(*this, &ChessController::on_demo_move_timeout), 100 );

    return false;
}

bool ChessController::on_demo_move_timeout()
{
	if( --timeout_counter ) {
        board->animate_continue();
        return true;
	}

    board->animate_finish();

	push_positions( board, engine->get_piece_positions() );

    Glib::signal_timeout().connect( sigc::mem_fun(*this, &ChessController::do_demo_move), 1000 );	// Wait a second, then initiate the next move

    return false;
}

