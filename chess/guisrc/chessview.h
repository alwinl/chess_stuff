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

#include <gtkmm.h>

class ChessController;
class ChessBoard;

/**-----------------------------------------------------------------------------
 * \brief The main application window
 *
 * This class manages the top level window
 */
class ChessWindow : public Gtk::ApplicationWindow
{
public:
	enum eMENUS { MENU_GAME, MENU_ARRANGE, MENU_STOP, MENU_COUNT };

	ChessWindow( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model );

	void show_menu( eMENUS menu );
	void show_player_option( bool multi_player );

private:
    Gtk::MenuBar * menus[MENU_COUNT];
    Gtk::MenuItem * mnuTwoplayer;
};

#endif // APPVIEW_H
