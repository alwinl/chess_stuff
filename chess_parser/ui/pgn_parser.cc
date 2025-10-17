/*
 * pgn_parser.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include "pgn_parser_engine.h"

class PGNParser : public Gtk::ApplicationWindow
{
public:
	PGNParser();

private:
	Glib::RefPtr<Gtk::TextBuffer> text_buffer;
	std::unique_ptr<PGNParserEngine> engine;

    void init_aboutdialog();
    void on_action_open();
    bool on_action_quit();
};

PGNParser::PGNParser()
{
    engine = std::make_unique<PGNParserEngine>();

    set_title( "Portable Game Notation parser");

    add_action( "open", [this](){ on_action_open(); } );
	init_aboutdialog( );

	auto menu_button = Gtk::make_managed<Gtk::MenuButton>();
	menu_button->set_icon_name("open-menu-symbolic");

    auto menu_model = Gio::Menu::create();
    menu_model->append( "Open", "win.open");
    menu_model->append( "About", "win.about");

	menu_button->set_menu_model(menu_model);

	auto header_bar = Gtk::make_managed<Gtk::HeaderBar>();
	header_bar->pack_start(*menu_button);

	set_titlebar( *header_bar );

	auto child = Gtk::make_managed<Gtk::TextView>();
    child->set_size_request( 640, 480 );
    text_buffer = child->get_buffer();
	set_child( *child );

    signal_close_request().connect( [this](){ return on_action_quit(); }, true );
};

void PGNParser::on_action_open()
{
    auto dlg = Gtk::FileDialog::create();

    dlg->set_title( "Open Chess File" );
    dlg->set_modal( true );

    auto filter_list = Gio::ListStore<Gtk::FileFilter>::create();
    auto pgn_filter = Gtk::FileFilter::create();

    pgn_filter->set_name("PGN Files");
    pgn_filter->add_pattern("*.pgn");
    pgn_filter->add_pattern("*.PGN");

    filter_list->append( pgn_filter );
    dlg->set_filters( filter_list );

    auto open_fun = [this,dlg]( std::shared_ptr<Gio::AsyncResult>& async_result )
    {
        auto file  = dlg->open_finish( async_result );

        if( !engine->open_file( file->get_path() ) ) {
            Gtk::AlertDialog::create( "Error opening chess file." )->show(*this);
            return;
        }

        std::string content;

		engine->visit_tag_pairs( [&content](const std::string& tag, const std::string& value)
			{ content += "Tag: " + tag + ", value: " + value + "\n"; }
		);

		engine->visit_movetext( [&content](const std::string& white_move, const std::string& black_move)
			{ content += "White: " + white_move + ", Black: " + black_move + "\n"; }
		);

        text_buffer->set_text( content );
    };

    dlg->open( *this, open_fun );
}

bool PGNParser::on_action_quit()
{
    auto dlg = Gtk::AlertDialog::create("Do you really want to quit?" );

    dlg->set_buttons( {"Yes", "No"} );
    dlg->set_modal(true);

    auto choice_fun = [this,dlg]( std::shared_ptr<Gio::AsyncResult>& async_result )
    {
        if( dlg->choose_finish( async_result ) == 0 )
            set_visible( false );
    };

    dlg->choose( *this, choice_fun );

    return true;
}

void PGNParser::init_aboutdialog()
{
	auto about_dialog = Gtk::make_managed<Gtk::AboutDialog>();

    about_dialog->set_transient_for( *this );
    about_dialog->set_program_name("Chess Parser");
    // about_dialog->set_logo( Gdk::Texture::create_from_resource("/net/dnatechnologies/chessparser/application.png") ) ;
    about_dialog->set_version( "1.0.0" );
    about_dialog->set_comments("Parse PNG files.");
    about_dialog->set_copyright( "Copyright © 2021 Alwin Leerling" );
    about_dialog->set_website( "http://github" );
	about_dialog->set_license("LGPL");
	about_dialog->set_website_label("gtkmm website");
	about_dialog->set_authors({ "Alwin Leerling" });
	about_dialog->set_hide_on_close();

	add_action( "about", [about_dialog](){ about_dialog->set_visible(); } );
}

int main(int argc, char** argv)
{
	Glib::set_init_to_users_preferred_locale(false);

	auto app = Gtk::Application::create();

	return app->make_window_and_run<PGNParser>(argc, argv);
}
