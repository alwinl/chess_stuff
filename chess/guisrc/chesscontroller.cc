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
    add_action("hint",    sigc::mem_fun( *this, &ChessController::on_action_not_implemented ) );

    add_action("undo",    sigc::mem_fun( *this, &ChessController::on_action_not_implemented ) );
    add_action("redo",    sigc::mem_fun( *this, &ChessController::on_action_not_implemented ) );
    add_action("arrange", sigc::mem_fun( *this, &ChessController::on_action_arrange ) );

    add_action("easy",       sigc::mem_fun( *this, &ChessController::on_action_easy ) );
    add_action("timed",      sigc::mem_fun( *this, &ChessController::on_action_timed ) );
    add_action("total",      sigc::mem_fun( *this, &ChessController::on_action_totaltime ) );
    add_action("matching",   sigc::mem_fun( *this, &ChessController::on_action_matching ) );
    add_action("infinite",   sigc::mem_fun( *this, &ChessController::on_action_infinite ) );
    add_action("playsearch", sigc::mem_fun( *this, &ChessController::on_action_play_search ) );
    add_action("matesearch", sigc::mem_fun( *this, &ChessController::on_action_mate_search ) );

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

#if 0
    MenuItem "think_stop", CM_STOP
    MenuItem "edit_pop_done", EM_DONE
    MenuItem "edit_pop_clear", EM_CLEAR
    MenuItem "edit_pop_white_turn", IDM_WHITETURN, CHECKED
    MenuItem "edit_pop_black_turn", IDM_BLACKTURN
    MenuItem "edit_pop_cancel", EM_CANCEL
#endif // 0

    Glib::RefPtr<Gtk::Builder> ui_model = Gtk::Builder::create_from_resource( "/net/dnatechnologies/chess/appui.glade" );

	ui_model->get_widget_derived("main_view", view, *this );

    ui_model->get_widget( "widStatusBar", status_bar );
    ui_model->get_widget( "chkLevelEasy", chkLevelEasy );
    ui_model->get_widget( "chkLevelTimed", chkLevelTimed );
    ui_model->get_widget( "chkLevelTotalTime", chkLevelTotalTime );
    ui_model->get_widget( "chkLevelInfinite", chkLevelInfinite );
    ui_model->get_widget( "chkLevelPlaySearch", chkLevelPlaySearch );
    ui_model->get_widget( "chkLevelMateSearch", chkLevelMateSearch );

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

void ChessController::set_drag_piece( char piece )
{
    view->set_drag_piece( piece );
}


/**-----------------------------------------------------------------------------
 * \brief
 */
void ChessController::message_dialog( string message )
{
    Gtk::MessageDialog( *view, message, false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true).run();
}


/**-----------------------------------------------------------------------------
 * \brief
 *
 * \param message string
 * \return void
 */
void ChessController::push_statusbar_text( string message )
{
    status_bar->push( message );
}

/**-----------------------------------------------------------------------------
 * \brief
 */
void ChessController::set_piece_positions( std::string FEN_string, STInfo info )
{
    view->set_piece_positions( FEN_string );
    view->set_info( info );
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
void ChessController::drag_start( STSquare square )
    { director->start_move( square ); }

void ChessController::drag_end( STSquare square )
    { director->do_move( square ); }

void ChessController::drag_cancelled()
    { director->cancel_move(  ); }

void ChessController::select_edit_piece( char piece )
	{ director->select_edit_piece( piece ); }


void ChessController::on_action_new()
    { director->new_game(  ); }

void ChessController::on_action_open()
    { director->open_file(  ); }

void ChessController::on_action_save()
    { director->save_file(  ); }

void ChessController::on_action_save_as()
    { director->save_as(  ); }

void ChessController::on_action_quit()
    { director->end_app(  ); }

void ChessController::on_action_play()
    { director->advance(  ); }

void ChessController::on_action_piecevalues()
    { director->piece_value_changes(  ); }

/**-----------------------------------------------------------------------------
 * \brief Tell the view to toggle displaying the bestline information
 */
void ChessController::on_action_arrange()
{
	director->start_edit_mode();
    //view->toggle_edit_area_display();
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
 * \brief
 */
void ChessController::select_level( Gtk::CheckMenuItem * selected_item )
{
    /*
    chkLevelEasy->set_active(false);
    chkLevelTimed->set_active(false);
    chkLevelTotalTime->set_active(false);
    chkLevelInfinite->set_active(false);
    chkLevelPlaySearch->set_active(false);
    chkLevelMateSearch->set_active(false);
*/
    //selected_item->set_active(true);
}

/**-----------------------------------------------------------------------------
 * \brief
 */
void ChessController::on_action_easy()
{
	message_dialog( "on_action_easy not implemented yet" );

    select_level( chkLevelEasy );
}

/**-----------------------------------------------------------------------------
 * \brief
 */
void ChessController::on_action_timed()
{
    std::pair<bool,int> retval = dlgTimeInputter->time_per_move();

    if( ! retval.first )
        return;

    select_level( chkLevelTimed );

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

    select_level( chkLevelTotalTime );

//    int minutes_per_game = retval.second;
    // now we need to do something with minutes_per_game.
}

/**-----------------------------------------------------------------------------
 * \brief
 */
void ChessController::on_action_matching()
{
	message_dialog( "on_action_matching not implemented yet" );

}

/**-----------------------------------------------------------------------------
 * \brief
 */
void ChessController::on_action_infinite()
{
	message_dialog( "on_action_infinite not implemented yet" );

    select_level( chkLevelInfinite );
}

/**-----------------------------------------------------------------------------
 * \brief
 */
void ChessController::on_action_play_search()
{
	message_dialog( "on_action_play_search not implemented yet" );

    select_level( chkLevelPlaySearch );
}

/**-----------------------------------------------------------------------------
 * \brief
 */
void ChessController::on_action_mate_search()
{
	message_dialog( "on_action_mate_search not implemented yet" );

    select_level( chkLevelMateSearch );
}

/**-----------------------------------------------------------------------------
 * \brief
 */
STPieceValues ChessController::run_piece_value_dialog( STPieceValues current )
{
    dlgPieceValues->get_new_piece_values( current );

    return current;
}

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

