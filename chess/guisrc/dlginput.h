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

#ifndef DLGINPUT_H
#define DLGINPUT_H

#include <string>

#include "../logicsrc/timeinputter.h"

#include <gtkmm.h>

class DialogInput;

/** \brief
 */
class GUITimeInputter : public TimeInputter
{
public:
    GUITimeInputter( Glib::RefPtr<Gtk::Builder>& ui_model, Gtk::Window& parent );

protected:
    virtual void set_up( const std::string& title, const std::string& prompt, int& value );
    virtual bool manipulate_data( );
    virtual int new_time( );

private:
    DialogInput * dlg;
};

#endif // DLGINPUT_H
