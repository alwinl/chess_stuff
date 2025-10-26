/*
 * new_chess.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include <gtkmm.h>

#include "chess_window.h"

void init_aboutdialog( const Glib::RefPtr<Gtk::Application>& app, Gtk::ApplicationWindow * view )
{
	auto about_dialog = Gtk::make_managed<Gtk::AboutDialog>();

    about_dialog->set_transient_for( *view );
    about_dialog->set_program_name("Chess");
    about_dialog->set_logo( Gdk::Texture::create_from_resource("/net/dnatechnologies/chess/chess.png") ) ;
    about_dialog->set_version( "0.0.1" );
    about_dialog->set_comments("A GtkMM translation of Borland Chess");
    about_dialog->set_copyright( "Copyright © 2019 Alwin Leerling" );
    about_dialog->set_website( "http://github" );
	about_dialog->set_authors( { "Alwin Leerling"} );
	about_dialog->set_license_type( Gtk::License::GPL_3_0 );
	about_dialog->set_hide_on_close();

	app->add_action( "about", [about_dialog](){ about_dialog->set_visible(); } );
}

void init_action_quit( const Glib::RefPtr<Gtk::Application>& app, Gtk::ApplicationWindow * view )
{
	auto quit_dlg = Gtk::AlertDialog::create( "You really want to quit?" );

	quit_dlg->set_buttons( {"Ok", "Cancel"} );
	quit_dlg->set_modal();

	auto response_handler = [app, quit_dlg]( Glib::RefPtr<Gio::AsyncResult>& result )
	{
		try {
			if( quit_dlg->choose_finish( result) == 0 )
				app->quit();
		}
		catch( const Glib::Error& ex ) { /* user dismissed the dialog, do not shutdown */ }
	};

    app->add_action("quit", [quit_dlg, view, response_handler](){ quit_dlg->choose( *view, response_handler ); } );
}

int main(int argc, char** argv)
{
	Glib::set_init_to_users_preferred_locale(false);

	auto app = Gtk::Application::create("net.dnatechnologies.chess");

    app->signal_activate().connect( [app]()
    {
        auto builder = Gtk::Builder::create_from_resource( "/net/dnatechnologies/chess/chess.ui" );
        auto window = Gtk::Builder::get_widget_derived<ChessWindow>(builder, "main_view");

        init_aboutdialog( app, window );
        init_action_quit( app, window );

        app->add_window(*window);
        window->present();
    } );

	return app->run(argc, argv);
}

