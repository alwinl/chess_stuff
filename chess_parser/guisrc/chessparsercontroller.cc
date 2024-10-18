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

#include "chessparserengine.h"

#include <string>

#include "chessparsercontroller.h"

using namespace std;

int main( int argc, char* argv[])
{
    ChessParserController().run( argc, argv );
}

ChessParserController::ChessParserController() : Gtk::Application( "net.dnatechnologies.chessparser" )
{
	Glib::set_application_name("chessparser");

    engine = std::make_unique<ChessParserEngine>();
}

void ChessParserController::on_startup()
{
	Gtk::Application::on_startup();

    add_action("open",                sigc::mem_fun( *this, &ChessParserController::on_action_open         ) );
    add_action("quit",                sigc::mem_fun( *this, &ChessParserController::on_action_quit         ) );
	add_action("about",               sigc::mem_fun( *this, &ChessParserController::on_action_about        ) );

    Glib::RefPtr<Gtk::Builder> ui_model = Gtk::Builder::create_from_resource( "/net/dnatechnologies/chessparser/chessparserui.glade" );

	ui_model->get_widget_derived("appView", view );
	ui_model->get_widget("appStatusbar", status_bar );

	text_buffer = Glib::RefPtr<Gtk::TextBuffer>::cast_dynamic( ui_model->get_object( "content" ) );

	Glib::signal_idle().connect( sigc::mem_fun(*this, &ChessParserController::on_idle) );
}

/*-----------------------------------------------------------------------------
 * The application has been started. Time to create and show a window
 * NB we could build multiple views here and activate (show) views
 * as needed
 */
void ChessParserController::on_activate()
{
	view->set_default_size(640,480);
	view->show_all_children();

	add_window( *view );
	view->show();

	view->set_icon( Gdk::Pixbuf::create_from_resource("/net/dnatechnologies/chessparser/chessparser.png") );
}

class ParserVisitor : public ParserVisitorBase
{
public:
	ParserVisitor( string& result ) : content(result) {}

	virtual void process_tag_pair( std::string tag, std::string value ) {
		content += "Tag: " + tag + ", value: " + value + "\n";
	}
	virtual void process_movetext( std::string white_move, std::string black_move ) {
		content += "White: " + white_move + ", Black: " + black_move + "\n";
	}

private:
	string& content;
};

//-----------------------------------------------------------------------------
// Action implementation
//-----------------------------------------------------------------------------
void ChessParserController::on_action_open()
{
    Gtk::FileChooserDialog dlg( *view, "Open Chess File", Gtk::FILE_CHOOSER_ACTION_OPEN );

    dlg.add_button( "Cancel", Gtk::RESPONSE_CANCEL );
    dlg.add_button( "Open", Gtk::RESPONSE_ACCEPT );

    Glib::RefPtr<Gtk::FileFilter> filter_pgn = Gtk::FileFilter::create();

    filter_pgn->set_name("PGN Files");
    filter_pgn->add_pattern("*.pgn");
    filter_pgn->add_pattern("*.PGN");

    dlg.add_filter( filter_pgn );
    dlg.set_filter( filter_pgn );

    if( dlg.run() != Gtk::RESPONSE_ACCEPT ) {
		dlg.hide();
        return;
    }

    dlg.hide();

    if( ! engine->open_file( dlg.get_filename() ) ) {

        Gtk::MessageDialog( *view, "Error opening chess file.", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true).run();
        return;
    }

    string content;

    ParserVisitor visitor( content );
	engine->visit_tag_pairs( &visitor );
	engine->visit_movetext( &visitor );

    text_buffer->set_text( content );

    status_bar->push( std::string("Opened ") + dlg.get_filename() );
}

void ChessParserController::on_action_quit()
{
	Gtk::MessageDialog dlg( *view, "You really want to quit?", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK_CANCEL, true );
	if( dlg.run() == Gtk::RESPONSE_OK)
		quit();
}

bool ChessParserController::on_idle()
{
	return true;
}

/**-----------------------------------------------------------------------------
 * \brief Show some application information
 *
 * Show our Help About... box
 */
void ChessParserController::on_action_about()
{
    Gtk::AboutDialog dlg;

    dlg.set_transient_for( *view ) ;
    dlg.set_name("ChessParser") ;
    dlg.set_logo( Gdk::Pixbuf::create_from_resource("/net/dnatechnologies/chessparser/chessparser.png") ) ;
    dlg.set_version( "0.01" ) ;
    dlg.set_comments("A GtkMM application") ;
    dlg.set_copyright( "Copyright © 2022 Alwin Leerling" ) ;
    dlg.set_website( "http://github" ) ;

    dlg.run();
}
