/*
 * Copyright 2014 Alwin Leerling <alwin@jambo>
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
 *
 *
 */

#include "chessboard.h"
#include "chesscontroller.h"

using namespace std;

/**-----------------------------------------------------------------------------
 * \brief ChessBoard constructor
 *
 * \param cobject BaseObjectType*
 * \param ui_model const Glib::RefPtr<Gtk::Builder>&
 * \param model_init AppModel&
 *
 */
ChessBoard::ChessBoard( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model, ChessController& app )
                            : Gtk::DrawingArea(cobject), drag_code(' '), reversed(false), is_edit(false), controller(app)
{
	using namespace Cairo;

	// create our image objects
	background_image_ = ImageSurface::create( FORMAT_RGB24, 8*SQUARE_SIZE, 8*SQUARE_SIZE );
	pieces_surface_   = ImageSurface::create( FORMAT_ARGB32, 6*SQUARE_SIZE, 2*SQUARE_SIZE );

	// Create a context for the pieces bitmap so we can draw on it
	RefPtr<Context> context = Context::create( pieces_surface_ );

	// load and draw the image onto the surface
	Glib::RefPtr<Gdk::Pixbuf> pieces_pixbuf = Gdk::Pixbuf::create_from_resource("/net/dnatechnologies/chess/pieces.png");
	Gdk::Cairo::set_source_pixbuf( context, pieces_pixbuf, 0.0, 0.0 );
	context->paint();

	// The pieces bitmap contains king through to pawn, white pieces in top row and black pieces in bottom row
    source_offsets['K'] = Gdk::Point( 0 * SQUARE_SIZE, 0 * SQUARE_SIZE );
    source_offsets['Q'] = Gdk::Point( 1 * SQUARE_SIZE, 0 * SQUARE_SIZE );
    source_offsets['R'] = Gdk::Point( 2 * SQUARE_SIZE, 0 * SQUARE_SIZE );
    source_offsets['B'] = Gdk::Point( 3 * SQUARE_SIZE, 0 * SQUARE_SIZE );
    source_offsets['N'] = Gdk::Point( 4 * SQUARE_SIZE, 0 * SQUARE_SIZE );
	source_offsets['P'] = Gdk::Point( 5 * SQUARE_SIZE, 0 * SQUARE_SIZE );

    source_offsets['k'] = Gdk::Point( 0 * SQUARE_SIZE, 1 * SQUARE_SIZE );
    source_offsets['q'] = Gdk::Point( 1 * SQUARE_SIZE, 1 * SQUARE_SIZE );
    source_offsets['r'] = Gdk::Point( 2 * SQUARE_SIZE, 1 * SQUARE_SIZE );
    source_offsets['b'] = Gdk::Point( 3 * SQUARE_SIZE, 1 * SQUARE_SIZE );
    source_offsets['n'] = Gdk::Point( 4 * SQUARE_SIZE, 1 * SQUARE_SIZE );
    source_offsets['p'] = Gdk::Point( 5 * SQUARE_SIZE, 1 * SQUARE_SIZE );

    info_data.push_back( pair<string,string>("Turn", "empty") );
    info_data.push_back( pair<string,string>("White", "empty") );
    info_data.push_back( pair<string,string>("Black", "empty") );
    info_data.push_back( pair<string,string>("Time", "empty") );
    info_data.push_back( pair<string,string>("Level", "empty") );
    info_data.push_back( pair<string,string>("Value", "empty") );
    info_data.push_back( pair<string,string>("Nodes", "empty") );
    info_data.push_back( pair<string,string>("N/Sec", "empty") );
    info_data.push_back( pair<string,string>("Depth", "empty") );
    info_data.push_back( pair<string,string>("Bestline", "empty") );

    show_bestline_info = true;
    animate_code = ' ';

    controller.set_board( this );
}

/**-----------------------------------------------------------------------------
 * \brief Called when screen size changes
 *
 * Recalculate our screen real estate. Can be used as a trigger to
 * recalculate other drawing parameters.
 *
 * We have three distinct portions in the window
 * chess board on the left, info window/edit window on the right
 *
 * Note that only one of the edit and info windows are shown at the same time
 * We reserve the right most 226 pixels for the edit/info window and
 * allocate the rest to the board.
 *
 * \param GdkEventConfigure*event
 * \return bool
 *
 */
bool ChessBoard::on_configure_event( GdkEventConfigure*event )
{
	Gtk::Allocation allocation = get_allocation();

	board_outline = Gdk::Rectangle(
            (allocation.get_width() - 226)/2 - 4 * SQUARE_SIZE, /* the left side of the board is midway of our allocated width minus half the board */
            allocation.get_height()/2 - 4 * SQUARE_SIZE,		/* the top side of the board is midway of the height minus half the board height */
            8 * SQUARE_SIZE,
            8 * SQUARE_SIZE
    );

	info_outline = Gdk::Rectangle(
			allocation.get_width() - 226, /* the info widow is placed in the right most 226 pixels of our window */
			0,
			226,
			allocation.get_height()
	);

	edit_outline = Gdk::Rectangle(
			allocation.get_width() - 113 - 3 * SQUARE_SIZE, /* the edit window is halfway of the rightmost 226 minus the width of half the pieces bitmap */
			allocation.get_height()/2 - 1 * SQUARE_SIZE,
			6 * SQUARE_SIZE,
			2 * SQUARE_SIZE
	);

	return false;
}

/**-----------------------------------------------------------------------------
 * \brief
 */
bool ChessBoard::draw_board( const Cairo::RefPtr<Cairo::Context>& cr )
{
	// Draw border and board
	cr->rectangle( board_outline.get_x(), board_outline.get_y(), board_outline.get_width(), board_outline.get_height() );
	cr->set_source_rgb( 0, 0, 0);								// border
	cr->stroke_preserve();
	cr->set_source( background_image_, board_outline.get_x(), board_outline.get_y());	// board
	cr->fill();

    return false;
}

/**-----------------------------------------------------------------------------
 * \brief
 */
bool ChessBoard::draw_pieces( const Cairo::RefPtr<Cairo::Context>& cr )
{
	for( std::pair<const STSquare,char>& piece : pieces ) {

		// calculate the destination pixel position by taking the left top of the board and
		// adding (converted) rank and file positions. Note y axis is reversed.
		// If the board is reversed, the x-axis is reversed, not the y-axis
		Gdk::Rectangle dest = Gdk::Rectangle( board_outline.get_x(), board_outline.get_y(), SQUARE_SIZE, SQUARE_SIZE );

		if( reversed ) {
			dest.set_x( dest.get_x() + (7 - piece.first.file)  * SQUARE_SIZE );
			dest.set_y( dest.get_y() +      piece.first.rank   * SQUARE_SIZE );
		} else {
			dest.set_x( dest.get_x() +      piece.first.file   * SQUARE_SIZE );
			dest.set_y( dest.get_y() + (7 - piece.first.rank)  * SQUARE_SIZE );
		}

		// To paint a particular piece we have to offset our source bitmap so that the correct bitmap
		// overlays our square to be painted. The pieces are stored as two rows (a white and a black row)
		// in six columns, calculable by type.
		Gdk::Point source = Gdk::Point( dest.get_x() - source_offsets[piece.second].get_x(), dest.get_y() - source_offsets[piece.second].get_y() );

		cr->set_source( pieces_surface_, source.get_x(), source.get_y() );
		cr->rectangle( dest.get_x(), dest.get_y(), dest.get_width(), dest.get_height() );
		cr->fill();
	}

    return false;
}

/**-----------------------------------------------------------------------------
 * \brief
 */
bool ChessBoard::draw_info( const Cairo::RefPtr<Cairo::Context>& cr )
{
    Cairo::TextExtents extents;

    cr->set_font_size( 12 );
    cr->get_text_extents( info_data[0].first, extents ); // Need it for the height of the frame

    int height = (show_bestline_info ? 12 : 11 ) * (extents.height + 5);

 	// Draw a frame
    cr->set_source_rgb(  foreground_colour.get_red(), foreground_colour.get_green(), foreground_colour.get_blue() );
	cr->rectangle( info_outline.get_x() + 5, info_outline.get_y() + 10, info_outline.get_width() - 10, height );
	cr->stroke();

	// blank the area behind the word "Information"
    cr->get_text_extents( "Information", extents );

	cr->set_source_rgb( background_colour.get_red(), background_colour.get_green(), background_colour.get_blue() );
	cr->rectangle( info_outline.get_x() + 10, info_outline.get_y() + 5, extents.width + 10, extents.height );
	cr->fill();

	// now paint the word "Information"
    cr->set_source_rgb(  foreground_colour.get_red(), foreground_colour.get_green(), foreground_colour.get_blue() );
    cr->move_to( info_outline.get_x() + 15, info_outline.get_y() + extents.height + 5 );
    cr->show_text( "Information" );

	// paint the information
    cr->get_text_extents( info_data[9].first, extents ); // "Bestline" is the longest word

    double offset_y = info_outline.get_y() + extents.height + 25;
    double label_offset_x = info_outline.get_x() + 10;
    double info_offset_x = info_outline.get_x() + 10 + extents.width + 10;

    vector< pair<string,string> >::iterator last_item = info_data.end();

    if( ! show_bestline_info )
        --last_item;

    for( vector< pair<string,string> >::iterator it = info_data.begin(); it != last_item; ++it ) {

        cr->move_to( label_offset_x, offset_y );
        cr->show_text( (*it).first );

        cr->move_to( info_offset_x, offset_y );
        cr->show_text( (*it).second );

        offset_y += extents.height + 5;
    }

    return false;
}

/**-----------------------------------------------------------------------------
 * \brief
 */
bool ChessBoard::draw_edit( const Cairo::RefPtr<Cairo::Context>& cr )
{
	// Draw border and pieces bitmap
	cr->rectangle( edit_outline.get_x(), edit_outline.get_y(), edit_outline.get_width(), edit_outline.get_height() );
	cr->set_source_rgb( 0, 0, 0);								// border
	cr->stroke_preserve();
	cr->set_source( pieces_surface_, edit_outline.get_x(), edit_outline.get_y());	// pieces bitmap
	cr->fill();

	return false;
}

/**-----------------------------------------------------------------------------
 * \brief
 */
bool ChessBoard::draw_drag_piece( const Cairo::RefPtr<Cairo::Context>& cr )
{
	Gdk::Rectangle dest = Gdk::Rectangle( drag_point.get_x(), drag_point.get_y(), SQUARE_SIZE, SQUARE_SIZE );
	Gdk::Point source = Gdk::Point( dest.get_x() - source_offsets[drag_code].get_x(), dest.get_y() - source_offsets[drag_code].get_y() );

	cr->set_source( pieces_surface_, source.get_x(), source.get_y() );
	cr->rectangle( dest.get_x(), dest.get_y(), dest.get_width(), dest.get_height() );
	cr->fill();

	return false;
}

/**-----------------------------------------------------------------------------
 * \brief
 */
bool ChessBoard::draw_animate_piece( const Cairo::RefPtr<Cairo::Context>& cr )
{
	Gdk::Rectangle dest = Gdk::Rectangle( animate_point.get_x(), animate_point.get_y(), SQUARE_SIZE, SQUARE_SIZE );
	Gdk::Point source = Gdk::Point( dest.get_x() - source_offsets[animate_code].get_x(), dest.get_y() - source_offsets[animate_code].get_y() );

	cr->set_source( pieces_surface_, source.get_x(), source.get_y() );
	cr->rectangle( dest.get_x(), dest.get_y(), dest.get_width(), dest.get_height() );
	cr->fill();

	return false;
}

/**-----------------------------------------------------------------------------
 * \brief Paint a chessboard
 *
 * \param cr const Cairo::RefPtr<Cairo::Context>&
 * \return bool
 *
 */
bool ChessBoard::on_draw( const Cairo::RefPtr<Cairo::Context>& cr )
{
	// Draw background
	cr->set_source_rgb( background_colour.get_red(), background_colour.get_green(), background_colour.get_blue() );
 	cr->paint();

 	draw_board( cr );

	if( !pieces.empty() )
		draw_pieces( cr );

    // Now draw either the info window or the edit window
    if( is_edit )
		draw_edit( cr );
    else
    	draw_info( cr );

	// Draw the drag piece
    if( drag_code != ' ' )
		draw_drag_piece( cr );

	if( animate_code != ' ' )
		draw_animate_piece( cr );

	return false;

}

/**-----------------------------------------------------------------------------
 * \brief Invalidating the window causes the on_draw function to be called
 *
 * \return void
 *
 */
void ChessBoard::update()
{
    Glib::RefPtr<Gdk::Window> win = get_window();
    if( !win )      // window hqs not been realised yet
        return;

    win->invalidate( true);
}

/**-----------------------------------------------------------------------------
 * \brief
 *
 * \return pair<int,int>
 *
 */
STSquare ChessBoard::calc_square_from_point( Gdk::Point point )
{
	STSquare square;

    square = make_square(
        (point.get_x() - board_outline.get_x()) / SQUARE_SIZE,
        (point.get_y() - board_outline.get_y()) / SQUARE_SIZE
    );

	if( reversed )
		square.file = 7 - square.file;
	else
		square.rank = 7 - square.rank;

	return square;
}

/**-----------------------------------------------------------------------------
 * \brief
 *
 * The bitmap is organised as:
 *          KQRBNP
 *          kqrbnp
 *
 * \param point Gdk::Point
 * \return char
 *
 */
char ChessBoard::calc_piece_from_point( Gdk::Point point )
{
	static string piece_chars = "KQRBNPkqrbnp";

	// to find out which piece we got, we need to calculate how many squares in and down the point falls
	int col = (point.get_x() - edit_outline.get_x()) / SQUARE_SIZE;
	int row = (point.get_y() - edit_outline.get_y()) / SQUARE_SIZE;

	return piece_chars[ row * 6 + col ];
}

/**-----------------------------------------------------------------------------
 * \brief
 *
 * \return char
 *
 */
char ChessBoard::calc_piece_from_square( STSquare square )
{
	typename map<STSquare,char>::iterator it = pieces.find( square );

	return ( it != pieces.end() ) ? (*it).second : ' ';
}

/**-----------------------------------------------------------------------------
 * \brief
 *
 * \return void
 *
 */
bool ChessBoard::on_button_press_event( GdkEventButton* button_event )
{
    Gdk::Rectangle mouse_pos = Gdk::Rectangle( button_event->x, button_event->y, 1, 1 );

    if( board_outline.intersects( mouse_pos ) ) {

		drag_start_square = calc_square_from_point( Gdk::Point(button_event->x, button_event->y) );

		drag_code = calc_piece_from_square( drag_start_square );
		drag_point = Gdk::Point(button_event->x - .5 * SQUARE_SIZE, button_event->y - .5 * SQUARE_SIZE);

		update();

        return true;
    }

    if( is_edit && edit_outline.intersects( mouse_pos ) ) {

		drag_code = calc_piece_from_point( Gdk::Point(button_event->x, button_event->y ) );
		drag_point = Gdk::Point(button_event->x - .5 * SQUARE_SIZE, button_event->y - .5 * SQUARE_SIZE);

		update();

		return true;
    }

    return true;
}

/**-----------------------------------------------------------------------------
 * \brief
 *
 * \return void
 *
 */
bool ChessBoard::on_button_release_event( GdkEventButton* release_event )
{
    if( drag_code == ' ' )
        return true;

    Gdk::Rectangle mouse_pos = Gdk::Rectangle( release_event->x, release_event->y, 1, 1 );
    bool intersecting = board_outline.intersects( mouse_pos );

    if( is_edit ) {
		drag_code = ' ';
		update();

		if( intersecting ) {
			STSquare end_square = calc_square_from_point( Gdk::Point(release_event->x, release_event->y) );
			controller.do_arrange_drop( end_square, drag_code );
		} else
			controller.do_arrange_drop( drag_start_square, ' ' ); /* dropping a space is removing the piece */

		return true;
    }

	drag_code = ' ';
	update();

    if( !intersecting ) {
        return true;
    }

	// here we need to do the move
	STSquare end_square = calc_square_from_point( Gdk::Point(release_event->x, release_event->y) );
	controller.make_move( drag_start_square, end_square );

    return true;
}

/**-----------------------------------------------------------------------------
 * \brief
 *
 * \return void
 *
 */
bool ChessBoard::on_motion_notify_event( GdkEventMotion* motion_event )
{
    if( drag_code == ' ' )
        return true;

    drag_point = Gdk::Point( motion_event->x - .5 * SQUARE_SIZE, motion_event->y - .5 * SQUARE_SIZE );

    update();

    return true;
}

/**-----------------------------------------------------------------------------
 * \brief
 *
 * \param FEN_string string
 * \return void
 *
 */
void ChessBoard::set_piece_positions( std::string FEN_string )
{
    STSquare square = make_square(0,7);         // first = file, second is rank

    pieces.clear();

    for( char& code : FEN_string ) {

        if( (code > '0') && (code < '9') ) {       // Character can either be a number (squares to skip)
           square.file += (code - '0' );
        } else if( code == '/' ) {                      // ... or an end of rank indicator
            --square.rank;
            square.file = 0;
        } else {                                    // or an piece designator
        	pieces.insert( std::pair<STSquare,char>(square, code) );
            ++square.file;
        }
    }

    animate_code = ' ';

    update();
}

/**-----------------------------------------------------------------------------
 * \brief
 *
 * \param bg Gdk::RGBA
 * \param white Gdk::RGBA
 * \param black Gdk::RGBA
 * \return void
 *
 */
void ChessBoard::set_colours( Gdk::RGBA bg, Gdk::RGBA white, Gdk::RGBA black, Gdk::RGBA fg )
{
	using namespace Cairo;

    background_colour = bg;
    white_colour = white;
    black_colour = black;
	foreground_colour = fg;

	/* Create a context for the background image object so we can draw on it */
	RefPtr<Context> context = Context::create( background_image_ );

	/* draw the board background */
	for( int cell=0; cell<64; cell++ ) {
		if( (cell + (cell/8)) % 2 )
			context->set_source_rgb( white.get_red(), white.get_green(), white.get_blue() );
		else
			context->set_source_rgb( black.get_red(), black.get_green(), black.get_blue() );

		context->rectangle( (cell%8) * SQUARE_SIZE, (cell/8) * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE );
		context->fill();
	}

	update();   // Redraw the chessboard
}

/**-----------------------------------------------------------------------------
 * \brief Turn the board up side down
 *
 * \return bool
 *
 */
void ChessBoard::toggle_reverse()
{
    reversed = !reversed;
    update();
}

/**-----------------------------------------------------------------------------
 * \brief
 *
 */
void ChessBoard::toggle_bestline()
{
    show_bestline_info = ! show_bestline_info;
    update();
}

/**-----------------------------------------------------------------------------
 * \brief
 *
 */
void ChessBoard::set_edit( bool on )
{
	if( is_edit == on )
		return;

	is_edit = on;
	update();
}

/**-----------------------------------------------------------------------------
 * \brief
 *
 */
void ChessBoard::set_info( STInfo& info )
{
    info_data[0].second = info.turn;
    info_data[1].second = info.white;
    info_data[2].second = info.black;
    info_data[3].second = info.time;
    info_data[4].second = info.level;
    info_data[5].second = info.value;
    info_data[6].second = info.nodes;
    info_data[7].second = info.n_sec;
    info_data[8].second = info.depth;
    info_data[9].second = info.bestline;

    update();
}

/**-----------------------------------------------------------------------------
 * \brief
 *
 */
void ChessBoard::animate_start( STSquare start_square, STSquare end_square, char piece )
{
	Gdk::Point end_point;

	if( reversed ) {
		start_square.file = 7 - start_square.file;
		end_square.file = 7 - end_square.file;
	} else {
		start_square.rank = 7 - start_square.rank;
		end_square.rank = 7 - end_square.rank;
	}

	animate_point.set_x( board_outline.get_x() + start_square.file * SQUARE_SIZE );
	animate_point.set_y( board_outline.get_y() + start_square.rank * SQUARE_SIZE );

	end_point.set_x( board_outline.get_x() + end_square.file * SQUARE_SIZE );
	end_point.set_y( board_outline.get_y() + end_square.rank * SQUARE_SIZE );

	annimate_delta.set_x( (end_point.get_x() - animate_point.get_x()) / 10 );
	annimate_delta.set_y( (end_point.get_y() - animate_point.get_y()) / 10 );

	animate_point.set_x( animate_point.get_x() + annimate_delta.get_x() );
	animate_point.set_y( animate_point.get_y() + annimate_delta.get_y() );

	animate_code = piece;

	update();
}

/**-----------------------------------------------------------------------------
 * \brief
 *
 */
void ChessBoard::animate_step( )
{
	animate_point.set_x( animate_point.get_x() + annimate_delta.get_x() );
	animate_point.set_y( animate_point.get_y() + annimate_delta.get_y() );

	update();
}

/**-----------------------------------------------------------------------------
 * \brief
 *
 */
void ChessBoard::animate_stop()
{
	animate_code = ' ';

	update();
}
