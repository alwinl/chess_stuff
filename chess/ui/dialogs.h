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

using ColorArray = std::array<std::string,4>;

enum class eResult { OK, CANCEL, UNKNOWN };

class DialogColours : public Gtk::Window
{
public:
    DialogColours( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model );

    void set_colours( ColorArray colours );
    ColorArray get_colours( );

private:
    Gtk::ColorDialogButton * btnBackground;
    Gtk::ColorDialogButton * btnForeground;
    Gtk::ColorDialogButton * btnWhiteColour;
    Gtk::ColorDialogButton * btnBlackColour;

    Glib::RefPtr<Gtk::ColorDialog> colour_dialog;

    ColorArray initial_colours;

    eResult result = eResult::UNKNOWN;
};

class DialogPieceValues : public Gtk::Window
{
public:
    DialogPieceValues( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model );

	void set_values( std::map<char, int> values );
	std::map<char, int> get_values();

private:
	std::array<Gtk::SpinButton *,5> spnButton;

    std::array<int,5> orig_value;

    eResult result = eResult::UNKNOWN;
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

    eResult result = eResult::UNKNOWN;
};

class DialogNewGame : public Gtk::Window
{
public:
    DialogNewGame( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model );

    int get_choice() const { return choice; }

private:
	void make_choice( int the_choice ) { choice = the_choice; hide(); };

	int choice;

    eResult result = eResult::UNKNOWN;
};

class DialogLevel : public Gtk::Window
{
public:
    enum class eLevels {EASY, TIMED, TOTALTIME, INFINITE, PLYSEARCH, MATESEARCH, MATCHING, NONE };

    DialogLevel( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model );

    eLevels get_level() const { return choice; }

private:
	Gtk::CheckButton * chkLevelEasy;
	Gtk::CheckButton * chkLevelTimed;
	Gtk::CheckButton * chkLevelTotalTime;
	Gtk::CheckButton * chkLevelInfinite;
	Gtk::CheckButton * chkLevelPlySearch;
	Gtk::CheckButton * chkLevelMateSearch;
	Gtk::CheckButton * chkLevelMatching;
    Gtk::Entry * fldLevelTimed;
    Gtk::Entry * fldLevelTotalTime;

    void make_choice( eLevels the_choice ) { choice = the_choice; hide(); };

	eLevels choice;

    eResult result = eResult::UNKNOWN;
};

}

#endif // DIALOGS_H
