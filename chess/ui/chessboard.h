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

#pragma once

#include <gtkmm.h>

#include <string>
#include <array>
#include <utility>

namespace chess_gui {

using Point = std::pair<float,float>;

class ChessBoard : public Gtk::DrawingArea
{
public:
	ChessBoard( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model );

    void set_piece_positions( std::array<char,64> new_pieces );
	void set_info( std::array<std::pair<std::string,std::string>,10> info );
	void set_colours( std::array<std::string,4> new_colours );
	void set_edit( bool on );
    void toggle_reverse();
    void toggle_bestline();

	void drag_start( double mouse_x, double mouse_y, char piece_code );
	void drag_continue( double mouse_x, double mouse_y );
	void drag_finish();

	void animate_start( uint16_t start_square, uint16_t end_square, char piece );
	void animate_continue();
	void animate_finish();

	void highlight_start( uint16_t square );
	void highlight_continue();
	void highlight_finish();

	// bool point_in_chessboard( Point point );
	Point chesssquare_to_boardpoint( uint16_t square );
	uint16_t boardpoint_to_chesssquare( Point point );

	// bool point_in_editwindow( Point point );
	char editpoint_to_piececode( Point point );
	Point piececode_to_editpoint( char piece_code );

private:
	void on_draw( const Cairo::RefPtr<Cairo::Context>& cr, int width, int height );

	void paint_board();
	void paint_pieces();
	void paint_edit_pieces();
	void paint_info();

	Cairo::RefPtr<Cairo::ImageSurface> background_image;
	Cairo::RefPtr<Cairo::ImageSurface> pieces_image;
	Cairo::RefPtr<Cairo::ImageSurface> info_image;
	Cairo::RefPtr<Cairo::ImageSurface> pieces_overlay;

	Gdk::Rectangle board_outline;
	Gdk::Rectangle info_outline;
	Gdk::Rectangle edit_outline;

	Gdk::RGBA background_colour;
	Gdk::RGBA foreground_colour;
	Gdk::RGBA white_colour;
	Gdk::RGBA black_colour;

	bool show_bestline = true;
	bool draw_highlight = false;
	bool draw_floating_piece = false;
	bool is_reversed = false;
	bool is_edit = false;

	std::array<char,64> pieces;
	std::array<std::pair<std::string,std::string>,10> info;

	Point floating_piece_position;
	char floating_piece_code;
	Point annimate_delta;
	Point highlight_pos;
};

}
