/*
 * Copyright 2014 Alwin Leerling <alwin@jambo>
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

#include "chesscontroller.h"

#include "chessview.h"
#include "chessboard.h"
#include "dlgcolours.h"
#include "dlgtimeinputter.h"
#include "dlgpiecevalues.h"
#include "dlgfilenamechooser.h"

#include "../logicsrc/chessengine.h"


/**-----------------------------------------------------------------------------
 * \brief Create an instance of the application
 *
 * \return Glib::RefPtr<ChessController>
 */
Glib::RefPtr<ChessController> ChessController::create( ChessEngine* engine_init )
{
    return Glib::RefPtr<ChessController>( new ChessController( engine_init ) );
}

/**-----------------------------------------------------------------------------
 * \brief Application class constructor
 */
ChessController::ChessController( ChessEngine* engine_init ) : Gtk::Application( "net.dnatechnologies.chess" ), engine( engine_init )
{
	Glib::set_application_name("GTKmm Chess");
	move_calculator_thread = nullptr;
}

/**-----------------------------------------------------------------------------
 * \brief Application class destructor
 */
ChessController::~ChessController( )
{
}

/**-----------------------------------------------------------------------------
 * \brief Startup code
 */
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

    add_action("about",   sigc::mem_fun( *this, &ChessController::on_action_help_about ) );

    add_action("arrange_done", sigc::mem_fun( *this, &ChessController::on_action_arrange_done ) );
    add_action("arrange_clear", sigc::mem_fun( *this, &ChessController::on_action_arrange_clear ) );
    add_action("arrange_cancel",  sigc::mem_fun( *this, &ChessController::on_action_arrange_cancel ) );

    add_action("thinking_stop", sigc::mem_fun( *this, &ChessController::on_action_thinking_stop ) );
}

/**-----------------------------------------------------------------------------
 * \brief Startup code
 */
void ChessController::get_widgets()
{
    Glib::RefPtr<Gtk::Builder> ui_model = Gtk::Builder::create_from_resource( "/net/dnatechnologies/chess/appui.glade" );

	ui_model->get_widget_derived("main_view", view );
    ui_model->get_widget( "widStatusBar", status_bar );
	ui_model->get_widget_derived("canvas", board );

	board->signal_button_release_event().connect( sigc::mem_fun( *this, &ChessController::on_board_button_released) );

	Gtk::RadioMenuItem * chkMenuItem;

	std::vector<std::string> level_widgets = { "chkLevelEasy", "chkLevelTimed", "chkLevelTotalTime", "chkLevelInfinite", "chkLevelPlaySearch", "chkLevelMateSearch", "chkLevelMatching" };
	for( unsigned int level = EASY; level < LEVELCOUNT; ++level ) {

		ui_model->get_widget( level_widgets[level], chkMenuItem );
		chkLevelItems.push_back( chkMenuItem );

		chkLevelItems[level]->signal_activate().connect( sigc::bind<unsigned int>(sigc::mem_fun(*this, &ChessController::on_action_level ), level) );
	}

	std::vector<std::string> turn_widgets = { "chkTurnWhite", "chkTurnBlack" };
	for( unsigned int turn = TURNWHITE; turn < TURNCOUNT; ++turn ) {

		ui_model->get_widget( turn_widgets[turn], chkMenuItem );
		chkTurnItems.push_back( chkMenuItem );

		chkTurnItems[turn]->signal_activate().connect( sigc::bind<unsigned int>(sigc::mem_fun(*this, &ChessController::on_action_arrange_turn ), turn) );
	}

	move_calculator_slot.connect(sigc::mem_fun(*this, &ChessController::on_move_calculator_notify ));

	ui_model->get_widget( "chkOptionSound", chkSound );

	engine->init_colour_chooser( new GUIColourChooser( ui_model, *view ) );
	engine->init_time_inputter( new GUITimeInputter( ui_model, *view ) );
	engine->init_piece_value_object( new GUIPieceValues( ui_model, *view ) );
	engine->init_filename_chooser( new GUIFilenameChooser( *view ) );
}

/**-----------------------------------------------------------------------------
 * \brief Startup code
 */
void ChessController::on_startup()
{
    Gtk::Application::on_startup();

	bind_actions();
	get_widgets();
}

/**-----------------------------------------------------------------------------
 * \brief Connect the GUI to our objects
 *
 * The application has been started. Time to create and show a window
 * NB we could build multiple views here and activate (show) views
 * as needed
 */
void ChessController::on_activate()
{
    add_window( *view );
    view->show();

	board->set_colours( engine->get_colours(), engine->get_info() );

	chkLevelItems[EASY]->set_active();
	chkTurnItems[TURNWHITE]->set_active();

    on_action_new();
}

/**-----------------------------------------------------------------------------
 * \brief Menu actions
 */
void ChessController::on_action_new()
{
	status_bar->push( std::string("") );

	engine->new_game(  );

    board->set_piece_positions( engine->get_piece_positions() );
    board->set_info( engine->get_info() );

	status_bar->push( std::string("New game") );
}

/**-----------------------------------------------------------------------------
 * \brief Menu actions
 */
void ChessController::on_action_open()
{
	status_bar->push( std::string("") );

	if( ! engine->open_file( ) ) {
		if( chkSound->get_active() )
			Gdk::Display::get_default()->beep();

		Gtk::MessageDialog( *view, "Error restoring game.", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true).run();
		return;
	}

	status_bar->push( std::string("Game loaded") );
}

/**-----------------------------------------------------------------------------
 * \brief Menu actions
 */
void ChessController::on_action_save()
{
	status_bar->push( std::string("") );

	if( ! engine->save_file( ) ) {
		if( chkSound->get_active() )
			Gdk::Display::get_default()->beep();

		Gtk::MessageDialog( *view, "Error saving game. Try Save As", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true).run();
		return;
	}

	status_bar->push( std::string("Saved game") );
}

/**-----------------------------------------------------------------------------
 * \brief Menu actions
 */
void ChessController::on_action_save_as()
{
	status_bar->push( std::string("") );

	if( ! engine->save_file_as( ) ) {
		if( chkSound->get_active() )
			Gdk::Display::get_default()->beep();

		Gtk::MessageDialog( *view, "Error saving game.", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true).run();
		return;
	}

	status_bar->push( std::string("Saved game") );
}


/**-----------------------------------------------------------------------------
 * \brief Handle a user request to end the program
 *
 * We first ask the engine if we need to save data.
 * If not we can quit right away.
 * If we have unsaved data, pop up a box to confirm to lose the data
 */
void ChessController::on_action_quit()
{
	if( engine->can_quit() )
		quit();

	Gtk::MessageDialog dlg( *view, "Unsaved game. You really want to quit?", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK_CANCEL, true );
	if( dlg.run() == Gtk::RESPONSE_OK)
		quit();
}

/**-----------------------------------------------------------------------------
 * \brief Menu actions
 */
void ChessController::on_action_play()
{
	engine->advance(  );

    board->set_piece_positions( engine->get_piece_positions() );
    board->set_info( engine->get_info() );

}

/**-----------------------------------------------------------------------------
 * \brief Menu actions
 */
void ChessController::on_action_hint()
{
	board->highlight( engine->hint() );
}

/**-----------------------------------------------------------------------------
 * \brief Menu actions
 */
void ChessController::on_action_undo()
{
	engine->undo();
}

/**-----------------------------------------------------------------------------
 * \brief Menu actions
 */
void ChessController::on_action_redo()
{
	engine->redo();
}

/**-----------------------------------------------------------------------------
 * \brief Menu actions
 */
void ChessController::on_action_arrange()
{
	engine->arranging_start();

    board->set_piece_positions( engine->get_piece_positions() );

	view->show_menu( ChessWindow::MENU_ARRANGE );
	board->set_edit( true );
}

/**-----------------------------------------------------------------------------
 * \brief Menu actions
 */
void ChessController::on_action_twoplayer()
{
	view->show_player_option( engine->toggle_multiplayer() );
}

/**-----------------------------------------------------------------------------
 * \brief Menu actions
 */
void ChessController::on_action_demomode()
{
	engine->do_demo();
}

/**-----------------------------------------------------------------------------
 * \brief Menu actions
 */
void ChessController::on_action_piecevalues()
{
	engine->change_piece_values( );
}

/**-----------------------------------------------------------------------------
 * \brief Menu actions
 */
void ChessController::on_action_level( unsigned int level)
{
	if( ! chkLevelItems[level]->get_active() )
		return;

	engine->change_level( (eLevels)level );
}

/**-----------------------------------------------------------------------------
 * \brief Run the Colours Dialog
 *
 * Provide the user with a means to set the background, foreground and white and black tile colours
 */
void ChessController::on_action_colours()
{
    if( engine->choose_colours( ) )
		board->set_colours( engine->get_colours(), engine->get_info() );
}

/**-----------------------------------------------------------------------------
 * \brief Tell the view to turn the board 180 degrees
 */
void ChessController::on_action_reverse()
{
	board->toggle_reverse( );
}

/**-----------------------------------------------------------------------------
 * \brief Tell the view to toggle displaying the bestline information
 */
void ChessController::on_action_showbestline()
{
	board->toggle_bestline( engine->get_info() );
}

/**-----------------------------------------------------------------------------
 * \brief Show some application information
 *
 * Show our Help About... box
 */
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

/**-----------------------------------------------------------------------------
 * \brief Menu actions
 */
void ChessController::on_action_arrange_done()
{
	if( ! engine->arranging_end( false ) ) {
		if( chkSound->get_active() )
			Gdk::Display::get_default()->beep();

		Gtk::MessageDialog( *view, "This is not a valid board. Please fix.", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_CANCEL, true ).run();
		return;
	}

    board->set_piece_positions( engine->get_piece_positions() );

	view->show_menu( ChessWindow::MENU_GAME );
	board->set_edit( false );
}

/**-----------------------------------------------------------------------------
 * \brief Menu actions
 */
void ChessController::on_action_arrange_clear()
{
	engine->arranging_clear();

    board->set_piece_positions( engine->get_piece_positions() );
}

/**-----------------------------------------------------------------------------
 * \brief Menu actions
 */
void ChessController::on_action_arrange_turn( unsigned int turn )
{
	if( chkTurnItems[turn]->get_active() )
		engine->arrange_turn( (eTurns)turn );
}

/**-----------------------------------------------------------------------------
 * \brief Menu actions
 */
void ChessController::on_action_arrange_cancel()
{
	engine->arranging_end( true );

    board->set_piece_positions( engine->get_piece_positions( ) );

	view->show_menu( ChessWindow::MENU_GAME );
	board->set_edit( false );
}

/**-----------------------------------------------------------------------------
 * \brief Menu actions
 */
void ChessController::on_action_thinking_stop()
{
	engine->stop_thinking();
}


void ChessController::on_move_calculator_notify()
{
	if( move_calculator_thread->joinable() )
		move_calculator_thread->join();

	board->set_piece_positions( engine->get_piece_positions() );
    board->set_info( engine->get_info() );

	view->show_menu( ChessWindow::MENU_GAME );
}


void ChessController::move_calculator()
{
	engine->calculate_move();

	move_calculator_slot.emit();
}


/**-----------------------------------------------------------------------------
 * \brief
 */
bool ChessController::on_animate_timeout()
{
	if( ! --timeout_counter ) {
		board->animate_move_finish();

		STSquare end_square = board->get_end_square();
		char piece_code = board->get_piece_code();

		engine->put_piece_on_square( end_square, piece_code );
		board->set_piece_positions( engine->get_piece_positions() );

		// if it is the computers turn here, let the AI calculate a move
		view->show_menu( ChessWindow::MENU_STOP );

		move_calculator_thread = new std::thread( sigc::mem_fun(*this, &ChessController::move_calculator) );

		return false;
	}

	board->animate_move_continue();
	return true;
}

/**-----------------------------------------------------------------------------
 * \brief Button actions
 */
bool ChessController::on_board_button_released( GdkEventButton* button_event )
{
	STSquare start_square = board->get_drag_square();
	STSquare end_square = board->get_end_square();
	char piece_code = board->get_piece_code();

	if( start_square.file != -1 )
		engine->put_piece_on_square( start_square, ' ' ); // Putting a space is removing the piece

	if( engine->in_edit_mode() ) {
		engine->put_piece_on_square( end_square, piece_code );
		board->set_piece_positions( engine->get_piece_positions() );
		return true;
	}

	// regular move, check if this this move cannot be made
	if( ! engine->enter_move( start_square, end_square ) ) {
		board->set_piece_positions( engine->get_piece_positions() );
		return true;
	}

	board->animate_move_start( );

	timeout_counter = 10;
	Glib::signal_timeout().connect( sigc::mem_fun(*this, &ChessController::on_animate_timeout), 100 );

	return true;
}


