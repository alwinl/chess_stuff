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

#include "dlgcolours.h"

/** \brief
 */
class DialogColours : public Gtk::Dialog
{
public:
    DialogColours( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model, Gtk::Window& parent );

    void set_colours( ColourChooser::STColours& colours );
    ColourChooser::STColours get_colours( );

private:
    Gtk::ColorButton * btnBackground;
    Gtk::ColorButton * btnForeground;
    Gtk::ColorButton * btnWhiteColour;
    Gtk::ColorButton * btnBlackColour;
};

/** \brief
 *
 * \param cobject BaseObjectType*
 * \param ui_model const Glib::RefPtr<Gtk::Builder>&
 * \param parent Gtk::Window&
 *
 */
DialogColours::DialogColours( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model, Gtk::Window& parent )
    : Gtk::Dialog( cobject )
{
    set_transient_for( parent );

	ui_model->get_widget( "btnBackgroundColour", btnBackground );
	ui_model->get_widget( "btnForegroundColour", btnForeground );
	ui_model->get_widget( "btnWhiteColour", btnWhiteColour );
	ui_model->get_widget( "btnBlackColour", btnBlackColour );
}

void DialogColours::set_colours( ColourChooser::STColours& colours )
{
    btnBackground->set_rgba( Gdk::RGBA(colours.bg) );
    btnForeground->set_rgba( Gdk::RGBA(colours.fg) );
    btnWhiteColour->set_rgba( Gdk::RGBA(colours.white) );
    btnBlackColour->set_rgba( Gdk::RGBA(colours.black) );
}

ColourChooser::STColours DialogColours::get_colours( )
{
	ColourChooser::STColours colors;

    colors.bg    = btnBackground->get_rgba().to_string();
    colors.fg    = btnForeground->get_rgba().to_string();
    colors.white = btnWhiteColour->get_rgba().to_string();
    colors.black = btnBlackColour->get_rgba().to_string();

    return colors;
}




/** \brief
 *
 * \param ui_model Glib::RefPtr<Gtk::Builder>&
 * \param parent Gtk::Window&
 *
 */
GUIColourChooser::GUIColourChooser( Glib::RefPtr<Gtk::Builder>& ui_model, Gtk::Window& parent )
{
    ui_model->get_widget_derived( "dlgColours", dlg, parent );
}

void GUIColourChooser::set_colours( STColours& colours )
{
    dlg->set_colours( colours );
}

bool GUIColourChooser::manipulate_colours( )
{
    int response = dlg->run();

    dlg->hide();

    return response == Gtk::RESPONSE_OK;
}

ColourChooser::STColours GUIColourChooser::colours( )
{
    STColours colours;

    colours = dlg->get_colours(  );

    return colours;
}
