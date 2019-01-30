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
#include "chessapplication.h"

/**-----------------------------------------------------------------------------
 * \brief ChessBoard constructor
 *
 * \param cobject BaseObjectType*
 * \param ui_model const Glib::RefPtr<Gtk::Builder>&
 * \param model_init AppModel&
 *
 */
ChessBoard::ChessBoard(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model, ChessApplication& app )
                            : Gtk::DrawingArea(cobject), reversed(false), drag_code(' '), controller(app)
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
}

/**-----------------------------------------------------------------------------
 * \brief Called when screen size changes
 *
 * Recalculate our screen real estate. Can be used as a trigger to
 * recalculate other drawing parameters.
 *
 * \param GdkEventConfigure*event
 * \return bool
 *
 */
bool ChessBoard::on_configure_event( GdkEventConfigure*event )
{
	Gtk::Allocation allocation = get_allocation();

	outline_in_pixels = Gdk::Rectangle(
            allocation.get_width()/2 - 4 * SQUARE_SIZE,
            allocation.get_height()/2 - 4 * SQUARE_SIZE,
            8 * SQUARE_SIZE,
            8 * SQUARE_SIZE
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
	// Draw background
	cr->set_source_rgb( background_colour.get_red(), background_colour.get_green(), background_colour.get_blue() );
 	cr->paint();

	// Draw border and board
	cr->rectangle( outline_in_pixels.get_x(), outline_in_pixels.get_y(), outline_in_pixels.get_width(), outline_in_pixels.get_height() );
	cr->set_source_rgb( 0, 0, 0);								// border
	cr->stroke_preserve();
	cr->set_source( background_image_, outline_in_pixels.get_x(), outline_in_pixels.get_y());	// board
	cr->fill();

	// Draw the pieces
	if( piece_positions.empty() )
        return false;

    STSquare square = make_square(0,7);         // first = file, second is rank

    for( char& code : piece_positions ) {

        if( (code > '0') && (code < '9') ) {       // Character can either be a number (squares to skip)
           square.file += (code - '0' );
        } else if( code == '/' ) {                      // ... or an end of rank indicator
            --square.rank;
            square.file = 0;
        } else {                                    // or an piece designator
            // calculate the destination pixel position by taking the left top of the board and
            // adding (converted) rank and file positions. Note y axis is reversed.
            // If the board is reversed, the x-axis is reversed, not the y-axis
            Gdk::Rectangle dest = Gdk::Rectangle( outline_in_pixels.get_x(), outline_in_pixels.get_y(), SQUARE_SIZE, SQUARE_SIZE );

            if( reversed ) {
                dest.set_x( dest.get_x() + (7 - square.file)  * SQUARE_SIZE );
                dest.set_y( dest.get_y() +      square.rank   * SQUARE_SIZE );
            } else {
                dest.set_x( dest.get_x() +      square.file   * SQUARE_SIZE );
                dest.set_y( dest.get_y() + (7 - square.rank)  * SQUARE_SIZE );
            }

            // To paint a particular piece we have to offset our source bitmap so that the correct bitmap
            // overlays our square to be painted. The pieces are stored as two rows (a white and a black row)
            // in six columns, calculable by type.
            Gdk::Point source = Gdk::Point( dest.get_x() - source_offsets[code].get_x(), dest.get_y() - source_offsets[code].get_y() );

            cr->set_source( pieces_surface_, source.get_x(), source.get_y() );
            cr->rectangle( dest.get_x(), dest.get_y(), dest.get_width(), dest.get_height() );
            cr->fill();

            ++square.file;
        }
    }

	// Draw the drag piece
    if( drag_code != ' ' ) {
        Gdk::Rectangle dest = Gdk::Rectangle( drag_point.get_x(), drag_point.get_y(), SQUARE_SIZE, SQUARE_SIZE );
        Gdk::Point source = Gdk::Point( dest.get_x() - source_offsets[drag_code].get_x(), dest.get_y() - source_offsets[drag_code].get_y() );

        cr->set_source( pieces_surface_, source.get_x(), source.get_y() );
        cr->rectangle( dest.get_x(), dest.get_y(), dest.get_width(), dest.get_height() );
        cr->fill();
    }

	return false;

}

/**-----------------------------------------------------------------------------
 * \brief
 *
 * \return pair<int,int>
 *
 */
STSquare ChessBoard::calc_square_from_point( Gdk::Point point )
{
    return make_square(
        (point.get_x() - outline_in_pixels.get_x()) / SQUARE_SIZE,
        (point.get_y() - outline_in_pixels.get_y()) / SQUARE_SIZE
    );
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
    bool intersecting = outline_in_pixels.intersects( mouse_pos );

    if( !intersecting )
        return true;

    drag_point = Gdk::Point(button_event->x - .5 * SQUARE_SIZE, button_event->y - .5 * SQUARE_SIZE);
    controller.drag_start( calc_square_from_point( Gdk::Point(button_event->x, button_event->y) ) );

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
    bool intersecting = outline_in_pixels.intersects( mouse_pos );

    if( !intersecting ) {
        controller.drag_cancelled();
        return true;
    }

    controller.drag_end( calc_square_from_point( Gdk::Point(release_event->x, release_event->y) ) );

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
 * \param FEN_string string
 * \return void
 *
 */
void ChessBoard::set_piece_positions( std::string FEN_string )
{
    piece_positions = FEN_string;
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
void ChessBoard::set_colours( Gdk::RGBA bg, Gdk::RGBA white, Gdk::RGBA black )
{
	using namespace Cairo;

    background_colour = bg;
    white_colour = white;
    black_colour = black;

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
bool ChessBoard::toggle_reverse()
{
    reversed = !reversed;

    update();

    return reversed;
}


