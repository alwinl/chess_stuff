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
#include "dlginput.h"
#include "dlgpiecevalues.h"
#include "dlgfilenamechooser.h"

#include "../logicsrc/chessappbase.h"


using namespace std;

/**-----------------------------------------------------------------------------
 * \brief Create an instance of the application
 *
 * \return Glib::RefPtr<ChessController>
 */
Glib::RefPtr<ChessController> ChessController::create( ChessAppBase* director_init )
{
    return Glib::RefPtr<ChessController>( new ChessController( director_init ) );
}

/**-----------------------------------------------------------------------------
 * \brief Application class constructor
 */
ChessController::ChessController( ChessAppBase* director_init ) : Gtk::Application( "net.dnatechnologies.chess" ), director( director_init )
{
	Glib::set_application_name("GTKmm Chess");

	guiColourChooser = nullptr;
	guiTimeInputter = nullptr;
	guiPieceValues = nullptr;
    guiOpenFile = nullptr;
    guiSaveFile = nullptr;

	board = nullptr;

    app_colours.bg = "rgb(78,154,6)";
    app_colours.fg = "rgb(0,0,0)";
    app_colours.black = "rgb(85,87,83)";
    app_colours.white = "rgb(238,238,236)";

    sound_on = false;
}

/**-----------------------------------------------------------------------------
 * \brief Application class destructor
 */
ChessController::~ChessController( )
{
	delete guiColourChooser;
	delete guiTimeInputter;
	delete guiPieceValues;
    delete guiOpenFile;
    delete guiSaveFile;
}

/**-----------------------------------------------------------------------------
 * \brief Startup code
 */
void ChessController::on_startup()
{
    Gtk::Application::on_startup();

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
    add_action("sound",       sigc::mem_fun( *this, &ChessController::on_action_sound ) );

    add_action("colours",      sigc::mem_fun( *this, &ChessController::on_action_colours ) );
    add_action("reverse",      sigc::mem_fun( *this, &ChessController::on_action_reverse ) );
    add_action("showbestline", sigc::mem_fun( *this, &ChessController::on_action_showbestline ) );

    add_action("about",   sigc::mem_fun( *this, &ChessController::on_action_help_about ) );

    add_action("arrange_done", sigc::mem_fun( *this, &ChessController::on_action_arrange_done ) );
    add_action("arrange_clear", sigc::mem_fun( *this, &ChessController::on_action_arrange_clear ) );
    add_action("arrange_cancel",  sigc::mem_fun( *this, &ChessController::on_action_arrange_cancel ) );

    add_action("thinking_stop", sigc::mem_fun( *this, &ChessController::on_action_thinking_stop ) );

    Glib::RefPtr<Gtk::Builder> ui_model = Gtk::Builder::create_from_resource( "/net/dnatechnologies/chess/appui.glade" );

	ui_model->get_widget_derived("main_view", view, *this );
    ui_model->get_widget( "widStatusBar", status_bar );

	vector<string> level_widgets = { "chkLevelEasy", "chkLevelTimed", "chkLevelTotalTime", "chkLevelInfinite", "chkLevelPlaySearch", "chkLevelMateSearch" };
	for( unsigned int level = EASY; level < LEVELCOUNT; ++level ) {
		ui_model->get_widget( level_widgets[level], chkLevel[level] );
		chkLevel[level]->signal_activate().connect( sigc::bind<unsigned int>(sigc::mem_fun(*this, &ChessController::on_action_level ), level) );
	}

	chkLevel[EASY]->set_active();

	vector<string> turn_widgets = { "chkTurnWhite", "chkTurnBlack" };
	for( unsigned int turn = TURNWHITE; turn < TURNCOUNT; ++turn ) {
		ui_model->get_widget( turn_widgets[turn], chkTurn[turn] );
		chkTurn[turn]->signal_activate().connect( sigc::bind<unsigned int>(sigc::mem_fun(*this, &ChessController::on_action_arrange_turn ), turn) );
	}

	chkTurn[TURNWHITE]->set_active();

	guiColourChooser = new GUIColourChooser( ui_model, *view );
	guiTimeInputter = new GUITimeInputter( ui_model, *view );
	guiPieceValues = new GUIPieceValues( ui_model, *view );
	guiOpenFile = new GUIFilenameChooser( *view, Gtk::FILE_CHOOSER_ACTION_OPEN );
	guiSaveFile = new GUIFilenameChooser( *view, Gtk::FILE_CHOOSER_ACTION_SAVE );
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

	board->set_colours( Gdk::RGBA(app_colours.bg), Gdk::RGBA(app_colours.white), Gdk::RGBA(app_colours.black), Gdk::RGBA(app_colours.fg) );

    on_action_new();
}

/**-----------------------------------------------------------------------------
 * \brief
 */
bool ChessController::on_animate_timeout()
{
	if( ! --timeout_counter ) {
		board->animate_stop();
		return false;
	}

	board->animate_step();
	return true;
}

/**-----------------------------------------------------------------------------
 * \brief
 */
bool ChessController::on_flash_timeout()
{
	static bool highlight_on = false;

	if( ! --timeout_counter ) {
		highlight_on = false;
		board->highlight_flash( highlight_on );
		return false;
	}

	highlight_on = !highlight_on;
	board->highlight_flash( highlight_on );
	return true;
}


/**-----------------------------------------------------------------------------
 * \brief Menu actions
 */
void ChessController::on_action_new()
{
	director->new_game(  );

	status_bar->push( string("New game") );
}

/**-----------------------------------------------------------------------------
 * \brief Menu actions
 */
void ChessController::on_action_open()
{
	pair<bool,string> result = guiOpenFile->get_filename( "", "~/" );

	if( ! result.first )
		return;

	if( ! director->open_file( result.second ) ) {
		if( sound_on )
			Gdk::Display::get_default()->beep();

		Gtk::MessageDialog( *view, "Error restoring game.", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true).run();
		return;
	}

	status_bar->push( string("Opened ") + result.second );
}

/**-----------------------------------------------------------------------------
 * \brief Menu actions
 */
void ChessController::on_action_save()
{
	if( filename.empty() )
		on_action_save_as();

	if( ! director->save_file( filename ) ) {
		if( sound_on )
			Gdk::Display::get_default()->beep();

		Gtk::MessageDialog( *view, "Error saving game. Try Save As", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true).run();
		return;
	}

	status_bar->push( string("Saved ") + filename );
}

/**-----------------------------------------------------------------------------
 * \brief Menu actions
 */
void ChessController::on_action_save_as()
{
	pair<bool,string> result = guiSaveFile->get_filename( filename, "~/" );

	if( !result.first )
		return;

	if( result.second.find( ".chess") == string::npos )     // no .chess added
		result.second += string(".chess");

	if( ! director->save_file( result.second ) ) {
		if( sound_on )
			Gdk::Display::get_default()->beep();

		Gtk::MessageDialog( *view, "Error saving game.", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true).run();
		return;
	}

	filename = result.second;
	status_bar->push( string("Saved ") + filename );
}


/**-----------------------------------------------------------------------------
 * \brief Handle a user request to end the program
 *
 * We first ask the director if we need to save data.
 * If not we can quit right away.
 * If we have unsaved data, pop up a box to confirm to lose the data
 */
void ChessController::on_action_quit()
{
	if( director->can_quit() )
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
	director->advance(  );
}

/**-----------------------------------------------------------------------------
 * \brief Menu actions
 */
void ChessController::on_action_hint()
{
	STSquare square = director->hint();

	board->highlight_start( square );

	timeout_counter = 10;
	Glib::signal_timeout().connect( sigc::mem_fun(*this, &ChessController::on_flash_timeout), 100 );

	while( timeout_counter ) {
		while( Gtk::Main::instance()->events_pending() )
			Gtk::Main::instance()->iteration();
	}
}

/**-----------------------------------------------------------------------------
 * \brief Menu actions
 */
void ChessController::on_action_undo()
{
	director->undo();
}

/**-----------------------------------------------------------------------------
 * \brief Menu actions
 */
void ChessController::on_action_redo()
{
	director->redo();
}

/**-----------------------------------------------------------------------------
 * \brief Menu actions
 */
void ChessController::on_action_arrange()
{
	director->arrange_start();

	view->show_menu( ChessWindow::MENU_ARRANGE );
	board->set_edit( true );
}

/**-----------------------------------------------------------------------------
 * \brief Menu actions
 */
void ChessController::on_action_twoplayer()
{
	view->show_player_option( director->toggle_multiplayer() );
}

/**-----------------------------------------------------------------------------
 * \brief Menu actions
 */
void ChessController::on_action_demomode()
{
	director->do_demo();
}

/**-----------------------------------------------------------------------------
 * \brief Menu actions
 */
void ChessController::on_action_piecevalues()
{
	director->change_piece_values( guiPieceValues );
}

/**-----------------------------------------------------------------------------
 * \brief Menu actions
 */
void ChessController::on_action_sound()
{
	sound_on = !sound_on;
}

/**-----------------------------------------------------------------------------
 * \brief Menu actions
 */
void ChessController::on_action_level( unsigned int level)
{
	int time_parameter = 0;

	if( (eLevels)level == TIMED ) {
	    std::pair<bool,int> retval = guiTimeInputter->time_per_move( 120 );

		if( ! retval.first )
			return;

		time_parameter = retval.second;
	}

	if( (eLevels)level == TOTALTIME ) {
	    std::pair<bool,int> retval = guiTimeInputter->total_game_time( 60 );

		if( ! retval.first )
			return;

		time_parameter = retval.second;
	}

	director->change_level( (eLevels)level, time_parameter );
}

/**-----------------------------------------------------------------------------
 * \brief Run the Colours Dialog
 *
 * Provide the user with a means to set the background, foreground and white and black tile colours
 */
void ChessController::on_action_colours()
{
    pair<bool, STColours> result = guiColourChooser->choose_colours( app_colours );

    if( !result.first )
		return;

	app_colours = result.second;

    board->set_colours( Gdk::RGBA(app_colours.bg), Gdk::RGBA(app_colours.white), Gdk::RGBA(app_colours.black), Gdk::RGBA(app_colours.fg) );
}

/**-----------------------------------------------------------------------------
 * \brief Tell the view to turn the board 180 degrees
 */
void ChessController::on_action_reverse()
	{ board->toggle_reverse(); }

/**-----------------------------------------------------------------------------
 * \brief Tell the view to toggle displaying the bestline information
 */
void ChessController::on_action_showbestline()
	{ board->toggle_bestline(); }

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
	if( ! director->arrange_end( false ) ) {
		if( sound_on )
			Gdk::Display::get_default()->beep();

		Gtk::MessageDialog( *view, "This is not a valid board. Please fix.", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_CANCEL, true ).run();
		return;
	}

	view->show_menu( ChessWindow::MENU_GAME );
	board->set_edit( false );
}

/**-----------------------------------------------------------------------------
 * \brief Menu actions
 */
void ChessController::on_action_arrange_clear() { director->arrange_clear(); }

/**-----------------------------------------------------------------------------
 * \brief Menu actions
 */
void ChessController::on_action_arrange_turn( unsigned int turn )
{
	director->arrange_turn( (eTurns)turn );
}

/**-----------------------------------------------------------------------------
 * \brief Menu actions
 */
void ChessController::on_action_arrange_cancel()
{
	director->arrange_end( true );

	view->show_menu( ChessWindow::MENU_GAME );
	board->set_edit( false );
}

/**-----------------------------------------------------------------------------
 * \brief Menu actions
 */
void ChessController::on_action_thinking_stop()
	{ director->stop_thinking(); }

/*-----------------------------------------------------------------------------
 *  The next functions are call backs from the board
 */

/**-----------------------------------------------------------------------------
 * \brief
 */
void ChessController::put_piece_on_square( STSquare square, char piece )
{
	director->put_piece_on_square( square, piece );
}

/**-----------------------------------------------------------------------------
 * \brief
 */
void ChessController::make_move(  STSquare start_square, STSquare end_square )
{
	director->do_move( start_square, end_square );
}

/*-----------------------------------------------------------------------------
 * The next functions are called from the engine
 */

/**-----------------------------------------------------------------------------
 * \brief
 */
void ChessController::set_piece_positions( std::string FEN_string )
	{ board->set_piece_positions( FEN_string ); }

/**-----------------------------------------------------------------------------
 * \brief
 */
void ChessController::set_info( STInfo info )
	{ board->set_info( info ); }

/**-----------------------------------------------------------------------------
 * \brief
 */
void ChessController::set_thinking( bool on )
{
	view->show_menu( on ? ChessWindow::MENU_STOP : ChessWindow::MENU_GAME );
}

/**-----------------------------------------------------------------------------
 * \brief
 */
void ChessController::animate( STSquare start_square, STSquare end_square, char piece )
{
	board->animate_start( start_square, end_square, piece );

	timeout_counter = 10;
	Glib::signal_timeout().connect( sigc::mem_fun(*this, &ChessController::on_animate_timeout), 100 );

	while( timeout_counter ) {
		while( Gtk::Main::instance()->events_pending() )
			Gtk::Main::instance()->iteration();
	}
}

