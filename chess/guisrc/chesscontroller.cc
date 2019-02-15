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

#include "../logicsrc/chessengine.h"
#include "../logicsrc/appmodel.h"
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

	dlgColourChooser = nullptr;
	dlgTimeInputter = nullptr;
	dlgPieceValues = nullptr;
    dlgOpenFile = nullptr;
    dlgSaveFile = nullptr;


	board = nullptr;


    app_colours.bg = "rgb(78,154,6)";
    app_colours.fg = "rgb(0,0,0)";
    app_colours.black = "rgb(85,87,83)";
    app_colours.white = "rgb(238,238,236)";
}

/**-----------------------------------------------------------------------------
 * \brief Application class destructor
 */
ChessController::~ChessController( )
{
	delete dlgColourChooser;
	delete dlgTimeInputter;
	delete dlgPieceValues;
    delete dlgOpenFile;
    delete dlgSaveFile;
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

    add_action("twoplayer",   sigc::mem_fun( *this, &ChessController::on_action_not_implemented ) );
    add_action("demomode",    sigc::mem_fun( *this, &ChessController::on_action_not_implemented ) );
    add_action("piecevalues", sigc::mem_fun( *this, &ChessController::on_action_piecevalues ) );
    add_action("drag",        sigc::mem_fun( *this, &ChessController::on_action_not_implemented ) );
    add_action("select",      sigc::mem_fun( *this, &ChessController::on_action_not_implemented ) );
    add_action("sound",       sigc::mem_fun( *this, &ChessController::on_action_not_implemented ) );

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
		chkLevel[level]->signal_activate().connect( sigc::mem_fun(*this, &ChessController::on_action_level ) );
	}

	chkLevel[EASY]->set_active();
	current_level = EASY;

	vector<string> turn_widgets = { "chkTurnWhite", "chkTurnBlack" };
	for( unsigned int turn = TURNWHITE; turn < TURNCOUNT; ++turn ) {
		ui_model->get_widget( turn_widgets[turn], chkTurn[turn] );
		chkTurn[turn]->signal_activate().connect( sigc::mem_fun(*this, &ChessController::on_action_arrange_turn ) );
	}

	chkTurn[TURNWHITE]->set_active();
	current_turn = TURNWHITE;

	dlgColourChooser = new GUIColourChooser( ui_model, *view );
	dlgTimeInputter = new GUITimeInputter( ui_model, *view );
	dlgPieceValues = new GUIPieceValues( ui_model, *view );
	dlgOpenFile = new GUIFilenameChooser( *view, Gtk::FILE_CHOOSER_ACTION_OPEN );
	dlgSaveFile = new GUIFilenameChooser( *view, Gtk::FILE_CHOOSER_ACTION_SAVE );
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

    if( board )
		board->set_colours( Gdk::RGBA(app_colours.bg), Gdk::RGBA(app_colours.white), Gdk::RGBA(app_colours.black), Gdk::RGBA(app_colours.fg) );

    on_action_new();
}

bool ChessController::on_animate_timeout()
{
	if( ! --timeout_counter ) {
		board->animate_stop();
		return false;
	}

	board->animate_step();
	return true;
}

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
 * \brief Place holder for actions we have not programmed yet
 */
void ChessController::on_action_not_implemented()
{
	Gtk::MessageDialog( *view, "not implemented yet", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true).run();
}

/**-----------------------------------------------------------------------------
 * \brief Menu actions
 */
void ChessController::on_action_new()
{
	director->new_game(  );

	status_bar->push( string("New game") );
}

void ChessController::on_action_open()
{
	std::string name = director->open_file(  );

	if( name.empty() )
		Gtk::MessageDialog( *view, "Error restoring game.", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true).run();
	else
		status_bar->push( string("Opened ") + name );
}

void ChessController::on_action_save()
{
	std::string name = director->save_file(  );

	if( name.empty() )
		Gtk::MessageDialog( *view, "Error saving game. Try Save As", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true).run();
	else
		status_bar->push( string("Saved ") + name );
}

void ChessController::on_action_save_as()
{
	std::string name = director->save_as(  );

	if( name.empty() )
		Gtk::MessageDialog( *view, "Error saving game.", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true).run();
	else
		status_bar->push( string("Saved ") + name );
}


void ChessController::on_action_quit() { director->end_app(  ); }
void ChessController::on_action_play() { director->advance(  ); }
void ChessController::on_action_hint() { director->hint(); }

void ChessController::on_action_undo() { director->undo(); }
void ChessController::on_action_redo() { director->redo(); }

void ChessController::on_action_arrange()
{
	director->arrange_start();

	view->show_menu( ChessWindow::MENU_ARRANGE );
	board->set_edit( true );
}

void ChessController::on_action_level()
{
	for( unsigned int level = EASY; level < LEVELCOUNT; ++level ) {
		if( chkLevel[level]->get_active() ) {
			if( current_level != level ) {
				director->change_level( (eLevels)level );
				current_level = (eLevels)level;
			}
		}
	}
}

void ChessController::on_action_piecevalues() { director->piece_value_changes(  ); }

/**-----------------------------------------------------------------------------
 * \brief Run the Colours Dialog
 *
 * Provide the user with a means to set the background, foreground and white and black tile colours
 */
void ChessController::on_action_colours()
{
    dlgColourChooser->choose_colours( app_colours );

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




void ChessController::on_action_arrange_done()
{
	director->arrange_end( false );

	view->show_menu( ChessWindow::MENU_GAME );
	board->set_edit( false );
}

void ChessController::on_action_arrange_clear() { director->arrange_clear(); }

void ChessController::on_action_arrange_turn()
{
	for( unsigned int turn = TURNWHITE; turn < TURNCOUNT; ++turn ) {
		if( chkTurn[turn]->get_active() ) {
			if( current_turn != turn ) {
				director->arrange_turn( (eTurns)turn );
				current_turn = (eTurns)turn;
			}
		}
	}
}

void ChessController::on_action_arrange_cancel()
{
	director->arrange_end( true );

	view->show_menu( ChessWindow::MENU_GAME );
	board->set_edit( false );
}

void ChessController::on_action_thinking_stop() { director->stop_thinking(); }

/**-----------------------------------------------------------------------------
 *  The next functions are call backs from the board
 */
void ChessController::put_piece_on_square( STSquare square, char piece )
{
	director->put_piece_on_square( square, piece );
}

void ChessController::make_move(  STSquare start_square, STSquare end_square )
{
	director->do_move( start_square, end_square );
}

char ChessController::get_piece( STSquare square ) { return director->get_piece( square); };

/**-----------------------------------------------------------------------------
 * The next functions are called from the engine
 */
void ChessController::set_piece_positions( std::string FEN_string )
	{ board->set_piece_positions( FEN_string ); }

void ChessController::set_info( STInfo info )
	{ board->set_info( info ); }

STPieceValues ChessController::run_piece_value_dialog( STPieceValues current )
	{ dlgPieceValues->get_new_piece_values( current ); return current; }

void ChessController::start_thinking()
{
	view->show_menu( ChessWindow::MENU_STOP );
}

void ChessController::stop_thinking()
{
	view->show_menu( ChessWindow::MENU_GAME );
}

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

void ChessController::flash_square( STSquare square )
{
	board->highlight_start( square );

	timeout_counter = 10;
	Glib::signal_timeout().connect( sigc::mem_fun(*this, &ChessController::on_flash_timeout), 100 );

	while( timeout_counter ) {
		while( Gtk::Main::instance()->events_pending() )
			Gtk::Main::instance()->iteration();
	}
}

TimeInputter* ChessController::get_time_inputter()
	{ return dlgTimeInputter; }

PieceValues * ChessController::get_piece_valuer()
	{ return dlgPieceValues; }

FilenameChooser * ChessController::get_openfile_chooser()
	{ return dlgOpenFile; }

FilenameChooser * ChessController::get_savefile_chooser()
	{ return dlgSaveFile; }
