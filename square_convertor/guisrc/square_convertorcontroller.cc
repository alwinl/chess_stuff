/*
 * square_convertor Copyright 2022 Alwin Leerling dna.leerling@gmail.com
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

#include "square_convertorengine.h"

#include "square_convertorcontroller.h"

int main( int argc, char * argv[] )
{
	square_convertorController().run( argc, argv );
}

square_convertorController::square_convertorController() : Gtk::Application( "net.dnatechnologies.square_convertor" )
{
	Glib::set_application_name("square_convertor");

    engine = new square_convertorEngine;
}

void square_convertorController::on_startup()
{
	Gtk::Application::on_startup();

    add_action("quit",                sigc::mem_fun( *this, &square_convertorController::on_action_quit         ) );
	add_action("about",               sigc::mem_fun( *this, &square_convertorController::on_action_about        ) );

    Glib::RefPtr<Gtk::Builder> ui_model = Gtk::Builder::create_from_resource( "/net/dnatechnologies/square_convertor/square_convertorui.glade" );

	ui_model->get_widget_derived("appView", view );
	ui_model->get_widget("lblSAN"  ,lblSAN  );
	ui_model->get_widget("lblUint" ,lblUint );
	ui_model->get_widget("fldSAN"  ,fldSAN  );
	ui_model->get_widget("fldUint" ,fldUint );

	fldSAN->signal_changed().connect( sigc::mem_fun(*this, &square_convertorController::SANtoUInt) );
	fldUint->signal_changed().connect( sigc::mem_fun(*this, &square_convertorController::UInttoSAN) );
	//Glib::signal_idle().connect( sigc::mem_fun(*this, &square_convertorController::on_idle) );
}

/*-----------------------------------------------------------------------------
 * The application has been started. Time to create and show a window
 * NB we could build multiple views here and activate (show) views
 * as needed
 */
void square_convertorController::on_activate()
{
	view->set_default_size(640,480);
	view->show_all_children();

	add_window( *view );
	view->show();

	view->set_icon( Gdk::Pixbuf::create_from_resource("/net/dnatechnologies/square_convertor/square_convertor.png") );
}

//-----------------------------------------------------------------------------
// Action implementation
//-----------------------------------------------------------------------------
void square_convertorController::on_action_quit()
{
	Gtk::MessageDialog dlg( *view, "You really want to quit?", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK_CANCEL, true );
	if( dlg.run() == Gtk::RESPONSE_OK)
		quit();
}

bool square_convertorController::on_idle()
{
	return true;
}

/**-----------------------------------------------------------------------------
 * \brief Show some application information
 *
 * Show our Help About... box
 */
void square_convertorController::on_action_about()
{
    Gtk::AboutDialog dlg;

    dlg.set_transient_for( *view ) ;
    dlg.set_name("square_convertor") ;
    dlg.set_logo( Gdk::Pixbuf::create_from_resource("/net/dnatechnologies/square_convertor/square_convertor.png") ) ;
    dlg.set_version( "0.01" ) ;
    dlg.set_comments("A GtkMM application") ;
    dlg.set_copyright( "Copyright © 2022 Alwin Leerling" ) ;
    dlg.set_website( "http://github" ) ;

    dlg.run();
}

void square_convertorController::SANtoUInt()
{
	std::string SAN = fldSAN->get_text();

	if( SAN.size() == 2 ) {

		unsigned int square = SAN[0] - 'a' + (SAN[1] - '1') * 8;

		lblUint->set_text( std::to_string( square ) );
	}
}

void square_convertorController::UInttoSAN()
{
	unsigned int square = std::atoi( fldUint->get_text().c_str() );

	std::string SAN;

	SAN.push_back( (square % 8) + 'a' );
	SAN.push_back( (square / 8) + '1' );

	lblSAN->set_text( SAN );
}

