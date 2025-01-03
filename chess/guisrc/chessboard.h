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

#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <gtkmm.h>

#include <string>
#include <array>
#include <utility>

namespace chess_gui {

using Point = std::pair<float,float>;

/**-----------------------------------------------------------------------------
 * \brief Chess board area
 *
 * This class encapsulates the drawing of a complete chessboard, info area and edit bitmap
 * with background on a DrawingArea widget.
 */
class ChessBoard : public Gtk::DrawingArea
{
private:
	static const int SQUARE_SIZE = 36;
	static const int INFO_WIDTH = 240;
	static const int MAX_HEIGHT = 1024;

public:
	ChessBoard( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model );

    void set_piece_positions( std::array<char,64> new_pieces );
	void set_info( std::array<std::pair<std::string,std::string>,10> info );
	void set_colours( std::array<std::string,4> new_colours );
	void set_edit( bool on );
    void toggle_reverse();
    void toggle_bestline();

	void highlight_start( uint16_t square );
	void highlight_continue();
	void highlight_finish();

	void animate_start( uint16_t start_square, uint16_t end_square, char piece );
	void animate_continue();
	void animate_finish();

	void computer_is_thinking( bool on ) { is_computer_move = on; }

private:
	// virtual bool on_configure_event( GdkEventConfigure* event );
	void on_draw( const Cairo::RefPtr<Cairo::Context>& cr, int width, int height );
	// virtual bool on_button_press_event( GdkEventButton* button_event );
    // virtual bool on_button_release_event( GdkEventButton* release_event );
    // virtual bool on_motion_notify_event( GdkEventMotion* motion_event );

	void paint_board();
	void paint_pieces();
	void paint_edit_pieces();
	void paint_info();

	void start_dragging( Point start_point, char piece_code );
	void update_dragging( Point new_point );
	void stop_dragging();

	Point square_to_point( uint16_t square );
	uint16_t point_to_square( Point point );
	Point piececode_to_editpoint( char piece_code );
	char editpoint_to_piececode( Point point );

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
	bool is_dragging = false;
	bool is_animating = false;
	bool is_computer_move = false;
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

#endif // CHESSBOARD_H
