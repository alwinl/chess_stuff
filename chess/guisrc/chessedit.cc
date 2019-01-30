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

#include "chessedit.h"
#include "chessapplication.h"

ChessEdit::ChessEdit( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model, ChessApplication& app )
                        : Gtk::DrawingArea(cobject), controller(app), drag_code(' ')
{
	using namespace Cairo;

	// create our image objects
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

    piece_chars = "KQRBNPkqrbnp";
}

bool ChessEdit::on_configure_event( GdkEventConfigure* event )
{
	Gtk::Allocation allocation = get_allocation();

	outline_in_pixels = Gdk::Rectangle(
            allocation.get_width()/2 - 3 * SQUARE_SIZE, allocation.get_height()/2 - 1 * SQUARE_SIZE,
            6 * SQUARE_SIZE, 2 * SQUARE_SIZE
    );

    return true;
}

bool ChessEdit::on_draw( const Cairo::RefPtr<Cairo::Context>& cr )
{
	// Draw background
	cr->set_source_rgb( background_colour.get_red(), background_colour.get_green(), background_colour.get_blue() );
 	cr->paint();

	// Draw border and board
	cr->rectangle( outline_in_pixels.get_x(), outline_in_pixels.get_y(), outline_in_pixels.get_width(), outline_in_pixels.get_height() );
	cr->set_source_rgb( 0, 0, 0);								// border
	cr->stroke_preserve();
	cr->set_source( pieces_surface_, outline_in_pixels.get_x(), outline_in_pixels.get_y());	// board
	cr->fill();

	// Draw the drag piece
    if( drag_code != ' ' ) {
        Gdk::Rectangle dest = Gdk::Rectangle( drag_point.get_x(), drag_point.get_y(), SQUARE_SIZE, SQUARE_SIZE );
        Gdk::Point source = Gdk::Point( dest.get_x() - source_offsets[drag_code].get_x(), dest.get_y() - source_offsets[drag_code].get_y() );

        cr->set_source( pieces_surface_, source.get_x(), source.get_y() );
        cr->rectangle( dest.get_x(), dest.get_y(), dest.get_width(), dest.get_height() );
        cr->fill();
    }

    return true;
}

bool ChessEdit::on_button_press_event( GdkEventButton* button_event )
{
    // Do some sanity testing first. Make sure the click is in the outline
    if( ! outline_in_pixels.intersects( Gdk::Rectangle( button_event->x, button_event->y, 1, 1 ) ) )
        return true;

    // to find out which piece we got, we need to calculate the button_event point to piece grid square
    int row = (button_event->y - outline_in_pixels.get_y()) / SQUARE_SIZE;
    int col = (button_event->x - outline_in_pixels.get_x()) / SQUARE_SIZE;

    drag_code = piece_chars[row*6 + col];
    drag_point = Gdk::Point(button_event->x - .5 * SQUARE_SIZE, button_event->y - .5 * SQUARE_SIZE);

    controller.set_drag_piece( drag_code );

    update();
    return true;
}

bool ChessEdit::on_button_release_event( GdkEventButton* release_event )
{
    controller.set_drag_piece( ' ' );
    drag_code = ' ';
    return true;
}

bool ChessEdit::on_motion_notify_event( GdkEventMotion* motion_event )
{
    drag_point = Gdk::Point(motion_event->x - .5 * SQUARE_SIZE, motion_event->y - .5 * SQUARE_SIZE);
    update();

    return true;
}


/**-----------------------------------------------------------------------------
 * \brief Invalidating the window causes the on_draw function to be called
 *
 * \return void
 *
 */
void ChessEdit::update()
{
    Glib::RefPtr<Gdk::Window> win = get_window();
    if( !win )      // window hqs not been realised yet
        return;

    win->invalidate( true);
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
void ChessEdit::set_colours( Gdk::RGBA bg )
{
    background_colour = bg;

	update();   // Redraw the chessboard
}




