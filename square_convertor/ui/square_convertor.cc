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

#include "square_convertor_utils.h"

#include <gtkmm.h>

class SquareConvertor : public Gtk::ApplicationWindow
{
public:
    SquareConvertor();

private:
    Gtk::Grid * view;

	Gtk::Label *lblSAN;
	Gtk::Label *lblUint;
	Gtk::Entry *fldSAN;
	Gtk::Entry *fldUint;
};


SquareConvertor::SquareConvertor() : Gtk::ApplicationWindow()
{
    set_title("Square Convertor");

	std::string ui_file = "square_convertor.ui";  // local build
	if( !Glib::file_test(ui_file, Glib::FileTest::EXISTS) )
		ui_file = std::string(DATADIR) + "/Chess_Stuff/ui/square_convertor.ui";

	auto builder = Gtk::Builder::create_from_file(ui_file);

    view = builder->get_widget<Gtk::Grid>( "MainView");
    set_child(*view);
    view->show();

	lblSAN  = builder->get_widget<Gtk::Label>( "lblSAN"  );
	lblUint = builder->get_widget<Gtk::Label>( "lblUint" );
	fldSAN  = builder->get_widget<Gtk::Entry>( "fldSAN"  );
	fldUint = builder->get_widget<Gtk::Entry>( "fldUint" );

	fldSAN->signal_changed().connect( [this](){ lblUint->set_text( SANtoUInt( fldSAN->get_text() ) ); } );
	fldUint->signal_changed().connect( [this](){ lblSAN->set_text( UInttoSAN( fldUint->get_text() ) ); } );
}

int main( int argc, char * argv[] )
{
    auto app = Gtk::Application::create();

    return app->make_window_and_run<SquareConvertor>( argc, argv );
}
