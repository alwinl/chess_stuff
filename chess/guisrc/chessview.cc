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

#include "chessview.h"
#include "chesscontroller.h"
#include "chessboard.h"

#include "../logicsrc/pods.h"

/**-----------------------------------------------------------------------------
 * \brief Director of all screen real estate supplied to the application
 *
 * \param cobject BaseObjectType*
 * \param ui_model const Glib::RefPtr<Gtk::Builder>&
 * \param model_init AppModel&
 *
 */
ChessWindow::ChessWindow( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model, ChessController& app )
			: Gtk::ApplicationWindow(cobject)
{
	set_default_size( 640,480 );
	set_icon( Gdk::Pixbuf::create_from_resource("/net/dnatechnologies/chess/chess.png") );

	ui_model->get_widget_derived("canvas", board_area, app );

    ui_model->get_widget( "mnuGame", mnuGame );
    ui_model->get_widget( "mnuArrange", mnuArrange );
    ui_model->get_widget( "mnuStop", mnuStop );

	show_all_children();

	mnuArrange->hide();
	mnuStop->hide();
}

/**-----------------------------------------------------------------------------
 * The following functions dispatch to the appropriate sub object
 */
void ChessWindow::set_piece_positions( std::string FEN_string )
{
    board_area->set_piece_positions( FEN_string );
}

void ChessWindow::set_drag_piece( char piece )
{
    board_area->set_drag_piece( piece );
}

void ChessWindow::set_info( STInfo& info )
{
	board_area->set_info( info );
}

void ChessWindow::set_colours( STColours& colours )
{
    board_area->set_colours( Gdk::RGBA(colours.bg), Gdk::RGBA(colours.white), Gdk::RGBA(colours.black), Gdk::RGBA(colours.fg) );
}

void ChessWindow::reverse_board()
{
    board_area->toggle_reverse();
}

void ChessWindow::toggle_bestline_display()
{
    board_area->toggle_bestline();
}

void ChessWindow::set_edit_mode( bool on )
{
	mnuGame->hide();
	mnuStop->hide();
	mnuArrange->hide();

	if( on )
		mnuArrange->show();
	else
		mnuGame->show();

	board_area->set_edit( on );
}

