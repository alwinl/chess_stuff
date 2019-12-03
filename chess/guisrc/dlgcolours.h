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

#ifndef DLGCOLOURS_H
#define DLGCOLOURS_H

#include "../logicsrc/colourchooser.h"

#include <gtkmm.h>

class DialogColours;

/** \brief
 */
class GUIColourChooser : public ColourChooser
{
public:
    GUIColourChooser( Glib::RefPtr<Gtk::Builder>& ui_model, Gtk::Window& parent, STColours& init_colours );

protected:
    virtual void set_colours( STColours& colours );
    virtual bool manipulate_colours( );
    virtual STColours colours( );

private:
	DialogColours * dlg;
};

#endif // DLGCOLOURS_H
