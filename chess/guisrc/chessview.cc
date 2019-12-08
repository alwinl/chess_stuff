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

/**-----------------------------------------------------------------------------
 * \brief Director of all screen real estate supplied to the application
 *
 * \param cobject BaseObjectType*
 * \param ui_model const Glib::RefPtr<Gtk::Builder>&
 * \param controller ChessController&
 *
 */
ChessWindow::ChessWindow( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model )
			: Gtk::ApplicationWindow(cobject)
{
	set_default_size( 640,480 );
	set_icon( Gdk::Pixbuf::create_from_resource("/net/dnatechnologies/chess/chess.png") );

    ui_model->get_widget( "mnuGame", menus[MENU_GAME] );
    ui_model->get_widget( "mnuArrange", menus[MENU_ARRANGE] );
    ui_model->get_widget( "mnuStop", menus[MENU_STOP] );

    ui_model->get_widget( "mnuTwoplayer", mnuTwoplayer );

	show_all_children();

	menus[MENU_ARRANGE]->hide();
	menus[MENU_STOP]->hide();
}

/**-----------------------------------------------------------------------------
 * \brief Switches between the available menus
 */
void ChessWindow::show_menu( eMENUS menu )
{
	for( int i = 0; i< MENU_COUNT; ++i ) {
		if( i == menu )
			menus[i]->show();
		else
			menus[i]->hide();
	}
}

/**-----------------------------------------------------------------------------
 * \brief Switches between single player and multi player label
 */
void ChessWindow::show_player_option( bool multi_player )
{
	mnuTwoplayer->set_label( multi_player ? "_Single Player" : "_Two Player" );
}


