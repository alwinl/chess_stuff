new_chess.cc
============

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

chess_window.h
==============
#pragma once

#include <gtkmm.h>

#include "board_view.h"
#include "dialogs.h"

class ChessEngine
{
public:
    bool load_game( std::string filepath ) { return true; }
    bool save_game( std::string filepath ) { return filepath != ""; }
};

class ChessWindow : public Gtk::ApplicationWindow
{
public:
	ChessWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);

private:
    Gtk::PopoverMenuBar * mnuGame;
    Gtk::PopoverMenuBar * mnuArrange;
    Gtk::PopoverMenuBar * mnuStop;
    BoardView * board_view;
    Gtk::Label * status_bar;

    std::unique_ptr<ChessEngine> engine;

    void init_board_data();
    void init_file_dialog( );
    void init_action_newgame( chess_gui::DialogNewGame* dlg );
    void init_action_piecevalues( chess_gui::DialogPieceValues* dlg );
    void init_action_colours( chess_gui::DialogColours* dlg );
    void init_action_level( chess_gui::DialogLevel* dlg );
    void init_action_simple();
    void init_mouse_events();

    void on_action_play();
    void on_action_hint();
    void on_action_undo();
    void on_action_redo();
    void on_action_arrange();
    void on_action_reverse();
    void on_action_showbestline();
    void on_action_demomode();

    void on_drag_start( int n_press, double mouse_x, double mouse_y );
    void on_drag_motion( double mouse_x, double mouse_y );
    void on_drag_done( int n_press, double mouse_x, double mouse_y );
};

chess_window.cc
===============
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

    // board_view->std_info =
    // {
    //     std::pair<std::string,std::string>{"Turn", "White" },
    //     std::pair<std::string,std::string>{"White", "e2-e4" },
    //     std::pair<std::string,std::string>{"Black", "e7-e5" },
    //     std::pair<std::string,std::string>{"Time", "now" },
    //     std::pair<std::string,std::string>{"Level", "Easy" },
    //     std::pair<std::string,std::string>{"Value", "0" },
    //     std::pair<std::string,std::string>{"Nodes", "1000000" },
    //     std::pair<std::string,std::string>{"N/Sec", "1000" },
    //     std::pair<std::string,std::string>{"Depth", "3" },
    //     std::pair<std::string,std::string>{"Bestline", "unknown" }
    // };
    // board_view->floating_pos = {10.0, 10.0};
    // board_view->floating_piece = 'q';
    // board_view->highlight_square = 5;
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
}

void ChessWindow::on_action_play() {}
void ChessWindow::on_action_hint() { board_view->set_highlight( 4 ); }
void ChessWindow::on_action_undo() {}
void ChessWindow::on_action_redo() {}
void ChessWindow::on_action_arrange() {}
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

board_view.h
============
#pragma once

#include <gtkmm.h>

#include <array>

class BoardView : public Gtk::DrawingArea
{
public:
    BoardView(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);

    void toggle_bestline() { show_bestline = !show_bestline; queue_draw(); }
    void toggle_reversed() { reversed = ! reversed; queue_draw(); }
    void set_highlight( uint16_t square ) { highlight_square = square; queue_draw(); }
    void set_editing( bool on ) { is_editing = on; queue_draw(); }
    void set_piece_code_at( uint16_t square, char code ) { piece_positions[square] = code; queue_draw(); }
    void set_floating( std::pair<float,float> pos, char code ) { floating_pos = pos; floating_piece = code; queue_draw(); };
    void move_floating_by( std::pair<float,float> delta ) { if( floating_piece != ' ') { floating_pos.first += delta.first; floating_pos.second += delta.second; queue_draw(); } }
    void set_colours( std::array<std::string,4> new_colours ) { 
        background_colour = Gdk::RGBA(new_colours[0]);
        foreground_colour = Gdk::RGBA(new_colours[1]);
        white_colour = Gdk::RGBA(new_colours[2]);
        black_colour = Gdk::RGBA(new_colours[3]);
        queue_draw();
    }

private:
	Gdk::Rectangle board_outline;
	Gdk::Rectangle info_outline;
	Gdk::Rectangle edit_outline;

	Cairo::RefPtr<Cairo::ImageSurface> assets_image;

    std::array<std::pair<std::string,std::string>,10> std_info;
    Gdk::RGBA background_colour;
	Gdk::RGBA foreground_colour;
	Gdk::RGBA white_colour;
	Gdk::RGBA black_colour;
    std::array<char, 64> piece_positions;
    std::pair<float,float> floating_pos;
    char floating_piece = ' ';
    uint16_t highlight_square = (uint16_t)-1;
    bool is_editing = false;
    bool show_bestline = true;
    bool reversed = false;

    void calculate_outlines( int width, int height );

    void draw( const Cairo::RefPtr<Cairo::Context>& cr, int width, int height );

    void paint_background( const Cairo::RefPtr<Cairo::Context>& cr ) const;
    void paint_board( const Cairo::RefPtr<Cairo::Context>& cr ) const;
    void paint_pieces( const Cairo::RefPtr<Cairo::Context> &cr ) const;
    void paint_editwindow( const Cairo::RefPtr<Cairo::Context> &cr ) const;
    void paint_infowindow( const Cairo::RefPtr<Cairo::Context> &cr ) const;
    void paint_floating( const Cairo::RefPtr<Cairo::Context> &cr ) const;
    void paint_highlight( const Cairo::RefPtr<Cairo::Context> &cr ) const;
};

board_view.cc
=============
#include "board_view.h"

constexpr int SQUARE_SIZE = 36;
constexpr int INFO_WIDTH = 240;
constexpr int MAX_HEIGHT = 1024;
constexpr int REVERSE_RANK_MASK = 0b00111000;



BoardView::BoardView(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder) : Gtk::DrawingArea( cobject )
{
	assets_image = Cairo::ImageSurface::create( Cairo::Surface::Format::ARGB32, 6*SQUARE_SIZE, 2*SQUARE_SIZE );
	Cairo::RefPtr<Cairo::Context> context = Cairo::Context::create( assets_image );
	Glib::RefPtr<Gdk::Pixbuf> pieces_pixbuf = Gdk::Pixbuf::create_from_resource("/net/dnatechnologies/chess/pieces.png");

	context->rectangle( 0, 0, 6 * SQUARE_SIZE, 2 * SQUARE_SIZE );
	Gdk::Cairo::set_source_pixbuf( context, pieces_pixbuf, 0.0, 0.0 );
	context->fill();

	set_draw_func( [this]( const Cairo::RefPtr<Cairo::Context> &cr, int width, int height ){ draw( cr, width, height); } );
}

void BoardView::draw( const Cairo::RefPtr<Cairo::Context> &cr, int width, int height )
{
	calculate_outlines( width, height );

	paint_background( cr );
	paint_board( cr );
	paint_pieces( cr );

    is_editing ? paint_editwindow( cr ) : paint_infowindow( cr );

	paint_floating( cr );
	paint_highlight( cr );
}

void BoardView::calculate_outlines( int width, int height )
{
	/* the left side of the board is midway of our allocated width minus half the board */
	/* the top side of the board is midway of the height minus half the board height */
	board_outline = Gdk::Rectangle(
            (width - INFO_WIDTH)/2 - 4 * SQUARE_SIZE,
            height/2 - 4 * SQUARE_SIZE,
            8 * SQUARE_SIZE + 2,
            8 * SQUARE_SIZE + 2
    );

	/* the info widow is placed on the right side of the window */
	info_outline = Gdk::Rectangle(
			width - INFO_WIDTH,
			0,
			INFO_WIDTH,
			height
	);

	/* the edit window is halfway of the info width minus the width of half the pieces bitmap */
	edit_outline = Gdk::Rectangle(
			width - INFO_WIDTH/2 - 3 * SQUARE_SIZE,
			height/2 - 1 * SQUARE_SIZE,
			6 * SQUARE_SIZE + 2,
			2 * SQUARE_SIZE + 2
	);
}

void BoardView::paint_background( const Cairo::RefPtr<Cairo::Context>& cr ) const
{
	cr->set_source_rgb( background_colour.get_red(), background_colour.get_green(), background_colour.get_blue() );
 	cr->paint();
}

void BoardView::paint_board( const Cairo::RefPtr<Cairo::Context> &cr ) const
{
	cr->rectangle( board_outline.get_x(), board_outline.get_y(), board_outline.get_width(), board_outline.get_height() );

	cr->set_source_rgb( black_colour.get_red(), black_colour.get_green(), black_colour.get_blue() );
	cr->fill_preserve();

	cr->set_source_rgb( 0, 0, 0);
	cr->stroke();

	/* Paint each alternate square in the white colour */
	cr->set_source_rgb( white_colour.get_red(), white_colour.get_green(), white_colour.get_blue() );

	for( int cell=0; cell<64; cell++ ) {
		if( !( (cell + (cell/8)) % 2 ) ) {
			cr->rectangle( board_outline.get_x() + (cell%8) * SQUARE_SIZE + 1, board_outline.get_y() + (cell/8) * SQUARE_SIZE + 1, SQUARE_SIZE, SQUARE_SIZE );
			cr->fill();
		}
	}
}

void BoardView::paint_pieces( const Cairo::RefPtr<Cairo::Context> &cr ) const
{
	static std::string piece_chars = "KQRBNPkqrbnp";

	for( int square = 0; square < 64; ++square ) {

		char piece_code = piece_positions[square];

		if( piece_code == ' ' )
			continue;

		unsigned int index = piece_chars.find( piece_code );

		if( reversed )
			square ^= REVERSE_RANK_MASK;

		double dest_x   =      (square % 8)  * SQUARE_SIZE + board_outline.get_x();
		double dest_y   = (7 - (square / 8)) * SQUARE_SIZE + board_outline.get_y();
		
		double source_x = dest_x - (index % 6) * SQUARE_SIZE;
		double source_y = dest_y - (index / 6) * SQUARE_SIZE;

		cr->set_source( assets_image, source_x, source_y );
		cr->rectangle( dest_x, dest_y, SQUARE_SIZE, SQUARE_SIZE );
		cr->fill();
	}
}

void BoardView::paint_editwindow( const Cairo::RefPtr<Cairo::Context> &cr ) const
{
	cr->rectangle( edit_outline.get_x(), edit_outline.get_y(), 6 * SQUARE_SIZE + 2, 2 * SQUARE_SIZE + 2 );
	cr->set_source_rgb( 0, 0, 0);
	cr->stroke();

	cr->rectangle( edit_outline.get_x() + 1, edit_outline.get_y() + 1, 6 * SQUARE_SIZE, 2 * SQUARE_SIZE );
	cr->set_source( assets_image, edit_outline.get_x(), edit_outline.get_y());
	cr->fill();
}

void BoardView::paint_infowindow( const Cairo::RefPtr<Cairo::Context> &cr ) const
{
    Cairo::TextExtents extents;

    cr->set_font_size( 12 );
    cr->get_text_extents( "Bestline", extents ); // Need it for the height of the frame

 	// Paint a frame
    int height = (show_bestline ? 12 : 11 ) * (extents.height + 5);

    cr->set_source_rgb(  foreground_colour.get_red(), foreground_colour.get_green(), foreground_colour.get_blue() );
	cr->rectangle( info_outline.get_x() + 5, info_outline.get_y() + 10, INFO_WIDTH - 10, height );
	cr->stroke();

	// Blank the frame portion behind the word "Information"
    cr->get_text_extents( "Information", extents );

	cr->rectangle( info_outline.get_x() + 10, info_outline.get_y() + 5, extents.width + 10, extents.height );
    cr->set_source_rgb(  background_colour.get_red(), background_colour.get_green(), background_colour.get_blue() );
	cr->fill();

	// Paint the word "Information"
    cr->set_source_rgb(  foreground_colour.get_red(), foreground_colour.get_green(), foreground_colour.get_blue() );
    cr->move_to( info_outline.get_x() + 15, info_outline.get_y() + extents.height + 5 );
    cr->show_text( "Information" );

	// Paint the actual information
    double offset_y = info_outline.get_y() + extents.height + 25;
    double label_offset_x = info_outline.get_x() + 10;
    double info_offset_x = info_outline.get_x() + 10 + extents.width + 10;

	auto end_it = std_info.end();
	if( ! show_bestline )
		--end_it;

	for( auto it = std_info.begin(); it != end_it; ++it ) {

		cr->move_to( label_offset_x, offset_y );
		cr->show_text( std::get<0>(*it) );
		cr->move_to( info_offset_x, offset_y );
		cr->show_text( std::get<1>(*it) );

		offset_y += extents.height + 5;
	}
}

void BoardView::paint_floating( const Cairo::RefPtr<Cairo::Context> &cr ) const
{
	static std::string piece_chars = "KQRBNPkqrbnp";

	if( floating_piece == ' ' )
		return;

	auto [dest_x, dest_y] = floating_pos;
	
	unsigned int index = piece_chars.find( floating_piece );

	double source_x = dest_x - (index % 6) * SQUARE_SIZE;
	double source_y = dest_y - (index / 6) * SQUARE_SIZE;

	cr->set_source( assets_image, source_x, source_y );
	cr->rectangle( dest_x, dest_y, SQUARE_SIZE, SQUARE_SIZE );
	cr->fill();
}

void BoardView::paint_highlight( const Cairo::RefPtr<Cairo::Context> &cr ) const
{
	uint16_t square = highlight_square;

	if( square == (uint16_t)-1 )
		return;

	if( reversed )
		square ^= REVERSE_RANK_MASK;

	double pos_x = 	(square % 8) * SQUARE_SIZE + board_outline.get_x();
	double pos_y = 	(7 - (square / 8)) * SQUARE_SIZE + board_outline.get_y();

	cr->rectangle( pos_x, pos_y, SQUARE_SIZE, SQUARE_SIZE );
	cr->set_source_rgb(  foreground_colour.get_red(), foreground_colour.get_green(), foreground_colour.get_blue() );
	cr->set_line_width( 4.0 );
	cr->stroke();
}
