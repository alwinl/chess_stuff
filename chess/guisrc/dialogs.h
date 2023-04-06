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

class DialogColours : public Gtk::Dialog
{
public:
    DialogColours( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model, Gtk::Window& parent );

    void set_colours( std::array<std::string,4> colours );
    std::array<std::string,4> get_colours( );

private:
    Gtk::ColorButton * btnBackground;
    Gtk::ColorButton * btnForeground;
    Gtk::ColorButton * btnWhiteColour;
    Gtk::ColorButton * btnBlackColour;
};

class DialogPieceValues : public Gtk::Dialog
{
public:
    DialogPieceValues( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model, Gtk::Window& parent );

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

class DialogInput : public Gtk::Dialog
{
public:
    DialogInput( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model, Gtk::Window& parent );

    void dlg_setup( std::string title, std::string prompt, int value );
    void dlg_setup( std::string title, std::string prompt, std::string value );
    int get_input();

private:
    Gtk::Label * lblPrompt;
    Gtk::Entry * txtEntry;
};

class DialogNewGame : public Gtk::Dialog
{
public:
    DialogNewGame( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model, Gtk::Window& parent );

    int get_choice() const { return choice; }

private:
	void on_human_vs_AI();
	void on_AI_vs_human();
	void on_AI_vs_AI();
	void on_human_vs_human();

	int choice;
};

#endif // DIALOGS_H
