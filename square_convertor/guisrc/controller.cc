/*
 * controller.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include "controller.h"
#include "view.h"
 
#include "square_convertor_utils.h"

Controller::Controller() : Gtk::ApplicationWindow()
{
    set_title("Square Convertor");

    auto builder = Gtk::Builder::create_from_file("square_convertor.ui");

    view = Gtk::Builder::get_widget_derived<MainView>( builder, "MainView");
    set_child(*view);
    view->show();

	lblSAN  = builder->get_widget<Gtk::Label>( "lblSAN"  );
	lblUint = builder->get_widget<Gtk::Label>( "lblUint" );
	fldSAN  = builder->get_widget<Gtk::Entry>( "fldSAN"  );
	fldUint = builder->get_widget<Gtk::Entry>( "fldUint" );

	fldSAN->signal_changed().connect( [this](){ lblUint->set_text( SANtoUInt( fldSAN->get_text() ) ); } );
	fldUint->signal_changed().connect( [this](){ lblSAN->set_text( UInttoSAN( fldUint->get_text() ) ); } );
}
