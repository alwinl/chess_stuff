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

namespace chess_gui {

constexpr int SQUARE_SIZE = 36;
constexpr int INFO_WIDTH = 240;
constexpr int MAX_HEIGHT = 1024;

Point operator-( const Point& lhs, const Point& rhs )
{
	auto [lhs_x, lhs_y] = lhs;
	auto [rhs_x, rhs_y] = rhs;

	return Point( lhs_x - rhs_x, lhs_y - rhs_y );
}

Point operator+( const Point& lhs, const Point& rhs )
{
	auto [lhs_x, lhs_y] = lhs;
	auto [rhs_x, rhs_y] = rhs;

	return Point( lhs_x + rhs_x, lhs_y + rhs_y );
}

Point operator/( const Point& lhs, int denominator )
{
	auto [lhs_x, lhs_y] = lhs;
	return Point( lhs_x / denominator, lhs_y / denominator );
}

Point operator*( const Point& lhs, int multiplier )
{
	auto [lhs_x, lhs_y] = lhs;
	return Point( lhs_x * multiplier, lhs_y * multiplier );
}

ChessBoard::ChessBoard( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model ) : Gtk::DrawingArea(cobject)
{
	set_draw_func( sigc::mem_fun( *this, &ChessBoard::on_draw) );

	background_image = Cairo::ImageSurface::create( Cairo::Surface::Format::RGB24, 8*SQUARE_SIZE + 2, 8*SQUARE_SIZE + 2 );	// one pixel border
	pieces_image = Cairo::ImageSurface::create( Cairo::Surface::Format::ARGB32, 6*SQUARE_SIZE + 2, 2*SQUARE_SIZE + 2 );
	info_image = Cairo::ImageSurface::create( Cairo::Surface::Format::ARGB32, INFO_WIDTH, MAX_HEIGHT );
	pieces_overlay = Cairo::ImageSurface::create( Cairo::Surface::Format::ARGB32, 8*SQUARE_SIZE, 8*SQUARE_SIZE );

    paint_edit_pieces();
}

void ChessBoard::on_draw( const Cairo::RefPtr<Cairo::Context>& cr, int width, int height )
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

	cr->set_source_rgb( background_colour.get_red(), background_colour.get_green(), background_colour.get_blue() );
 	cr->paint();

	cr->rectangle( board_outline.get_x(), board_outline.get_y(), board_outline.get_width(), board_outline.get_height() );
	cr->set_source( background_image, board_outline.get_x(), board_outline.get_y());
	cr->fill();

	cr->rectangle( board_outline.get_x() + 1, board_outline.get_y() + 1, board_outline.get_width() - 2, board_outline.get_height() - 2 );	// deduct the border
	cr->set_source( pieces_overlay, board_outline.get_x(), board_outline.get_y() );
	cr->fill();

    // Now draw either the info window or the edit window
    if( is_edit ) {
		cr->rectangle( edit_outline.get_x(), edit_outline.get_y(), edit_outline.get_width(), edit_outline.get_height() );
		cr->set_source( pieces_image, edit_outline.get_x(), edit_outline.get_y());
		cr->fill();
    } else {
		cr->rectangle( info_outline.get_x(), info_outline.get_y(), info_outline.get_width(), info_outline.get_height() );
		cr->set_source( info_image, info_outline.get_x(), info_outline.get_y());
		cr->fill();
    }

	if( draw_floating_piece ) {
		Point floating_piece_source = floating_piece_position - piececode_to_editpoint( floating_piece_code );
		cr->rectangle( floating_piece_position.first, floating_piece_position.second, SQUARE_SIZE, SQUARE_SIZE );
		cr->set_source( pieces_image, floating_piece_source.first, floating_piece_source.second );
		cr->fill();
    }

	if( draw_highlight ) {
		auto [hl_x, hl_y] = highlight_pos;

		cr->rectangle( hl_x, hl_y, SQUARE_SIZE, SQUARE_SIZE );
		cr->set_source_rgb(  foreground_colour.get_red(), foreground_colour.get_green(), foreground_colour.get_blue() );
		cr->set_line_width( 4.0 );
		cr->stroke();
	}
}

/**-----------------------------------------------------------------------------
 * The next set of functions create the images the draw function uses
 */
void ChessBoard::paint_board( )
{
	Cairo::RefPtr<Cairo::Context> context = Cairo::Context::create( background_image );

	context->rectangle( 0, 0, board_outline.get_width(), board_outline.get_height() );

	context->set_source_rgb( black_colour.get_red(), black_colour.get_green(), black_colour.get_blue() );
	context->fill_preserve();

	context->set_source_rgb( 0, 0, 0);
	context->stroke();

	/* Paint each alternate square in the white colour */
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
	context->set_operator( Cairo::Context::Operator::CLEAR );
	context->paint();
	context->restore();

	for( int index = 0; index < 64; ++index ) {

		if( pieces[index] == ' ' )
			continue;

		Point dest = chesssquare_to_boardpoint( index ) - Point( board_outline.get_x(), board_outline.get_y() );
		Point source = dest - piececode_to_editpoint( pieces[index] );

		context->set_source( pieces_image, source.first, source.second );
		context->rectangle( dest.first, dest.second, SQUARE_SIZE, SQUARE_SIZE );
		context->fill();
	}
}

void ChessBoard::paint_edit_pieces()
{
	Cairo::RefPtr<Cairo::Context> context = Cairo::Context::create( pieces_image );

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
	context->set_operator( Cairo::Context::Operator::CLEAR );
	context->paint();
	context->restore();

 	// Paint a frame
    int height = (show_bestline ? 12 : 11 ) * (extents.height + 5);

    context->set_source_rgb(  foreground_colour.get_red(), foreground_colour.get_green(), foreground_colour.get_blue() );
	context->rectangle( 5, 10, INFO_WIDTH - 10, height );
	context->stroke();

	// Blank the frame portion behind the word "Information"
	context->save();
	context->set_operator( Cairo::Context::Operator::CLEAR );
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
 * Animations
 */
void ChessBoard::drag_start( double mouse_x, double mouse_y, char piece_code )
{
	floating_piece_position = Point(mouse_x, mouse_y) - Point(.5 * SQUARE_SIZE, .5 * SQUARE_SIZE);
	floating_piece_code = piece_code;
	draw_floating_piece = true;

	queue_draw();
}

void ChessBoard::drag_continue( double mouse_x, double mouse_y )
{
	floating_piece_position = Point(mouse_x, mouse_y) - Point(.5 * SQUARE_SIZE, .5 * SQUARE_SIZE);

    queue_draw();
}

void ChessBoard::drag_finish()
{
	draw_floating_piece = false;

	queue_draw();
}

void ChessBoard::animate_start( uint16_t start_square, uint16_t end_square, char piece )
{
	floating_piece_position = chesssquare_to_boardpoint( start_square );
	floating_piece_code = piece;
	draw_floating_piece = true;

	annimate_delta = (chesssquare_to_boardpoint( end_square ) - floating_piece_position) / 10;

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

	queue_draw();
}

void ChessBoard::highlight_start( uint16_t square )
{
	highlight_pos = chesssquare_to_boardpoint( square );
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

#define REVERSE_RANK_MASK 0b00111000

uint16_t ChessBoard::boardpoint_to_chesssquare( Point point )
{
	if( ! board_outline.intersects( {(int)point.first, (int)point.second, 1, 1} ) )
		return (uint16_t)-1;

	point = ( point - Point( board_outline.get_x(), board_outline.get_y() ) );
	point = point / SQUARE_SIZE;

	uint16_t square = (int)point.first + 8 * (7 - (int)(point.second));

	if( is_reversed )
		square ^= REVERSE_RANK_MASK;

	return square;
}

Point ChessBoard::chesssquare_to_boardpoint( uint16_t square )
{
	if( is_reversed )
		square ^= REVERSE_RANK_MASK;

	Point point( (square % 8), (7 - (square / 8)) );

	point = point * SQUARE_SIZE;
	point = point + Point( board_outline.get_x(), board_outline.get_y() );

	return point;
}

char ChessBoard::editpoint_to_piececode( Point point )
{
	static std::string piece_chars = "KQRBNPkqrbnp";

	if( !is_edit || !edit_outline.intersects( {(int)point.first, (int)point.second, 1, 1} ) )
		return ' ';

	point = point - Point( edit_outline.get_x(), edit_outline.get_y() );
	point = point - Point( 1, 1 );

	point = point / SQUARE_SIZE;

	return piece_chars[ (int)point.first + 6 * (int)point.second ];
}

//The bitmap is organised as:
//         KQRBNP
//         kqrbnp
Point ChessBoard::piececode_to_editpoint( char piece_code )
{
	static std::string piece_chars = "KQRBNPkqrbnp";

	unsigned int index = piece_chars.find( piece_code );

	Point point {(index % 6), (index / 6)};

	point = point * SQUARE_SIZE;
	point = point + Point( 1, 1 );

	return point;
}

}