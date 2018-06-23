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

#include "dlginput.h"

#pragma GCC diagnostic ignored "-Wparentheses"
#include <gtkmm.h>
#pragma GCC diagnostic warning "-Wparentheses"

class DialogInput : public Gtk::Dialog
{
public:
    DialogInput( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model, Gtk::Window& parent );

    void dlg_setup( std::string title, std::string prompt, std::string value );
    std::string get_input();

private:
    Gtk::Label * lblPrompt;
    Gtk::Entry * txtEntry;
};



/** \brief
 *
 * \param cobject BaseObjectType*
 * \param ui_model const Glib::RefPtr<Gtk::Builder>&
 *
 */
DialogInput::DialogInput( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model, Gtk::Window& parent )
    : Gtk::Dialog( cobject )
{
    set_transient_for( parent );

	ui_model->get_widget( "lblInputPrompt", lblPrompt );
	ui_model->get_widget( "txtInputEntry", txtEntry );
}

void DialogInput::dlg_setup( std::string title, std::string prompt, std::string value )
{
    set_title( title );
    lblPrompt->set_text( prompt );
    txtEntry->set_text( value );
}

std::string DialogInput::get_input()
{
    return txtEntry->get_text();
}





/** \brief
 *
 * \param ui_model Glib::RefPtr<Gtk::Builder>&
 * \param parent Gtk::Window&
 *
 */
GUITimeInputter::GUITimeInputter( Glib::RefPtr<Gtk::Builder>& ui_model, Gtk::Window& parent )
{
	ui_model->get_widget_derived("dlgInput", dlg, parent );
}

void GUITimeInputter::set_up( const std::string& title, const std::string& prompt, int& value )
{
    dlg->dlg_setup( title, prompt, std::to_string( value) );
}

bool GUITimeInputter::manipulate_data( )
{
    int response = dlg->run();

    dlg->hide();

    return response == Gtk::RESPONSE_OK;
}

int GUITimeInputter::new_time( )
{
    return std::stoi( dlg->get_input() );
}


