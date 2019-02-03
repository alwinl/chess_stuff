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
#include "dlgcolours.h"
#include "dlginput.h"
#include "dlgpiecevalues.h"

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
		chkLevel[level]->signal_activate().connect( sigc::mem_fun(*this, &ChessController::on_level_changed ) );
	}

	chkLevel[EASY]->set_active();
	current_level = EASY;

	vector<string> turn_widgets = { "chkTurnWhite", "chkTurnBlack" };
	for( unsigned int turn = TURNWHITE; turn < TURNCOUNT; ++turn ) {
		ui_model->get_widget( turn_widgets[turn], chkTurn[turn] );
		chkTurn[turn]->signal_activate().connect( sigc::mem_fun(*this, &ChessController::on_arrange_turn_changed ) );
	}

	chkTurn[TURNWHITE]->set_active();
	current_turn = TURNWHITE;

	dlgColourChooser = new GUIColourChooser( ui_model, *view );
	dlgTimeInputter = new GUITimeInputter( ui_model, *view );
	dlgPieceValues = new GUIPieceValues( ui_model, *view );
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
    view->set_colours( app_colours );

    on_action_new();
}


/**-----------------------------------------------------------------------------
 * \brief Place holder for actions we have not programmed yet
 */
void ChessController::on_action_not_implemented()
{
	message_dialog( "not implemented yet" );
}

/**-----------------------------------------------------------------------------
 * \brief engine dispatchers
 */
void ChessController::on_action_new() { director->new_game(  ); }

void ChessController::on_action_open() { director->open_file(  ); }

void ChessController::on_action_save() { director->save_file(  ); }

void ChessController::on_action_save_as() { director->save_as(  ); }

void ChessController::on_action_quit() { director->end_app(  ); }

void ChessController::on_action_play() { director->advance(  ); }

void ChessController::on_action_hint() { }

void ChessController::on_action_undo() { }

void ChessController::on_action_redo() { }

void ChessController::on_action_piecevalues() { director->piece_value_changes(  ); }


void ChessController::on_action_arrange() { director->arrange_start(); }


void ChessController::on_action_thinking_stop() {}

void ChessController::on_action_arrange_done() { director->arrange_end( false ); }
void ChessController::on_action_arrange_clear() { director->arrange_clear(); }
void ChessController::on_action_arrange_cancel() { director->arrange_end( true ); }


/**-----------------------------------------------------------------------------
 * \brief
 *
 * \return void
 *
 */
void ChessController::on_level_changed()
{
	for( unsigned int level = EASY; level < LEVELCOUNT; ++level ) {
		if( chkLevel[level]->get_active() ) {
			if( current_level != level ) {
				message_dialog( "level changed" );
				current_level = (eLevels)level;
			}
		}
	}
}



/**-----------------------------------------------------------------------------
 * \brief
 *
 * \return void
 *
 */
void ChessController::on_arrange_turn_changed()
{
	for( unsigned int turn = TURNWHITE; turn < TURNCOUNT; ++turn ) {
		if( chkTurn[turn]->get_active() ) {
			if( current_turn != turn ) {
				message_dialog( "turn changed" );
				current_turn = (eTurns)turn;
			}
		}
	}
}

/*
 * The next couple of functions do not need to be dispatched up to the engine.
 * These are purely GUI matters.
 */

/**-----------------------------------------------------------------------------
 * \brief Run the Colours Dialog
 *
 * Provide the user with a means to set the background, foreground and white and black tile colours
 */
void ChessController::on_action_colours()
{
    dlgColourChooser->choose_colours( app_colours );
    view->set_colours( app_colours );
}

/**-----------------------------------------------------------------------------
 * \brief Tell the view to turn the board 180 degrees
 */
void ChessController::on_action_reverse()
{
	view->reverse_board();
}

/**-----------------------------------------------------------------------------
 * \brief Tell the view to toggle displaying the bestline information
 */
void ChessController::on_action_showbestline()
{
    view->toggle_bestline_display();
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


/*
 * The next functions are called from the engine
 */

void ChessController::message_dialog( string message )
{
    Gtk::MessageDialog( *view, message, false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true).run();
}

void ChessController::push_statusbar_text( string message )
{
    status_bar->push( message );
}

void ChessController::set_piece_positions( std::string FEN_string, STInfo info )
{
    view->set_piece_positions( FEN_string );
    view->set_info( info );
}

STPieceValues ChessController::run_piece_value_dialog( STPieceValues current )
{
    dlgPieceValues->get_new_piece_values( current );

    return current;
}

void ChessController::start_edit_mode()
{
	view->set_edit_mode( true );
}

void ChessController::end_edit_mode()
{
	view->set_edit_mode( false );
}

/**-----------------------------------------------------------------------------
 * \brief Ask the user for the name of a chess file
 *
 * \param filename string filename suggestion, could be the name of previous opened file
 * \param working_dir string
 * \return string blank if cancelled, filename if not blank
 */
string ChessController::open_filename( string filename, string working_dir )
{
    Gtk::FileChooserDialog dlg( *view, "Restore chess game", Gtk::FILE_CHOOSER_ACTION_OPEN );

    dlg.add_button( Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL );
    dlg.add_button( Gtk::Stock::OPEN, Gtk::RESPONSE_ACCEPT );

    Glib::RefPtr<Gtk::FileFilter> filter_chess = Gtk::FileFilter::create();
    filter_chess->set_name("Chess Files");
    filter_chess->add_pattern("*.chess");
    dlg.add_filter( filter_chess );

    Glib::RefPtr<Gtk::FileFilter> filter_all = Gtk::FileFilter::create();
    filter_all->set_name("All Files");
    filter_all->add_pattern("*.*");
    dlg.add_filter( filter_all );

    dlg.set_filter( filter_chess );

    dlg.set_current_folder( working_dir );
    if( ! filename.empty() )
		dlg.set_current_name( filename );

    return ( dlg.run() != Gtk::RESPONSE_ACCEPT ) ? "" :  dlg.get_filename();
}

/**-----------------------------------------------------------------------------
 * \brief
 *
 * \param filename string
 * \param working_dir string
 * \return string
 *
 */
string ChessController::save_filename( string filename, string working_dir )
{
    Gtk::FileChooserDialog dlg( *view, "Save chess game", Gtk::FILE_CHOOSER_ACTION_SAVE );

    dlg.add_button( Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL );
    dlg.add_button( Gtk::Stock::SAVE, Gtk::RESPONSE_OK );

    Glib::RefPtr<Gtk::FileFilter> filter_chess = Gtk::FileFilter::create();
    filter_chess->set_name("Chess Files");
    filter_chess->add_pattern("*.chess");
    dlg.add_filter( filter_chess );
    dlg.set_filter( filter_chess );

    dlg.set_current_folder( working_dir );
    dlg.set_current_name( filename );

    return ( dlg.run() != Gtk::RESPONSE_OK ) ? "" :  dlg.get_filename();
}

#if 0
/**-----------------------------------------------------------------------------
 * \brief
 */
void ChessController::on_action_timed()
{
    std::pair<bool,int> retval = dlgTimeInputter->time_per_move();

    if( ! retval.first )
        return;

//    int sec_per_move = retval.second;
    // now we need to do something with sec_per_move.
}

/**-----------------------------------------------------------------------------
 * \brief
 */
void ChessController::on_action_totaltime()
{
    std::pair<bool,int> retval = dlgTimeInputter->total_game_time();

    if( ! retval.first )
        return;

//    int minutes_per_game = retval.second;
    // now we need to do something with minutes_per_game.
}
#endif // 0

