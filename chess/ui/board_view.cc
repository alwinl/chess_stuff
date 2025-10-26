/*
 * new_board.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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
	static std::array<std::string, 10> labels =
		{ "Turn", "White", "Black", "Time", "Level", "Value", "Nodes", "N/Sec", "Depth", "Bestline" };

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

	int label_idx = 0;

	for( auto it = std_info.begin(); it != end_it; ++it ) {

		cr->move_to( label_offset_x, offset_y );
		cr->show_text( labels[label_idx] );
		cr->move_to( info_offset_x, offset_y );
		cr->show_text( *it );

		offset_y += extents.height + 5;
		++label_idx;
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
