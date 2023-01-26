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
#include <vector>
#include <thread>

#include <gtkmm.h>

class ChessEngine;

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
	ChessController( );
	virtual ~ChessController();

private:

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

	bool on_board_button_released( GdkEventButton* button_event );

	void bind_actions();

	void on_move_calculator_notify();
	void move_calculator();
	std::thread * move_calculator_thread;
	Glib::Dispatcher move_calculator_slot;

	bool on_animate_timeout();
	int timeout_counter;

	// Widgets
	ChessWindow * view;
    Gtk::Statusbar * status_bar;
	std::vector<Gtk::RadioMenuItem *> chkLevelItems;
	std::vector<Gtk::RadioMenuItem *> chkTurnItems;
	Gtk::CheckMenuItem * chkSound;
    ChessBoard * board;

	void get_widgets();

    // Data
    ChessEngine* engine;
};

#endif // APPCONTROLLER_H
