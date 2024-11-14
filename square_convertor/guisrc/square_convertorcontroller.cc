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

    engine = std::make_unique<square_convertorEngine>();
}

void square_convertorController::on_activate()
{
	add_action( "about", sigc::mem_fun( *this, &square_convertorController::on_action_about ) );

	auto refBuilder = Gtk::Builder::create();

	refBuilder->add_from_resource( "/net/dnatechnologies/square_convertor/square_convertor.ui");

	Gtk::ApplicationWindow * win = refBuilder->get_widget<Gtk::ApplicationWindow>("appView");
	lblSAN  = refBuilder->get_widget<Gtk::Label>( "lblSAN"  );
	lblUint = refBuilder->get_widget<Gtk::Label>( "lblUint" );
	fldSAN  = refBuilder->get_widget<Gtk::Entry>( "fldSAN"  );
	fldUint = refBuilder->get_widget<Gtk::Entry>( "fldUint" );

	fldSAN->signal_changed().connect( [this](){ lblUint->set_text( engine->SANtoUInt( fldSAN->get_text() ) ); } );
	fldUint->signal_changed().connect( [this](){ lblSAN->set_text( engine->UInttoSAN( fldUint->get_text() ) ); } );

    win->signal_close_request().connect( sigc::mem_fun( *this, &square_convertorController::on_action_quit ), true );

	add_window( *win );
    win->show();
}

bool square_convertorController::on_action_quit()
{
    auto dlg = Gtk::AlertDialog::create("Do you really want to quit?" );

    dlg->set_buttons( {"Yes", "No"} );
    dlg->set_modal(true);

    auto choice_fun = [this,dlg]( std::shared_ptr<Gio::AsyncResult>& async_result )
    {
        if( dlg->choose_finish( async_result ) == 0 )
            quit();
    };

    dlg->choose( *get_active_window(), choice_fun );

    return true;
}

void square_convertorController::on_action_about()
{
	if( about_dlg == nullptr ) {

		about_dlg = std::make_unique<Gtk::AboutDialog>();

		about_dlg->set_transient_for(*get_active_window());
		about_dlg->set_hide_on_close();
		about_dlg->set_logo(Gdk::Texture::create_from_resource("/net/dnatechnologies/square_convertor/square_convertor.png"));
		about_dlg->set_program_name("Square Convertor");
		about_dlg->set_version("1.0.0");
		about_dlg->set_copyright("Alwin Leerling");
		about_dlg->set_comments("Converts chess squares from algabraic notation into mailbox id and back");
		about_dlg->set_license("LGPL");
		about_dlg->set_website("http://www.gtkmm.org");
		about_dlg->set_website_label("gtkmm website");
		about_dlg->set_authors({ "Alwin Leerling" });
		about_dlg->set_modal( true );
	}

	about_dlg->set_visible( true );
	about_dlg->present();
}
