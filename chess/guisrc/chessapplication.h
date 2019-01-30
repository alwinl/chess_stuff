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

#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include <string>

#include <gtkmm.h>

#include "../logicsrc/pods.h"
//class STSquare;
//class STInfo;
//class STPieceValues;
//class STColours;

class ChessAppBase;

class ChessWindow;
class GUIColourChooser;
class GUITimeInputter;
class GUIPieceValues;

/**-----------------------------------------------------------------------------
 * \brief Main application object
 *
 * The ChessApplication class is the controller of the MVC construct employed here
 * This class combines and regulates the existence of windows or views (the V in MVC,
 * the existence and status of the application data and logic (the M(odel) in MVC)
 *
 * The Gtk::Application class already has management functions for windows
 * so what we have to add is the model paradigm.
 */
class ChessApplication : public Gtk::Application
{
public:
	// Construction/destruction. Constructor is private, class is a terminal class
	virtual ~ChessApplication();

	// Instance creator as constructor is private
	static Glib::RefPtr<ChessApplication> create( ChessAppBase* director_init );

	void drag_start( STSquare square );
	void drag_end( STSquare square );
	void drag_cancelled();

    void set_piece_positions( std::string FEN_string, STInfo info );
    void set_drag_piece( char piece );
    void push_statusbar_text( std::string message );
    void message_dialog( std::string message );
    STPieceValues run_piece_value_dialog( STPieceValues current );

    std::string open_filename( std::string filename, std::string working_dir );
    std::string save_filename( std::string filename, std::string working_dir );

private:
	// Private construction to ensure only references can be obtained. Private as class is terminal
	ChessApplication( ChessAppBase* director_init );

	virtual void on_startup();
	virtual void on_activate();

	// actions
	void on_action_not_implemented();
	void on_action_new();			// initialise the model to the empty condition
	void on_action_open();			// load the model from a file
	void on_action_save();			// save the model to a file
	void on_action_save_as();		// save the model to a new file
	void on_action_quit();			// quit application
	void on_action_play();
	void on_action_arrange();
	void on_action_easy();
	void on_action_timed();
	void on_action_totaltime();
    void on_action_matching();
    void on_action_infinite();
    void on_action_play_search();
    void on_action_mate_search();
    void on_action_piecevalues();
	void on_action_colours();
    void on_action_reverse();
    void on_action_showbestline();
	void on_action_help_about();

    void select_level( Gtk::CheckMenuItem * selected_item );

	STColours app_colours;
	ChessAppBase* director;

	// Widgets
	ChessWindow * view;
    Gtk::Statusbar * status_bar;
    Gtk::CheckMenuItem * chkLevelEasy;
    Gtk::CheckMenuItem * chkLevelTimed;
    Gtk::CheckMenuItem * chkLevelTotalTime;
    Gtk::CheckMenuItem * chkLevelInfinite;
    Gtk::CheckMenuItem * chkLevelPlaySearch;
    Gtk::CheckMenuItem * chkLevelMateSearch;
    GUIColourChooser * dlgColourChooser;
    GUITimeInputter * dlgTimeInputter;
    GUIPieceValues * dlgPieceValues;
};

#endif // APPCONTROLLER_H
