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

#ifndef CHESSCONTROLLER_H
#define CHESSCONTROLLER_H

#include <gtkmm.h>

#include <string>
#include <vector>
#include <thread>
#include <array>

class ChessEngine;

class ChessWindow;
class ChessBoard;
class DialogColours;
class DialogPieceValues;
class DialogInput;
class DialogNewGame;

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

	// Setup
	virtual void on_startup();
	virtual void on_activate();
	void bind_actions();
	void connect_signals();
	void get_widgets();

	// Menu actions
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
	void on_action_twoplayer();
	void on_action_demomode();
    void on_action_piecevalues();
	void on_action_colours();
    void on_action_reverse();
    void on_action_showbestline();
	void on_action_help_about();
	void on_action_arrange_clear();
	void on_action_arrange_cancel();
	void on_action_arrange_done();
	void on_action_arrange_make_fen();
	void on_action_thinking_stop();

	void on_action_level_easy();
	void on_action_level_timed();
	void on_action_level_total_time();
	void on_action_level_infinite();
	void on_action_level_ply_search();
	void on_action_level_mate_search();
	void on_action_level_matching();

	void on_action_arrange_turn_white();
	void on_action_arrange_turn_black();

	// Mouse input
	bool on_drag_start( GdkEventButton* button_event );
	bool on_drag_done( GdkEventButton* button_event );

	// AI move calculator
	void move_calculator_start();
	void move_calculator_thread();
	void on_move_calculator_notify();

	// Animation of moves, highlights and demo
    void do_animate( uint16_t start_square, uint16_t end_square, char piece );
	bool on_animate_timeout();
	void do_highlight( uint16_t square );
	bool on_highlight_timeout();
    bool do_demo_move();
    bool on_demo_move_timeout();

	// Widgets
	ChessWindow * view;
    ChessBoard * board;
    DialogPieceValues * dlgPieceValues;
    DialogColours * dlgColours;
    DialogInput * dlgTimeInput;
    DialogNewGame * dlgNewGame;
    Gtk::Statusbar * status_bar;
	Gtk::RadioMenuItem *chkLevelEasy;
	Gtk::RadioMenuItem *chkLevelTimed;
	Gtk::RadioMenuItem *chkLevelTotalTime;
	Gtk::RadioMenuItem *chkLevelInfinite;
	Gtk::RadioMenuItem *chkLevelPlySearch;
	Gtk::RadioMenuItem *chkLevelMateSearch;
	Gtk::RadioMenuItem *chkLevelMatching;
    Gtk::MenuItem * mnuTwoplayer;
    Gtk::MenuBar * mnuGame;
    Gtk::MenuBar * mnuArrange;
    Gtk::MenuBar * mnuStop;
	Gtk::RadioMenuItem * chkTurnWhite;
	Gtk::RadioMenuItem * chkTurnBlack;
	Gtk::CheckMenuItem * chkSound;

    // Data
	std::thread * thread_move_calculator;
	Glib::Dispatcher slot_move_calculator;
	int timeout_counter;
	uint16_t drag_start_square;
	uint16_t drag_end_square;
	char drag_piece_code;
    std::array<std::string,4> colours;
    std::array<char, 64> save_board;

    ChessEngine* engine;
};

#endif // CHESSCONTROLLER_H
