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

class TimeInputter;
class PieceValues;
class FilenameChooser;

class ChessAppBase;

class ChessWindow;
class ChessBoard;

class GUIColourChooser;
class GUITimeInputter;
class GUIPieceValues;
class GUIFilenameChooser;

/**-----------------------------------------------------------------------------
 * \brief Main application object
 *
 * The ChessController class is the controller of the MVC construct employed here
 * This class combines and regulates the existence of windows or views (the V in MVC,
 * the existence and status of the application data and logic (the M(odel) in MVC)
 *
 * The Gtk::Application class already has management functions for windows
 * so what we have to add is the model paradigm.
 */
class ChessController : public Gtk::Application
{
public:
	// Construction/destruction. Constructor is private, class is a terminal class
	virtual ~ChessController();

	// Instance creator as constructor is private
	static Glib::RefPtr<ChessController> create( ChessAppBase* director_init );

	// These functions are called from the logic
	void set_piece_positions( std::string FEN_string );
	void set_info( STInfo info );
    void start_thinking();
    void stop_thinking();
	void animate( STSquare start_square, STSquare end_square, char piece );
	void flash_square( STSquare square );

	TimeInputter* get_time_inputter();
	PieceValues * get_piece_valuer();
	FilenameChooser * get_openfile_chooser();
	FilenameChooser * get_savefile_chooser();

    // These four functions are call backs from the board
    void set_board( ChessBoard * board_init ) { board = board_init; };
	void put_piece_on_square( STSquare square, char piece );
	void make_move(  STSquare start_square, STSquare end_square );
	char get_piece( STSquare square );

private:
	// Private construction to ensure only references can be obtained. Private as class is terminal
	ChessController( ChessAppBase* director_init );

	virtual void on_startup();
	virtual void on_activate();

	bool on_animate_timeout();
	bool on_flash_timeout();

	// actions
	void on_action_not_implemented();

	void on_action_new();			// initialise the model to the empty condition
	void on_action_open();			// load the model from a file
	void on_action_save();			// save the model to a file
	void on_action_save_as();		// save the model to a new file
	void on_action_quit();			// quit application
	void on_action_play();
	void on_action_hint();

	void on_action_undo();
	void on_action_redo();
	void on_action_arrange();

	void on_action_level();

    void on_action_piecevalues();

	void on_action_colours();
    void on_action_reverse();
    void on_action_showbestline();

	void on_action_help_about();

	void on_action_arrange_done();
	void on_action_arrange_clear();
	void on_action_arrange_turn();
	void on_action_arrange_cancel();

	void on_action_thinking_stop();

	STColours app_colours;
	ChessAppBase* director;
	eLevels current_level;
	eTurns current_turn;
	int timeout_counter;


	// Widgets
	ChessWindow * view;
    Gtk::Statusbar * status_bar;
	Gtk::RadioMenuItem * chkLevel[LEVELCOUNT];
	Gtk::RadioMenuItem * chkTurn[TURNCOUNT];

    GUIColourChooser * dlgColourChooser;
    GUITimeInputter * dlgTimeInputter;
    GUIPieceValues * dlgPieceValues;
    GUIFilenameChooser * dlgOpenFile;
    GUIFilenameChooser * dlgSaveFile;

    ChessBoard * board;
};

#endif // APPCONTROLLER_H
