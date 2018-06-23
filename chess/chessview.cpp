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
#include "chessapplication.h"
#include "pods.h"
#include "chessboard.h"
#include "chessinfo.h"
#include "chessedit.h"

/**-----------------------------------------------------------------------------
 * \brief Director of all screen real estate supplied to the application
 *
 * \param cobject BaseObjectType*
 * \param ui_model const Glib::RefPtr<Gtk::Builder>&
 * \param model_init AppModel&
 *
 */
ChessWindow::ChessWindow( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model, ChessApplication& app ) : Gtk::ApplicationWindow(cobject), edit_area_visible(false)
{
	set_default_size(640,480);
	set_icon( Gdk::Pixbuf::create_from_resource("/net/dnatechnologies/chess/chess.png") );

	ui_model->get_widget_derived("canvas", board_area, app );
	ui_model->get_widget_derived("widInfo", info_area );
	ui_model->get_widget_derived("widEdit", edit_area, app );

	show_all_children();

	edit_area->hide();
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

void ChessWindow::set_info( STInfo info )
{
    info_area->set_turn_text( info.turn );
    info_area->set_white_text( info.white );
    info_area->set_black_text(  info.black );
    info_area->set_time_text( info.time );
    info_area->set_level_text( info.level );
    info_area->set_value_text( info.value );
    info_area->set_nodes_text( info.nodes );
    info_area->set_n_sec_text( info.n_sec );
    info_area->set_depth_text( info.depth );
    info_area->set_bestline_text( info.bestline );
}

void ChessWindow::set_colours( STColours& colours )
{
    board_area->set_colours( Gdk::RGBA(colours.bg), Gdk::RGBA(colours.white), Gdk::RGBA(colours.black) );
    info_area->set_colours( Gdk::RGBA(colours.bg), Gdk::RGBA(colours.fg) );
    edit_area->set_colours( Gdk::RGBA(colours.bg) );
}

bool ChessWindow::reverse_board()
{
    return board_area->toggle_reverse();
}

bool ChessWindow::toggle_bestline_display()
{
    return info_area->toggle_bestline();
}

bool ChessWindow::toggle_edit_area_display( )
{
    if( edit_area_visible ) {
        edit_area->hide();
        info_area->show();
        edit_area_visible = false;
    } else {
        edit_area->show();
        info_area->hide();
        edit_area_visible = true;
    }

    return edit_area_visible;
}


