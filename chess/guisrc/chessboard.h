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

#include <map>
#include <string>
#include <array>

#include "pods.h"

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
	static const int INFO_WIDTH = 226; /* 226 pixels is 6 * 36 pixels + 10 pixels for spacing/border */
	static const int MAX_HEIGHT = 1024;

public:
	ChessBoard( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model );

    void set_piece_positions( std::map<STSquare,STPiece> new_pieces );
	void set_info( STInfo info );
	void set_colours( std::array<std::string,4> new_colours );
	void set_edit( bool on );
    void toggle_reverse();
    void toggle_bestline();

	void highlight_start( STSquare square );
	void highlight_continue();
	void highlight_finish();

	void animate_start( STSquare& start_square, STSquare& end_square, char piece );
	void animate_continue();
	void animate_finish();

	void computer_is_thinking( bool on ) { is_computer_move = on; }

private:
	virtual bool on_configure_event( GdkEventConfigure* event );
	virtual bool on_draw( const Cairo::RefPtr<Cairo::Context>& cr );
	virtual bool on_button_press_event( GdkEventButton* button_event );
    virtual bool on_button_release_event( GdkEventButton* release_event );
    virtual bool on_motion_notify_event( GdkEventMotion* motion_event );

	void paint_board();
	void paint_pieces();
	void paint_edit_pieces();
	void paint_info();

	void start_dragging( Gdk::Point start_point, char piece_code );
	void update_dragging( Gdk::Point new_point );
	void stop_dragging();

	STSquare   point_to_square( Gdk::Point point );
	STSquare   adjust_for_reverse( STSquare square );
	Gdk::Point square_to_point( STSquare square );
	Gdk::Point square_to_board_point( STSquare square );
	Gdk::Point piece_source_point( Gdk::Point point, char piece_code );
	char       point_to_edit_piece( Gdk::Point point );

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

	std::map<STSquare,STPiece> pieces;
	std::array<std::pair<std::string,std::string>,10> info;

	Gdk::Point floating_piece_position;
	char floating_piece_code;
	Gdk::Point annimate_delta;
	Gdk::Point highlight_pos;
};

#endif // CHESSBOARD_H
