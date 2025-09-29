/*
 * ChessParser Copyright 2022 Alwin Leerling dna.leerling@gmail.com
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

#include <string>

#include "pgn_parser_engine.h"

#include <gtkmm.h>

class PGNParser : public Gtk::Application
{
public:
	PGNParser();

private:
	void on_activate() override;

	void on_action_open();
	bool on_action_quit();
	void on_action_about();

	Glib::RefPtr<Gtk::TextBuffer> text_buffer;
	std::unique_ptr<PGNParserEngine> engine;
	std::unique_ptr<Gtk::AboutDialog> about_dlg;
};

int main( int argc, char* argv[])
{
	Glib::set_init_to_users_preferred_locale(false);

    PGNParser().run( argc, argv );
}

PGNParser::PGNParser() : Gtk::Application( "net.dnatechnologies.chessparser" )
{
	Glib::set_application_name("chessparser");

    engine = std::make_unique<PGNParserEngine>();
}

void PGNParser::on_activate()
{
    add_action( "open", sigc::mem_fun( *this, &PGNParser::on_action_open ) );
	add_action( "about", sigc::mem_fun( *this, &PGNParser::on_action_about ) );

	auto window = Gtk::make_managed<Gtk::ApplicationWindow>();
	window->set_title( "Portable Game Notation parser" );

	auto menu_button = Gtk::make_managed<Gtk::MenuButton>();
	menu_button->set_icon_name("open-menu-symbolic");

    auto menu_model = Gio::Menu::create();
    menu_model->append( "Open", "app.open");
    menu_model->append( "About", "app.about");

	menu_button->set_menu_model(menu_model);

	auto header_bar = Gtk::make_managed<Gtk::HeaderBar>();
	header_bar->pack_start(*menu_button);

	window->set_titlebar( *header_bar );

	auto child = Gtk::make_managed<Gtk::TextView>();
    child->set_size_request( 640, 480 );
    text_buffer = child->get_buffer();
	window->set_child( *child );

    window->signal_close_request().connect( sigc::mem_fun( *this, &PGNParser::on_action_quit ), true );

	add_window( *window );
    window->show();
}

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
            Gtk::AlertDialog::create( "Error opening chess file." )->show(*get_active_window());
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

    dlg->open( *get_active_window(), open_fun );
}

bool PGNParser::on_action_quit()
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

void PGNParser::on_action_about()
{
	if( about_dlg == nullptr ) {

		about_dlg = std::make_unique<Gtk::AboutDialog>();

		about_dlg->set_transient_for(*get_active_window());
		about_dlg->set_hide_on_close();
		about_dlg->set_logo(Gdk::Texture::create_from_resource("/net/dnatechnologies/chessparser/application.png"));
		about_dlg->set_program_name("Chess Parser");
		about_dlg->set_version("1.0.0");
		about_dlg->set_copyright("Alwin Leerling");
		about_dlg->set_comments("Parse PNG files.");
		about_dlg->set_license("LGPL");
		about_dlg->set_website("http://www.gtkmm.org");
		about_dlg->set_website_label("gtkmm website");
		about_dlg->set_authors({ "Alwin Leerling" });
		about_dlg->set_modal( true );
	}

	about_dlg->set_visible( true );
	about_dlg->present();
}
