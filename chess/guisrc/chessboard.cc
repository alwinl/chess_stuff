/*
 * Copyright 2022 Alwin Leerling <dna.leerling@gmail.com>
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

#include "chessboard.h"

Gdk::Point operator-( const Gdk::Point& lhs, const Gdk::Point& rhs )
	{ return Gdk::Point( lhs.get_x() - rhs.get_x(), lhs.get_y() - rhs.get_y() ); }

Gdk::Point operator+( const Gdk::Point& lhs, const Gdk::Point& rhs )
	{ return Gdk::Point( lhs.get_x() + rhs.get_x(), lhs.get_y() + rhs.get_y() ); }

Gdk::Point operator/( const Gdk::Point& lhs, int denimonator )
	{ return Gdk::Point( lhs.get_x() / denimonator, lhs.get_y() / denimonator ); }


/**-----------------------------------------------------------------------------
 * \brief ChessBoard constructor
 *
 * \param cobject BaseObjectType*
 * \param ui_model const Glib::RefPtr<Gtk::Builder>&
 *
 */
ChessBoard::ChessBoard( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model ) : Gtk::DrawingArea(cobject)
{
	// create our image objects
	background_image = Cairo::ImageSurface::create( Cairo::FORMAT_RGB24, 8*SQUARE_SIZE + 2, 8*SQUARE_SIZE + 2 );	// one pixel border
	pieces_image = Cairo::ImageSurface::create( Cairo::FORMAT_ARGB32, 6*SQUARE_SIZE + 2, 2*SQUARE_SIZE + 2 );
	info_image = Cairo::ImageSurface::create( Cairo::FORMAT_ARGB32, INFO_WIDTH, MAX_HEIGHT );
	pieces_overlay = Cairo::ImageSurface::create( Cairo::FORMAT_ARGB32, 8*SQUARE_SIZE, 8*SQUARE_SIZE );

    paint_edit_pieces();
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
			/* the left side of the board is midway of our allocated width minus half the board */
            (allocation.get_width() - INFO_WIDTH)/2 - 4 * SQUARE_SIZE,
			/* the top side of the board is midway of the height minus half the board height */
            allocation.get_height()/2 - 4 * SQUARE_SIZE,
            8 * SQUARE_SIZE + 2,
            8 * SQUARE_SIZE + 2
    );

	info_outline = Gdk::Rectangle(
			/* the info widow is placed on the right side of the window */
			allocation.get_width() - INFO_WIDTH,
			0,
			INFO_WIDTH,
			allocation.get_height()
	);

	edit_outline = Gdk::Rectangle(
			/* the edit window is halfway of the info width minus the width of half the pieces bitmap */
			allocation.get_width() - INFO_WIDTH/2 - 3 * SQUARE_SIZE,
			allocation.get_height()/2 - 1 * SQUARE_SIZE,
			6 * SQUARE_SIZE + 2,
			2 * SQUARE_SIZE + 2
	);

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
	/* Draw background */
	cr->set_source_rgb( background_colour.get_red(), background_colour.get_green(), background_colour.get_blue() );
 	cr->paint();

 	/* draw the board */
	cr->rectangle( board_outline.get_x(), board_outline.get_y(), board_outline.get_width(), board_outline.get_height() );
	cr->set_source( background_image, board_outline.get_x(), board_outline.get_y());
	cr->fill();

	/* draw the pieces */
	cr->rectangle( board_outline.get_x() + 1, board_outline.get_y() + 1, board_outline.get_width() - 2, board_outline.get_height() - 2 );	// deduct the border
	cr->set_source( pieces_overlay, board_outline.get_x(), board_outline.get_y() );
	cr->fill();

    // Now draw either the info window or the edit window
    if( is_edit ) {
		/* draw edit window */
		cr->rectangle( edit_outline.get_x(), edit_outline.get_y(), edit_outline.get_width(), edit_outline.get_height() );
		cr->set_source( pieces_image, edit_outline.get_x(), edit_outline.get_y());
		cr->fill();
    } else {
    	/* draw info window */
		cr->rectangle( info_outline.get_x(), info_outline.get_y(), info_outline.get_width(), info_outline.get_height() );
		cr->set_source( info_image, info_outline.get_x(), info_outline.get_y());
		cr->fill();
    }

	if( draw_floating_piece ) {
		Gdk::Point floating_piece_source = floating_piece_position - piece_source_offset( floating_piece_code );
		cr->rectangle( floating_piece_position.get_x(), floating_piece_position.get_y(), SQUARE_SIZE, SQUARE_SIZE );
		cr->set_source( pieces_image, floating_piece_source.get_x(), floating_piece_source.get_y() );
		cr->fill();
    }

	if( draw_highlight ) {
		cr->rectangle( highlight_pos.get_x(), highlight_pos.get_y(), SQUARE_SIZE, SQUARE_SIZE );
		cr->set_source_rgb(  foreground_colour.get_red(), foreground_colour.get_green(), foreground_colour.get_blue() );
		cr->set_line_width( 4.0 );
		cr->stroke();
	}

	return false;
}

bool ChessBoard::on_button_press_event( GdkEventButton* button_event )
{
	if( is_animating || is_computer_move )		// block user input during animations
		return true;

    Gdk::Rectangle mouse_pos = Gdk::Rectangle( button_event->x, button_event->y, 1, 1 );
    Gdk::Point mouse_point = Gdk::Point(button_event->x, button_event->y);

    if( board_outline.intersects( mouse_pos ) ) {

		STSquare drag_start_square = point_to_square( mouse_point );
		char piece_code = pieces.at(drag_start_square);

		start_dragging( mouse_point, piece_code );

		button_event->x = drag_start_square.file;
		button_event->y = drag_start_square.rank;
		button_event->state = piece_code;

		return false;
    }

    if( is_edit && edit_outline.intersects( mouse_pos ) ) {

		start_dragging( mouse_point, point_to_piece_code( mouse_point ) );

		button_event->x = -1;
		button_event->y = -1;
		button_event->state = point_to_piece_code( mouse_point );

		return false;
    }

    return true;
}

bool ChessBoard::on_motion_notify_event( GdkEventMotion* motion_event )
{
	if( is_animating || !is_dragging )
		return true;

	update_dragging( Gdk::Point(motion_event->x, motion_event->y) );

    return true;
}

bool ChessBoard::on_button_release_event( GdkEventButton* release_event )
{
	if( is_animating || !is_dragging )
		return true;

    stop_dragging();

    Gdk::Rectangle mouse_pos = Gdk::Rectangle( release_event->x, release_event->y, 1, 1 );

	if( board_outline.intersects( mouse_pos ) ) {
        STSquare drag_end_square = point_to_square( Gdk::Point(release_event->x, release_event->y) );
		release_event->x = drag_end_square.file;
		release_event->y = drag_end_square.rank;
		return false;		// keep processing
	}

	if( is_edit ) {
		release_event->x = -1;
		release_event->y = -1;
        return false;		// keep processing
	}

    return true;
}

/**-----------------------------------------------------------------------------
 * The next set of functions create the images the draw function uses
 */
void ChessBoard::paint_board( )
{
	/* Create a context for the background image object so we can draw on it */
	Cairo::RefPtr<Cairo::Context> context = Cairo::Context::create( background_image );

	/* paint the complete image in the black colour */
	context->rectangle( 0, 0, board_outline.get_width(), board_outline.get_height() );

	context->set_source_rgb( black_colour.get_red(), black_colour.get_green(), black_colour.get_blue() );
	context->fill_preserve();

	/* paint the border */
	context->set_source_rgb( 0, 0, 0);
	context->stroke();

	/* Now paint each alternate square in the white colour */
	context->set_source_rgb( white_colour.get_red(), white_colour.get_green(), white_colour.get_blue() );
	for( int cell=0; cell<64; cell++ ) {
		if( !( (cell + (cell/8)) % 2 ) ) {
			context->rectangle( (cell%8) * SQUARE_SIZE + 1, (cell/8) * SQUARE_SIZE + 1, SQUARE_SIZE, SQUARE_SIZE );
			context->fill();
		}
	}
}

void ChessBoard::paint_pieces()
{
	Cairo::RefPtr<Cairo::Context> context = Cairo::Context::create( pieces_overlay );

	context->save();
	context->set_operator( Cairo::OPERATOR_CLEAR );
	context->paint();
	context->restore();

	// draw the pieces
	for( std::pair<const STSquare,char>& entry : pieces ) {

		Gdk::Point dest = square_to_board_point( entry.first );
		Gdk::Point source = dest - piece_source_offset( entry.second );

		context->set_source( pieces_image, source.get_x(), source.get_y() );
		context->rectangle( dest.get_x(), dest.get_y(), SQUARE_SIZE, SQUARE_SIZE );
		context->fill();
	}
}

void ChessBoard::paint_edit_pieces()
{
	// Create a context for the pieces bitmap so we can draw on it
	Cairo::RefPtr<Cairo::Context> context = Cairo::Context::create( pieces_image );

	// load and draw the image onto the surface
	Glib::RefPtr<Gdk::Pixbuf> pieces_pixbuf = Gdk::Pixbuf::create_from_resource("/net/dnatechnologies/chess/pieces.png");

	/* Paint pieces */
	context->rectangle( 1, 1, 6 * SQUARE_SIZE, 2 * SQUARE_SIZE );
	Gdk::Cairo::set_source_pixbuf( context, pieces_pixbuf, 0.0, 0.0 );
	context->fill();

	/* Paint border */
	context->rectangle( 0, 0, 6 * SQUARE_SIZE + 2, 2 * SQUARE_SIZE + 2 );
	context->set_source_rgb( 0, 0, 0);
	context->stroke();
}

void ChessBoard::paint_info()
{
	Cairo::RefPtr<Cairo::Context> context = Cairo::Context::create( info_image );
    Cairo::TextExtents extents;

    context->set_font_size( 12 );
    context->get_text_extents( "Bestline", extents ); // Need it for the height of the frame

    // Blank complete image
	context->save();
	context->set_operator( Cairo::OPERATOR_CLEAR );
	context->paint();
	context->restore();

 	// Paint a frame
    int height = (show_bestline ? 12 : 11 ) * (extents.height + 5);

    context->set_source_rgb(  foreground_colour.get_red(), foreground_colour.get_green(), foreground_colour.get_blue() );
	context->rectangle( 5, 10, info_outline.get_width() - 10, height );
	context->stroke();

	// Blank the frame portion behind the word "Information"
	context->save();
	context->set_operator( Cairo::OPERATOR_CLEAR );
    context->get_text_extents( "Information", extents );
	context->rectangle( 10, 5, extents.width + 10, extents.height );
	context->fill();
	context->restore();

	// Paint the word "Information"
    context->set_source_rgb(  foreground_colour.get_red(), foreground_colour.get_green(), foreground_colour.get_blue() );
    context->move_to( 15, extents.height + 5 );
    context->show_text( "Information" );

	// Paint the actual information
    double offset_y = extents.height + 25;
    double label_offset_x = 10;
    double info_offset_x = 10 + extents.width + 10;

	auto end_it = info.end();
	if( ! show_bestline )
		--end_it;

	for( auto it = info.begin(); it != end_it; ++it ) {

		context->move_to( label_offset_x, offset_y );
		context->show_text( std::get<0>(*it) );

		context->move_to( info_offset_x, offset_y );
		context->show_text( std::get<1>(*it) );

		offset_y += extents.height + 5;
	}
}

/**-----------------------------------------------------------------------------
 * The next set of functions update the state of the ChessBoard
 */
void ChessBoard::set_piece_positions( std::map<STSquare,char> new_pieces )
{
	pieces = new_pieces;

	paint_pieces();

    queue_draw();
}

void ChessBoard::set_colours( std::array<std::string,4> new_colours )
{
    background_colour = Gdk::RGBA(new_colours[0]);
	foreground_colour = Gdk::RGBA(new_colours[1]);
	white_colour =  Gdk::RGBA(new_colours[2]);
	black_colour =  Gdk::RGBA(new_colours[3]);

	paint_board();
	paint_info();

	queue_draw();
}

void ChessBoard::toggle_reverse()
{
    is_reversed = !is_reversed;

	paint_pieces();

    queue_draw();
}

void ChessBoard::set_edit( bool on )
{
	if( is_edit == on )
		return;

	is_edit = on;

	queue_draw();
}

void ChessBoard::set_info( std::array<std::pair<std::string,std::string>,10>  the_info )
{
	info = the_info;

    paint_info( );

    queue_draw();
}

void ChessBoard::toggle_bestline()
{
    show_bestline = !show_bestline;

    paint_info();

    queue_draw();
}

/**-----------------------------------------------------------------------------
 * Dragging functionality
 */
void ChessBoard::start_dragging( Gdk::Point start_point, char piece_code )
{
	floating_piece_position = start_point - Gdk::Point(.5 * SQUARE_SIZE, .5 * SQUARE_SIZE);
	floating_piece_code = piece_code;
	draw_floating_piece = true;

	is_dragging = true;

	queue_draw();
}

void ChessBoard::update_dragging( Gdk::Point new_point )
{
	floating_piece_position = new_point - Gdk::Point(.5 * SQUARE_SIZE, .5 * SQUARE_SIZE);

    queue_draw();
}

void ChessBoard::stop_dragging()
{
	draw_floating_piece = false;
	is_dragging = false;

	queue_draw();
}

/**-----------------------------------------------------------------------------
 * Animation functionality
 *
 */
void ChessBoard::animate_start( STSquare& start_square, STSquare& end_square, char piece )
{
	Gdk::Point end_point = square_to_point( end_square );

	floating_piece_position = square_to_point( start_square );
	floating_piece_code = piece;
	draw_floating_piece = true;

	annimate_delta = (end_point - floating_piece_position) / 10;

	is_animating = true;

	// make the first step
	floating_piece_position = floating_piece_position + annimate_delta;

	queue_draw();
}

void ChessBoard::animate_continue()
{
	floating_piece_position = floating_piece_position + annimate_delta;

	queue_draw();
}

void ChessBoard::animate_finish()
{
	draw_floating_piece = false;
	is_animating = false;

	queue_draw();
}

/**-----------------------------------------------------------------------------
 * Highlight functionality
 *
 */

void ChessBoard::highlight_start( STSquare square )
{
	highlight_pos = square_to_point( square );
	draw_highlight = true;
	queue_draw();
}

void ChessBoard::highlight_continue()
{
	draw_highlight = !draw_highlight;
	queue_draw();
}

void ChessBoard::highlight_finish()
{
	draw_highlight = false;
	queue_draw();
}

/**-----------------------------------------------------------------------------
 * The following set of functions are utility functions
 */
STSquare ChessBoard::adjust_for_reverse( STSquare square )
{
	if( is_reversed )
		square.file = 7 - square.file;
	else
		square.rank = 7 - square.rank;

	return square;
}

STSquare ChessBoard::point_to_square( Gdk::Point point )
{
	STSquare square;

    square.file = (point.get_x() - (board_outline.get_x() - 1) ) / SQUARE_SIZE;
    square.rank = (point.get_y() - (board_outline.get_y() - 1) ) / SQUARE_SIZE;

    square = adjust_for_reverse( square );

	return square;
}

Gdk::Point ChessBoard::square_to_point( STSquare square )
{
    square = adjust_for_reverse( square );

	return Gdk::Point(
		board_outline.get_x() + 1 + square.file * SQUARE_SIZE,
		board_outline.get_y() + 1 + square.rank * SQUARE_SIZE
	);
}

Gdk::Point ChessBoard::square_to_board_point( STSquare square )
{
	square = adjust_for_reverse( square );

	return Gdk::Point(
		1 + square.file * SQUARE_SIZE,
		1 + square.rank * SQUARE_SIZE
	);
}

Gdk::Point ChessBoard::piece_source_offset( char piece_code )
{
	static std::map<char,Gdk::Point> pieces_image_offsets = {
		// The pieces bitmap contains king through to pawn, white pieces in top row and black pieces in bottom row
		{ 'K', Gdk::Point( 0 * SQUARE_SIZE + 1, 0 * SQUARE_SIZE + 1 ) },
		{ 'Q', Gdk::Point( 1 * SQUARE_SIZE + 1, 0 * SQUARE_SIZE + 1 ) },
		{ 'R', Gdk::Point( 2 * SQUARE_SIZE + 1, 0 * SQUARE_SIZE + 1 ) },
		{ 'B', Gdk::Point( 3 * SQUARE_SIZE + 1, 0 * SQUARE_SIZE + 1 ) },
		{ 'N', Gdk::Point( 4 * SQUARE_SIZE + 1, 0 * SQUARE_SIZE + 1 ) },
		{ 'P', Gdk::Point( 5 * SQUARE_SIZE + 1, 0 * SQUARE_SIZE + 1 ) },
		{ 'k', Gdk::Point( 0 * SQUARE_SIZE + 1, 1 * SQUARE_SIZE + 1 ) },
		{ 'q', Gdk::Point( 1 * SQUARE_SIZE + 1, 1 * SQUARE_SIZE + 1 ) },
		{ 'r', Gdk::Point( 2 * SQUARE_SIZE + 1, 1 * SQUARE_SIZE + 1 ) },
		{ 'b', Gdk::Point( 3 * SQUARE_SIZE + 1, 1 * SQUARE_SIZE + 1 ) },
		{ 'n', Gdk::Point( 4 * SQUARE_SIZE + 1, 1 * SQUARE_SIZE + 1 ) },
		{ 'p', Gdk::Point( 5 * SQUARE_SIZE + 1, 1 * SQUARE_SIZE + 1 ) },
	};

	// To paint a particular piece we have to offset our source bitmap so that the correct bitmap
	// overlays our square to be painted. The pieces are stored as two rows (a white and a black row)
	// in six columns, calculable by type.
	return pieces_image_offsets[piece_code];
}

char ChessBoard::point_to_piece_code( Gdk::Point point )
{
	//The bitmap is organised as:
	//         KQRBNP
	//         kqrbnp
	static std::string piece_chars = "KQRBNPkqrbnp";

	// to find out which piece we got, we need to calculate how many squares in and down the point falls
	int col = (point.get_x() - edit_outline.get_x()) / SQUARE_SIZE;
	int row = (point.get_y() - edit_outline.get_y()) / SQUARE_SIZE;

	return piece_chars[ row * 6 + col ];
}
