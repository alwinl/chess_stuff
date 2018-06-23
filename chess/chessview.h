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

#ifndef APPVIEW_H
#define APPVIEW_H

#include <string>

#pragma GCC diagnostic ignored "-Wparentheses"
#include <gtkmm.h>
#pragma GCC diagnostic warning "-Wparentheses"

class ChessApplication;

class ChessBoard;
class ChessInfo;
class ChessEdit;

struct STColours;
struct STInfo;

//-----------------------------------------------------------------------------
// The View class
//
//-----------------------------------------------------------------------------
class ChessWindow : public Gtk::ApplicationWindow
{
public:
	ChessWindow( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model, ChessApplication& app );

	void set_piece_positions( std::string FEN_string );
	void set_drag_piece( char piece );
	void set_info( STInfo info );
	void set_colours( STColours& colours );
	bool reverse_board();
	bool toggle_bestline_display();
    bool toggle_edit_area_display( );

private:
	ChessInfo * info_area;
	ChessBoard * board_area;
	ChessEdit * edit_area;

	bool edit_area_visible;
};

#endif // APPVIEW_H
