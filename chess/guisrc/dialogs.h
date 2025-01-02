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

#ifndef DIALOGS_H
#define DIALOGS_H

#include <gtkmm.h>

#include <array>
#include <string>
#include <map>

namespace chess_gui {


class DialogColours : public Gtk::Window
{
public:
    DialogColours( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model );

    void set_colours( std::array<std::string,4> colours );
    std::array<std::string,4> get_colours( );

private:
    Gtk::ColorButton * btnBackground;
    Gtk::ColorButton * btnForeground;
    Gtk::ColorButton * btnWhiteColour;
    Gtk::ColorButton * btnBlackColour;
};

class DialogPieceValues : public Gtk::Window
{
public:
    DialogPieceValues( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model );

	void set_values( std::map<char, int> values );
	std::map<char, int> get_values();

private:
    void on_revert_clicked();

	struct PieceData {
		Gtk::SpinButton * spnButton;
		int orig_value;
	};

	std::array<PieceData,5> piece_data;
};

class DialogInput : public Gtk::Window
{
public:
    DialogInput( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model );

    void dlg_setup( std::string title, std::string prompt, int value );
    void dlg_setup( std::string title, std::string prompt, std::string value );
    int get_input();

private:
    Gtk::Label * lblPrompt;
    Gtk::Entry * txtEntry;
};

class DialogNewGame : public Gtk::Window
{
public:
    DialogNewGame( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model );

    int get_choice() const { return choice; }

private:
	void make_choice( int the_choice ) { choice = the_choice; /*response( Gtk::ResponseType::RESPONSE_OK );*/ };

	int choice;
};

}

#endif // DIALOGS_H
