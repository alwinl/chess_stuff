/*
 * chess_window.h Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#pragma once

#include <gtkmm.h>

#include "board_view.h"
#include "dialogs.h"

class ChessEngine
{
public:
    bool load_game( std::string filepath ) { return true; }
    bool save_game( std::string filepath ) { return filepath != ""; }
};

class ChessWindow : public Gtk::ApplicationWindow
{
public:
	ChessWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);

private:
    Gtk::PopoverMenuBar * mnuGame;
    Gtk::PopoverMenuBar * mnuArrange;
    Gtk::PopoverMenuBar * mnuStop;
    BoardView * board_view;
    Gtk::Label * status_bar;

    std::unique_ptr<ChessEngine> engine;

    void init_board_data();
    void init_file_dialog( );
    void init_action_newgame( chess_gui::DialogNewGame* dlg );
    void init_action_piecevalues( chess_gui::DialogPieceValues* dlg );
    void init_action_colours( chess_gui::DialogColours* dlg );
    void init_action_level( chess_gui::DialogLevel* dlg );
    void init_action_simple();
    void init_action_arrange_turn();
    void init_mouse_events();

    void on_action_play();
    void on_action_hint();
    void on_action_undo();
    void on_action_redo();
    void on_action_arrange();
    void on_action_reverse();
    void on_action_showbestline();
    void on_action_demomode();

    void on_drag_start( int n_press, double mouse_x, double mouse_y );
    void on_drag_motion( double mouse_x, double mouse_y );
    void on_drag_done( int n_press, double mouse_x, double mouse_y );

    void on_action_arrange_clear();
    void on_action_arrange_done();
    void on_action_arrange_cancel();
    void on_action_arrange_make_fen();
    
    void on_action_thinking_stop();

};

