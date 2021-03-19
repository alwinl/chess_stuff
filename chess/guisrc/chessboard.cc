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

/**-----------------------------------------------------------------------------
 * \brief ChessBoard constructor
 *
 * \param cobject BaseObjectType*
 * \param ui_model const Glib::RefPtr<Gtk::Builder>&
 * \param acontroller ChessController&
 *
 */
ChessBoard::ChessBoard( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model ) : Gtk::DrawingArea(cobject)
{
	// create our image objects
	background_image = Cairo::ImageSurface::create( Cairo::FORMAT_RGB24, 8*SQUARE_SIZE + 2, 8*SQUARE_SIZE + 2 );	// one pixel border
	pieces_image = Cairo::ImageSurface::create( Cairo::FORMAT_ARGB32, 6*SQUARE_SIZE + 2, 2*SQUARE_SIZE + 2 );
	info_image = Cairo::ImageSurface::create( Cairo::FORMAT_ARGB32, INFO_WIDTH, MAX_HEIGHT );
	pieces_overlay = Cairo::ImageSurface::create( Cairo::FORMAT_ARGB32, 8*SQUARE_SIZE, 8*SQUARE_SIZE );

    show_bestline = true;
	draw_highlight = false;
	is_dragging = false;
	is_animating = false;
	is_reversed = false;
	is_edit = false;

	floating_piece_code = ' ';

	timeout_counter = 0;

	// The pieces bitmap contains king through to pawn, white pieces in top row and black pieces in bottom row
    pieces_image_offsets['K'] = Gdk::Point( 0 * SQUARE_SIZE + 1, 0 * SQUARE_SIZE + 1 );
    pieces_image_offsets['Q'] = Gdk::Point( 1 * SQUARE_SIZE + 1, 0 * SQUARE_SIZE + 1 );
    pieces_image_offsets['R'] = Gdk::Point( 2 * SQUARE_SIZE + 1, 0 * SQUARE_SIZE + 1 );
    pieces_image_offsets['B'] = Gdk::Point( 3 * SQUARE_SIZE + 1, 0 * SQUARE_SIZE + 1 );
    pieces_image_offsets['N'] = Gdk::Point( 4 * SQUARE_SIZE + 1, 0 * SQUARE_SIZE + 1 );
	pieces_image_offsets['P'] = Gdk::Point( 5 * SQUARE_SIZE + 1, 0 * SQUARE_SIZE + 1 );

    pieces_image_offsets['k'] = Gdk::Point( 0 * SQUARE_SIZE + 1, 1 * SQUARE_SIZE + 1 );
    pieces_image_offsets['q'] = Gdk::Point( 1 * SQUARE_SIZE + 1, 1 * SQUARE_SIZE + 1 );
    pieces_image_offsets['r'] = Gdk::Point( 2 * SQUARE_SIZE + 1, 1 * SQUARE_SIZE + 1 );
    pieces_image_offsets['b'] = Gdk::Point( 3 * SQUARE_SIZE + 1, 1 * SQUARE_SIZE + 1 );
    pieces_image_offsets['n'] = Gdk::Point( 4 * SQUARE_SIZE + 1, 1 * SQUARE_SIZE + 1 );
    pieces_image_offsets['p'] = Gdk::Point( 5 * SQUARE_SIZE + 1, 1 * SQUARE_SIZE + 1 );

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

    if( floating_piece_code != ' ' ) {
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

/**-----------------------------------------------------------------------------
 * \brief
 */
STSquare ChessBoard::adjust_for_reverse( STSquare square )
{
	if( is_reversed )
		square.file = 7 - square.file;
	else
		square.rank = 7 - square.rank;

	return square;
}

/**-----------------------------------------------------------------------------
 * \brief
 */
STSquare ChessBoard::point_to_square( Gdk::Point point )
{
	STSquare square;

    square.file = (point.get_x() - (board_outline.get_x() - 1) ) / SQUARE_SIZE;
    square.rank = (point.get_y() - (board_outline.get_y() - 1) ) / SQUARE_SIZE;

    square = adjust_for_reverse( square );

	return square;
}

/**-----------------------------------------------------------------------------
 * \brief
 */
Gdk::Point ChessBoard::square_to_point( STSquare square )
{
	Gdk::Point pt;

    square = adjust_for_reverse( square );

	pt.set_x( (board_outline.get_x() + 1 ) + square.file * SQUARE_SIZE );
	pt.set_y( (board_outline.get_y() + 1 ) + square.rank * SQUARE_SIZE );

	return pt;
}

/**-----------------------------------------------------------------------------
 * \brief
 */
Gdk::Point ChessBoard::square_to_board_point( STSquare square )
{
	Gdk::Point pt;

	square = adjust_for_reverse( square );

	pt.set_x( 1 + square.file * SQUARE_SIZE );
	pt.set_y( 1 + square.rank * SQUARE_SIZE );

	return pt;
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
char ChessBoard::point_to_edit_piece( Gdk::Point point )
{
	static std::string piece_chars = "KQRBNPkqrbnp";

	// to find out which piece we got, we need to calculate how many squares in and down the point falls
	int col = (point.get_x() - edit_outline.get_x()) / SQUARE_SIZE;
	int row = (point.get_y() - edit_outline.get_y()) / SQUARE_SIZE;

	return piece_chars[ row * 6 + col ];
}

/**-----------------------------------------------------------------------------
 * \brief
 *
 * To paint a particular piece we have to offset our source bitmap so that the correct bitmap
 * overlays our square to be painted. The pieces are stored as two rows (a white and a black row)
 * in six columns, calculable by type.
 */
Gdk::Point ChessBoard::source_point( Gdk::Point point, char piece_code )
{
	return Gdk::Point(
		point.get_x() - pieces_image_offsets[piece_code].get_x(),
		point.get_y() - pieces_image_offsets[piece_code].get_y()
	);
}

/**-----------------------------------------------------------------------------
 * \brief
 *
 * \return void
 *
 */
void ChessBoard::start_dragging( char piece, Gdk::Point start_point )
{
	floating_piece_code = piece;
	floating_piece_position = Gdk::Point( start_point.get_x() - .5 * SQUARE_SIZE, start_point.get_y() - .5 * SQUARE_SIZE );
	floating_piece_source = source_point( floating_piece_position, floating_piece_code );
	is_dragging = true;

	queue_draw();
}

/**-----------------------------------------------------------------------------
 * \brief
 *
 * \return void
 *
 */
void ChessBoard::update_dragging( Gdk::Point new_point )
{
    floating_piece_position = Gdk::Point( new_point.get_x() - .5 * SQUARE_SIZE, new_point.get_y() - .5 * SQUARE_SIZE );
    floating_piece_source = source_point( floating_piece_position, floating_piece_code );

    queue_draw();
}

/**-----------------------------------------------------------------------------
 * \brief
 *
 * \return void
 *
 */
void ChessBoard::stop_dragging()
{
	is_dragging = false;
	floating_piece_code = ' ';

	queue_draw();
}

/**-----------------------------------------------------------------------------
 * \brief
 *
 * \return void
 *
 */
bool ChessBoard::on_button_press_event( GdkEventButton* button_event )
{
	if( is_animating )		// block user input during animations
		return true;

    Gdk::Rectangle mouse_pos = Gdk::Rectangle( button_event->x, button_event->y, 1, 1 );
    Gdk::Point mouse_point = Gdk::Point(button_event->x, button_event->y);

    if( board_outline.intersects( mouse_pos ) ) {

		drag_start_square = point_to_square( mouse_point );

		typename std::map<STSquare,STPiece>::iterator it = pieces.find(drag_start_square);
		if( it != pieces.end() ) {
			STPiece piece = (*it).second;
			piece.is_dragging = true;
			(*it).second = piece;

			start_dragging( piece.code, mouse_point );

			paint_pieces();
			queue_draw();
		}

    } else if( is_edit && edit_outline.intersects( mouse_pos ) ) {

		drag_start_square.file = -1;

		start_dragging( point_to_edit_piece( mouse_point ), mouse_point );
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
	if( is_animating )		// block user input during animations
		return true;

	if( !is_dragging )
		return true;

    Gdk::Rectangle mouse_pos = Gdk::Rectangle( release_event->x, release_event->y, 1, 1 );
    save_piece_code = floating_piece_code;

    stop_dragging();

	typename std::map<STSquare,STPiece>::iterator it = pieces.find(drag_start_square);
	if( it != pieces.end() ) {
		STPiece piece = (*it).second;
		piece.is_dragging = false;
		(*it).second = piece;
	}

	drag_end_square = point_to_square( Gdk::Point(release_event->x, release_event->y) );

	if( board_outline.intersects( mouse_pos ) )
		return false;		// keep processing

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
	if( is_animating )		// block user input during animations
		return true;

	if( !is_dragging )
		return true;

	update_dragging( Gdk::Point(motion_event->x, motion_event->y) );

    return true;
}

/**-----------------------------------------------------------------------------
 * \brief
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
		if( (cell + (cell/8)) % 2 ) {
			context->rectangle( (cell%8) * SQUARE_SIZE + 1, (cell/8) * SQUARE_SIZE + 1, SQUARE_SIZE, SQUARE_SIZE );
			context->fill();
		}
	}
}

/**-----------------------------------------------------------------------------
 * \brief
 */
void ChessBoard::paint_pieces()
{
	Cairo::RefPtr<Cairo::Context> context = Cairo::Context::create( pieces_overlay );

	context->save();
	context->set_operator( Cairo::OPERATOR_CLEAR );
	context->paint();
	context->restore();

	// draw the pieces
	for( std::pair<const STSquare,STPiece>& entry : pieces ) {

		STPiece piece = entry.second;

		if( piece.is_dragging )
			continue;

		Gdk::Point dest = square_to_board_point( entry.first );
		Gdk::Point source = source_point( dest, piece.code );

		context->set_source( pieces_image, source.get_x(), source.get_y() );
		context->rectangle( dest.get_x(), dest.get_y(), SQUARE_SIZE, SQUARE_SIZE );
		context->fill();
	}
}

/**-----------------------------------------------------------------------------
 * \brief
 */
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

/**-----------------------------------------------------------------------------
 * \brief
 */
void ChessBoard::paint_text( Cairo::RefPtr<Cairo::Context>& context, double x, double y, std::string text )
{
	context->move_to( x, y );
	context->show_text( text );
}

/**-----------------------------------------------------------------------------
 * \brief
 */
void ChessBoard::paint_info( STInfo& the_info )
{
	Cairo::RefPtr<Cairo::Context> context = Cairo::Context::create( info_image );
    Cairo::TextExtents extents;

    context->set_font_size( 12 );
    context->get_text_extents( "Bestline", extents ); // Need it for the height of the frame

    int height = (show_bestline ? 12 : 11 ) * (extents.height + 5);

    // blank the complete image
	context->save();
	context->set_operator( Cairo::OPERATOR_CLEAR );
	context->paint();
	context->restore();

 	// Draw a frame
    context->set_source_rgb(  foreground_colour.get_red(), foreground_colour.get_green(), foreground_colour.get_blue() );
	context->rectangle( 5, 10, info_outline.get_width() - 10, height );
	context->stroke();

	// blank the area behind the word "Information"
	context->save();
	context->set_operator( Cairo::OPERATOR_CLEAR );
    context->get_text_extents( "Information", extents );
	context->rectangle( 10, 5, extents.width + 10, extents.height );
	context->fill();
	context->restore();

	// now paint the word "Information"
    context->set_source_rgb(  foreground_colour.get_red(), foreground_colour.get_green(), foreground_colour.get_blue() );
    context->move_to( 15, extents.height + 5 );
    context->show_text( "Information" );

	// paint the information
    context->get_text_extents( "Bestline", extents ); // "Bestline" is the longest word

    double offset_y = extents.height + 25;
    double label_offset_x = 10;
    double info_offset_x = 10 + extents.width + 10;

	paint_text( context, label_offset_x, offset_y, "Turn"  ); paint_text( context, info_offset_x, offset_y, the_info.turn  ); offset_y += extents.height + 5;
	paint_text( context, label_offset_x, offset_y, "White" ); paint_text( context, info_offset_x, offset_y, the_info.white ); offset_y += extents.height + 5;
	paint_text( context, label_offset_x, offset_y, "Black" ); paint_text( context, info_offset_x, offset_y, the_info.black ); offset_y += extents.height + 5;
	paint_text( context, label_offset_x, offset_y, "Time"  ); paint_text( context, info_offset_x, offset_y, the_info.time  ); offset_y += extents.height + 5;
	paint_text( context, label_offset_x, offset_y, "Level" ); paint_text( context, info_offset_x, offset_y, the_info.level ); offset_y += extents.height + 5;
	paint_text( context, label_offset_x, offset_y, "Value" ); paint_text( context, info_offset_x, offset_y, the_info.value ); offset_y += extents.height + 5;
	paint_text( context, label_offset_x, offset_y, "Nodes" ); paint_text( context, info_offset_x, offset_y, the_info.nodes ); offset_y += extents.height + 5;
	paint_text( context, label_offset_x, offset_y, "N/Sec" ); paint_text( context, info_offset_x, offset_y, the_info.n_sec ); offset_y += extents.height + 5;
	paint_text( context, label_offset_x, offset_y, "Depth" ); paint_text( context, info_offset_x, offset_y, the_info.depth ); offset_y += extents.height + 5;
	if( show_bestline ) {
		paint_text( context, label_offset_x, offset_y, "Bestline" ); paint_text( context, info_offset_x, offset_y, the_info.bestline );
	}
}

/**-----------------------------------------------------------------------------
 * \brief
 *
 * \param FEN_string string
 * \return void
 *
 */
void ChessBoard::set_piece_positions( std::map<STSquare,STPiece> new_pieces )
{
	pieces = new_pieces;

	paint_pieces();

    queue_draw();
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
void ChessBoard::set_colours( ColourChooser::STColours new_colours, STInfo info )
{
    background_colour = Gdk::RGBA(new_colours.bg);
	foreground_colour = Gdk::RGBA(new_colours.fg);
	white_colour =  Gdk::RGBA(new_colours.white);
	black_colour =  Gdk::RGBA(new_colours.black);

	paint_board();
	paint_info( info );

	queue_draw();
}

/**-----------------------------------------------------------------------------
 * \brief Turn the board up side down
 *
 * \return bool
 *
 */
void ChessBoard::toggle_reverse()
{
    is_reversed = !is_reversed;

	paint_pieces();

    queue_draw();
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

	queue_draw();
}

/**-----------------------------------------------------------------------------
 * \brief
 *
 */
void ChessBoard::set_info( STInfo info )
{
    paint_info( info );

    queue_draw();
}

/**-----------------------------------------------------------------------------
 * \brief
 *
 */
void ChessBoard::toggle_bestline( STInfo info )
{
    show_bestline = !show_bestline;

    paint_info( info );

    queue_draw();
}

/**-----------------------------------------------------------------------------
 * \brief
 *
 */
void ChessBoard::animate_move_start()
{
	Gdk::Point end_point = square_to_point( drag_end_square );
	floating_piece_position = square_to_point( drag_start_square );

	annimate_delta.set_x( (end_point.get_x() - floating_piece_position.get_x()) / 10 );
	annimate_delta.set_y( (end_point.get_y() - floating_piece_position.get_y()) / 10 );

	floating_piece_code = save_piece_code;
	is_animating = true;

	// make the first step
	floating_piece_position.set_x( floating_piece_position.get_x() + annimate_delta.get_x() );
	floating_piece_position.set_y( floating_piece_position.get_y() + annimate_delta.get_y() );
    floating_piece_source = source_point( floating_piece_position, floating_piece_code );

	queue_draw();
}

void ChessBoard::animate_move_continue()
{
	floating_piece_position.set_x( floating_piece_position.get_x() + annimate_delta.get_x() );
	floating_piece_position.set_y( floating_piece_position.get_y() + annimate_delta.get_y() );
    floating_piece_source = source_point( floating_piece_position, floating_piece_code );

	queue_draw();
}

void ChessBoard::animate_move_finish()
{
	floating_piece_code = ' ';
	is_animating = false;

	timeout_counter = 0;

	queue_draw();
}


/**-----------------------------------------------------------------------------
 * \brief
 */
bool ChessBoard::on_highlight_timeout()
{
	static bool highlight_on = false;

	if( ! --timeout_counter ) {
		highlight_on = false;
		draw_highlight = highlight_on;
		queue_draw();
		return false;
	}

	highlight_on = !highlight_on;
	draw_highlight = highlight_on;
	queue_draw();
	return true;
}

/**-----------------------------------------------------------------------------
 * \brief
 *
 */
void ChessBoard::highlight( STSquare square )
{
	highlight_pos = square_to_point( square );
	draw_highlight = true;

	queue_draw();

	timeout_counter = 10;
	Glib::signal_timeout().connect( sigc::mem_fun(*this, &ChessBoard::on_highlight_timeout), 100 );
}
