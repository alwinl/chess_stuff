/*
 * chess_window.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include "chess_window.h"
#include "dialogs.h"

ChessWindow::ChessWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder) : Gtk::ApplicationWindow(cobject)
{
    set_title( "GTKmm Chess");

    engine = std::make_unique<ChessEngine>();

    mnuGame    = builder->get_widget<Gtk::PopoverMenuBar>( "mnuGame" );
    mnuArrange = builder->get_widget<Gtk::PopoverMenuBar>( "mnuArrange" );
    mnuStop    = builder->get_widget<Gtk::PopoverMenuBar>( "mnuStop" );
    board_view = Gtk::Builder::get_widget_derived<BoardView>( builder, "canvas" );
    status_bar = builder->get_widget<Gtk::Label>( "widStatusBar" );

	auto dlgNewGame     = Gtk::Builder::get_widget_derived<chess_gui::DialogNewGame>( builder, "dlgNewGame" );
    auto dlgPieceValues = Gtk::Builder::get_widget_derived<chess_gui::DialogPieceValues>( builder, "dlgPieceValues" );
    auto dlgColours     = Gtk::Builder::get_widget_derived<chess_gui::DialogColours>( builder, "dlgColours" );
    auto dlgLevel       = Gtk::Builder::get_widget_derived<chess_gui::DialogLevel>( builder, "dlgLevel" );

    signal_realize().connect( [this]() {init_board_data(); } );
    signal_realize().connect( [this]() {init_file_dialog(); } );
    signal_realize().connect( [this, dlgNewGame]() {init_action_newgame( dlgNewGame ); } );
    signal_realize().connect( [this, dlgPieceValues]() {init_action_piecevalues( dlgPieceValues ); } );
    signal_realize().connect( [this, dlgColours]() {init_action_colours( dlgColours ); } );
    signal_realize().connect( [this, dlgLevel]() {init_action_level( dlgLevel ); } );
    signal_realize().connect( [this]() { init_action_simple(); } );
    signal_realize().connect( [this]() { init_action_arrange_turn(); } );
    signal_realize().connect( [this]() { init_mouse_events(); } );
};

void ChessWindow::init_board_data()
{
    // fill_positions
    std::string positions = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";

    unsigned int rank = 7;
    unsigned int file = 0;

    for( uint16_t square = 0; square < 64; ++square )
        board_view->set_piece_code_at( square, ' ' );

    for( char& code : positions ) {

        if( (code > '0') && (code < '9') ) {       // Character can either be a number (squares to skip)
        file += (code - '0' );
        } else if( code == '/' ) {                      // ... or an end of rank indicator
            --rank;
            file = 0;
        } else {								  // or an piece designator
            board_view->set_piece_code_at( rank * 8 + file, code );
            ++file;
        }
    }

    board_view->set_colours( {
        "rgb(78,154,6)",
        "rgb(0,0,0)",
        "rgb(238,238,236)",
        "rgb(85,87,83)",
    } );

    board_view->set_turn( true );
    board_view->set_white_move( "e2-e4" );
    board_view->set_black_move( "e7-e5" );
    board_view->set_time( "now" );
    board_view->set_level( "Easy" );
    board_view->set_value( 0 );
    board_view->set_nodes( 1000000 );
    board_view->set_node_speed( 1000 );
    board_view->set_depth( 3 );
    board_view->set_bestline( "unknown" );

    // board_view->floating_pos = {10.0, 10.0};
    // board_view->floating_piece = 'q';
    // board_view->hightlist_square = 5;
    // board_view->is_editing = false;
    // board_view->show_bestline = true;
    // board_view->reversed = false;
}

void ChessWindow::init_file_dialog( )
{
    auto dlg = Gtk::FileDialog::create();

    auto filter_list = Gio::ListStore<Gtk::FileFilter>::create();
    auto pgn_filter = Gtk::FileFilter::create();

    pgn_filter->set_name("PGN Files");
    pgn_filter->add_pattern("*.pgn");
    pgn_filter->add_pattern("*.PGN");

    filter_list->append( pgn_filter );
    dlg->set_filters( filter_list );
    dlg->set_modal( true );

    auto open_fun = [this,dlg]( const Glib::RefPtr<Gio::AsyncResult>& async_result )
    {
        try {
            auto file  = dlg->open_finish( async_result );

            if( !engine->load_game( file->get_path() ) ) {
                Gtk::AlertDialog::create( "Error opening chess file." )->show(*this);
                return;
            }

            board_view->queue_draw();

            status_bar->set_text( "Opened " + file->get_path() );
        }
        catch(...) {
            status_bar->set_text( "No file opened" );
        }
    };

    auto save_fun = [this,dlg]( const Glib::RefPtr<Gio::AsyncResult>& async_result )
    {
        try {
            auto file  = dlg->save_finish( async_result );

            if( !engine->save_game( file->get_path() ) ) {
                Gtk::AlertDialog::create( "Error saving chess file." )->show(*this);
                return;
            }

            status_bar->set_text( "Saved " + file->get_path() );
        }
        catch (...) {
            status_bar->set_text( "File not saved" );
        }
    };

    get_application()->add_action("open", [this, dlg, open_fun](){ dlg->set_title( "Open Chess File" ); dlg->open( *this, open_fun ); } );    
    get_application()->add_action("saveas", [this, dlg, save_fun](){ dlg->set_title( "Save Chess File" ); dlg->save( *this, save_fun ); } );    
    get_application()->add_action("save", [this, dlg, save_fun]()
    { 
        if( engine->save_game( "" ) )
            status_bar->set_text( "Saved game" );
        else {
            dlg->set_title( "Save Chess File" );
            dlg->save( *this, save_fun );
        }
    } );    
}

void ChessWindow::init_action_newgame( chess_gui::DialogNewGame* dlg )
{
    auto new_func = [this, dlg]()
    {
		// engine->new_game( dlg->get_choice() );

		board_view->queue_draw();

		status_bar->set_text( std::string("New game") + std::to_string(dlg->get_choice()) );
    };

    dlg->signal_hide().connect( new_func );

    get_application()->add_action( "new", [this, dlg]() { dlg->set_visible( true ); } );
}

void ChessWindow::init_action_piecevalues( chess_gui::DialogPieceValues* dlg )
{
    auto func = [this, dlg]()
    {
		// if( ! engine->set_piece_values( dlg->get_values() ) ) {
		// 	Gtk::AlertDialog::create( "Error setting piece values." )->show( *this );
		// 	return;
		// }

		status_bar->set_text( std::string("Piece values changed") );
    };

    dlg->signal_hide().connect( func );

    get_application()->add_action( "piecevalues", [this, dlg]() { dlg->set_visible( true ); } );
}

void ChessWindow::init_action_colours( chess_gui::DialogColours *dlg )
{
    dlg->set_colours(        
    {
        "rgb(78,154,6)",
        "rgb(0,0,0)",
        "rgb(238,238,236)",
        "rgb(85,87,83)",
    } );

    std::array<std::string,4> temp;

    auto func = [this, dlg]()
    {
        board_view->set_colours( dlg->get_colours() );

		status_bar->set_text( std::string("Changed colours") );
    };

    dlg->signal_hide().connect( func );

    get_application()->add_action( "colours", [this, dlg]() { dlg->set_visible( true ); } );
}

void ChessWindow::init_action_level( chess_gui::DialogLevel *dlg )
{
    auto func = [this, dlg]()
    {
		// if( ! engine->set_level( dlgLevel->get_level() ) ) {
		// 	Gtk::AlertDialog::create( "Error setting level." )->show( *this );
		// 	return;
		// }

		// status_bar->set_text( std::string("Level: ") + engine->get_level_string() );
        status_bar->set_text( std::string("Set level") );
    };

    dlg->signal_hide().connect( func );

    get_application()->add_action( "level", [this, dlg]() { dlg->set_visible( true ); } );
}

void ChessWindow::init_action_simple()
{
    get_application()->add_action("play",         [this](){ on_action_play();} );
    get_application()->add_action("hint",         [this](){ on_action_hint();} );
    get_application()->add_action("undo",         [this](){ on_action_undo();} );
    get_application()->add_action("redo",         [this](){ on_action_redo();} );
    get_application()->add_action("arrange",      [this](){ on_action_arrange();} );
    get_application()->add_action("reverse",      [this](){ on_action_reverse();} );
    get_application()->add_action("showbestline", [this](){ on_action_showbestline();} );
    get_application()->add_action("demomode",     [this](){ on_action_demomode();} );


    get_application()->add_action("arrange_clear",   [this](){ on_action_arrange_clear(); } );
    get_application()->add_action("arrange_done",    [this](){ on_action_arrange_done(); } );
    get_application()->add_action("arrange_cancel",  [this](){ on_action_arrange_cancel(); } );
    get_application()->add_action("arrange_makeFEN", [this](){ on_action_arrange_make_fen(); } );
	get_application()->add_action("think_stop",      [this](){ on_action_thinking_stop(); } );

}

void ChessWindow::init_action_arrange_turn()
{
    auto action = Gio::SimpleAction::create( "arrange_turn", Glib::VariantType("s"), Glib::Variant<Glib::ustring>::create("white") );

    action->signal_activate().connect(
        [this, action](const Glib::VariantBase& value)
        {
            auto color = Glib::VariantBase::cast_dynamic<Glib::Variant<Glib::ustring>>(value).get();
            action->change_state(Glib::Variant<Glib::ustring>::create(color));

            // if (color == "white")
            //     engine->arrange_set_turn(eColor::white);
            // else
            //     engine->arrange_set_turn(eColor::black);
        });

    get_application()->add_action(action);
}


void ChessWindow::on_action_play() {}
void ChessWindow::on_action_hint() { board_view->set_highlight( 4 ); }
void ChessWindow::on_action_undo() {}
void ChessWindow::on_action_redo() {}
void ChessWindow::on_action_arrange()
{
	mnuGame->hide();
	mnuArrange->show();

	board_view->set_editing( true );
}

void ChessWindow::on_action_reverse() { board_view->toggle_reversed( ); }
void ChessWindow::on_action_showbestline() { board_view->toggle_bestline(); }
void ChessWindow::on_action_demomode() {}

void ChessWindow::init_mouse_events()
{
	auto mouse_event = Gtk::GestureClick::create();
	auto motion_controller = Gtk::EventControllerMotion::create();

	board_view->add_controller( mouse_event );
	board_view->add_controller( motion_controller );

	mouse_event->signal_pressed().connect( [this]( int n_press, double mouse_x, double mouse_y ){ on_drag_start( n_press, mouse_x, mouse_y ); });
	mouse_event->signal_released().connect( [this]( int n_press, double mouse_x, double mouse_y ){ on_drag_done( n_press, mouse_x, mouse_y ); });

	motion_controller->signal_motion().connect( [this]( double mouse_x, double mouse_y ){ on_drag_motion( mouse_x, mouse_y ); });
}

void ChessWindow::on_drag_start( int n_press, double mouse_x, double mouse_y ) {}
void ChessWindow::on_drag_motion( double mouse_x, double mouse_y ) {}
void ChessWindow::on_drag_done( int n_press, double mouse_x, double mouse_y ) {}

void ChessWindow::on_action_arrange_clear()
{
	// engine->arranging_start();

	// board->set_piece_positions( engine->get_piece_positions() );
}

void ChessWindow::on_action_arrange_done()
{
	// if( ! engine->arranging_end( false ) ) {
	// 	Gtk::AlertDialog::create( "This is not a valid board. Please fix." )->show( *view );
	// 	return;
	// }

	// board->set_piece_positions( engine->get_piece_positions() );

	mnuArrange->hide();
	mnuGame->show();

	board_view->set_editing( false );
}

void ChessWindow::on_action_arrange_cancel()
{
	// engine->arranging_end( true );

	// board->set_piece_positions( engine->get_piece_positions() );

    mnuArrange->hide();
	mnuGame->show();

	board_view->set_editing( false );
}

void ChessWindow::on_action_arrange_make_fen() {}

void ChessWindow::on_action_thinking_stop() {}
