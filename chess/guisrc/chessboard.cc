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
		Gdk::Point floating_piece_source = floating_piece_position - piececode_to_editpoint( floating_piece_code );
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

		uint16_t drag_start_square = point_to_square( mouse_point );
		char piece_code = pieces[drag_start_square];

		if( piece_code == ' ' )
			return true;

		start_dragging( mouse_point, piece_code );

		button_event->x = drag_start_square;
		button_event->state = piece_code;

		return false;
    }

    if( is_edit && edit_outline.intersects( mouse_pos ) ) {

    	Gdk::Point edit_point = mouse_point - Gdk::Point( edit_outline.get_x(), edit_outline.get_y() );	// Make the point relative to the edit "widget"
		char piece_code = editpoint_to_piececode( edit_point );

		start_dragging( mouse_point, piece_code );

		button_event->x = -1;
		button_event->state = piece_code;

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
		release_event->x = point_to_square( Gdk::Point(release_event->x, release_event->y) );
		return false;		// keep processing
	}

	if( is_edit ) {
		release_event->x = -1;
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
	for( int index = 0; index < 64; ++index ) {

		if( pieces[index] == ' ' )
			continue;

		Gdk::Point dest = square_to_point( index ) - Gdk::Point( board_outline.get_x(), board_outline.get_y() );
		Gdk::Point source = dest - piececode_to_editpoint( pieces[index] );

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
void ChessBoard::set_piece_positions( std::array<char,64> new_pieces )
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

void ChessBoard::set_info( std::array<std::pair<std::string,std::string>,10> the_info )
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
void ChessBoard::animate_start( uint16_t start_square, uint16_t end_square, char piece )
{
	floating_piece_position = square_to_point( start_square );
	floating_piece_code = piece;
	draw_floating_piece = true;

	annimate_delta = (square_to_point( end_square ) - floating_piece_position) / 10;
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

void ChessBoard::highlight_start( uint16_t square )
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
uint16_t ChessBoard::point_to_square( Gdk::Point point )
{
	point = point - Gdk::Point( board_outline.get_x(), board_outline.get_y() );
	point = point - Gdk::Point( 1, 1 );

	uint16_t square = point.get_x() / SQUARE_SIZE + 8 * (7 - (point.get_y() / SQUARE_SIZE));

	if( is_reversed )
		square ^= 56;

	return square;
}

Gdk::Point ChessBoard::square_to_point( uint16_t square )
{
	if( is_reversed )
		square ^= 56;

	Gdk::Point point( (square % 8) * SQUARE_SIZE, (7 - (square / 8)) * SQUARE_SIZE );

	point = point + Gdk::Point( 1, 1 );
	point = point + Gdk::Point( board_outline.get_x(), board_outline.get_y() );

	return point;
}

//The bitmap is organised as:
//         KQRBNP
//         kqrbnp
Gdk::Point ChessBoard::piececode_to_editpoint( char piece_code )
{
	static std::string piece_chars = "KQRBNPkqrbnp";

	unsigned int index = piece_chars.find( piece_code );

	return Gdk::Point( (index % 6) * SQUARE_SIZE + 1, (index / 6) * SQUARE_SIZE + 1 );
}

char ChessBoard::editpoint_to_piececode( Gdk::Point point )
{
	static std::string piece_chars = "KQRBNPkqrbnp";

	return piece_chars[ (point.get_x() / SQUARE_SIZE) + 6 * (point.get_y() / SQUARE_SIZE) ];
}
