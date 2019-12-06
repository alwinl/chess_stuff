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
class ColourChooser;

class ChessAppBase;

class ChessWindow;
class ChessBoard;

/**-----------------------------------------------------------------------------
 * \brief Main application object
 *
 * The ChessController class is the GUI application class, as opposed to the main
 * application class. It's the top level object for the GUI and runs all communication
 * between the user and the real application. User input (menu selections, mouse
 * clicks etc) are translated to messages/object that make sense to the chess
 * engine. Instructions from the engine get interpreted here and appropriate GUI
 * actions are created and executed.
 */
class ChessController : public Gtk::Application
{
public:
	// Instance creator as constructor is private
	static Glib::RefPtr<ChessController> create( ChessAppBase* director_init );
	virtual ~ChessController();

	// These functions are called from the logic
	void set_piece_positions( std::string FEN_string );
	void set_info( STInfo info );
	void set_thinking( bool on );
	void animate( STSquare start_square, STSquare end_square, char piece );

    // These two functions are call backs from the board
	void put_piece_on_square( STSquare square, char piece );
	void make_move(  STSquare start_square, STSquare end_square );

private:
	// Non public construction to ensure only references can be obtained. Private as class is terminal
	ChessController( ChessAppBase* director_init );

	// Initialisation
	virtual void on_startup();
	virtual void on_activate();

	// Actions
	void on_action_new();
	void on_action_open();
	void on_action_save();
	void on_action_save_as();
	void on_action_quit();
	void on_action_play();
	void on_action_hint();
	void on_action_undo();
	void on_action_redo();
	void on_action_arrange();
	void on_action_level( unsigned int level );
	void on_action_twoplayer();
	void on_action_demomode();
    void on_action_piecevalues();
	void on_action_colours();
    void on_action_reverse();
    void on_action_showbestline();
	void on_action_help_about();
	void on_action_arrange_done();
	void on_action_arrange_clear();
	void on_action_arrange_turn( unsigned int turn );
	void on_action_arrange_cancel();
	void on_action_thinking_stop();

	void bind_actions();

	// Widgets
	ChessWindow * view;
    Gtk::Statusbar * status_bar;
	Gtk::RadioMenuItem * chkLevel[LEVELCOUNT];
	Gtk::RadioMenuItem * chkTurn[TURNCOUNT];
	Gtk::CheckMenuItem * chkSound;
    ChessBoard * board;
    ColourChooser * guiColourChooser;
    TimeInputter * guiTimeInputter;
    PieceValues * guiPieceValues;
    FilenameChooser * guiOpenFile;
    FilenameChooser * guiSaveFile;

	void get_widgets();

    // Data
	ChessAppBase* director;
	std::string filename;
};

#endif // APPCONTROLLER_H
