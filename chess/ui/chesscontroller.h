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
#include <iostream>

class ChessEngine;

namespace chess_gui {

class ChessWindow;
class ChessBoard;
class DialogColours;
class DialogPieceValues;
class DialogLevel;
class DialogInput;
class DialogNewGame;

class ChessController : public Gtk::Application
{
public:
	ChessController( );

private:
	enum class DlgID { NEW, PIECEVALUES, INPUT, COLOURS, LEVEL, NONE };

	// Setup
	void on_activate() override;
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
	void on_action_demomode();
	void on_action_level();
    void on_action_piecevalues();
	void on_action_colours();
    void on_action_reverse();
    void on_action_showbestline();
	void on_action_help_about();
	void on_action_arrange_clear();
	void on_action_arrange_cancel();
	void on_action_arrange_done();
	void on_action_arrange_turn_white();
	void on_action_arrange_turn_black();
	void on_action_arrange_make_fen();
	void on_action_thinking_stop();

	void init_action_arrange_turn();
	// void on_action_level_easy();
	// void on_action_level_timed();
	// void on_action_level_total_time();
	// void on_action_level_infinite();
	// void on_action_level_ply_search();
	// void on_action_level_mate_search();
	// void on_action_level_matching();

	void on_dialog_response( DlgID response );

	// Mouse input
	void on_drag_start( int n_press, double mouse_x, double mouse_y );
	void on_drag_done( int n_press, double mouse_x, double mouse_y );
	void on_drag_motion( double mouse_x, double mouse_y );

	// AI move calculator
	void move_calculator_start();
	void move_calculator_thread();
	void move_calculator_notify();

	// Animation of moves, highlights and demo
    void do_animate( uint16_t start_square, uint16_t end_square, char piece );
	bool on_animate_timeout();
	void do_highlight( uint16_t square );
	bool on_highlight_timeout();
    bool do_demo_move();
    bool on_demo_move_timeout();

	// Widgets
	Gtk::ApplicationWindow * view;
    ChessBoard * board;
    DialogLevel * dlgLevel;
    DialogPieceValues * dlgPieceValues;
    DialogColours * dlgColours;
    DialogInput * dlgTimeInput;
    DialogNewGame * dlgNewGame;
    Gtk::Label * status_bar;
	Gtk::AboutDialog * dlgAbout;
    Gtk::PopoverMenuBar * mnuGame;
    Gtk::PopoverMenuBar * mnuArrange;
    Gtk::PopoverMenuBar * mnuStop;

    // Data
	using ThreadMoveCalcType = std::unique_ptr<std::thread, std::function<void(std::thread*)>>;

	ThreadMoveCalcType thread_move_calculator = {nullptr, nullptr };
	Glib::Dispatcher slot_move_calculator;

	int timeout_counter;

	uint16_t drag_start_square;
	char drag_piece_code;

	uint16_t AI_start_square;
	uint16_t AI_end_square;
	char AI_piece;

    std::array<std::string,4> colours = {
		"rgb(78,154,6)",
		"rgb(0,0,0)",
		"rgb(238,238,236)",
		"rgb(85,87,83)",
	};

    bool is_demo = false;
	bool is_dragging = false;
	bool is_animating = false;
	bool is_computer_move = false;

    std::shared_ptr<ChessEngine> engine;
};

}

#endif // CHESSCONTROLLER_H
