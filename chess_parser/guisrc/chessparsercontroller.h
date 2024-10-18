/*
 * ChessParser Copyright 2022 Alwin Leerling dna.leerling@gmail.com
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

#ifndef CHESSPARSERCONTROLLER_H
#define CHESSPARSERCONTROLLER_H

#include <gtkmm.h>

class ChessParserEngine;

/**
 *	\brief ApplicationWindow derived class
 *
 * Most of the time this class is a necessary evil. We need to have a derived
 * class to be able to call get_widget_derived. Nothing is actually done with
 * this class.
 */
class GUIView : public Gtk::ApplicationWindow
{
public:
	GUIView( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model ) : Gtk::ApplicationWindow(cobject) {};
};

/**
 *	\brief The main controller
 *
 * This class runs the complete GUI. It's the creator and destroyer of windows.
 * It takes all user input and decides what it means, and communicates with
 * the engine in order to get the right information painted on the widgets.
 */
class ChessParserController : public Gtk::Application
{
public:
	ChessParserController();

private:
	// Overrides
	virtual void on_startup();
	virtual void on_activate();

	// Actions
	void on_action_open();
	void on_action_quit();
	void on_action_about();

	bool on_idle();

	// Widgets
	GUIView * view;
	Gtk::Statusbar * status_bar;

	Glib::RefPtr<Gtk::TextBuffer> text_buffer;

	// Data
	std::unique_ptr<ChessParserEngine> engine;
};

#endif // GUICONTROLLER_H
