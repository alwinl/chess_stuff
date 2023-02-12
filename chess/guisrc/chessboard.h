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
	void set_colours( std::array<std::string,4> new_colours, STInfo info );
	void set_edit( bool on );
    void toggle_reverse( );
    void toggle_bestline( STInfo the_info );

	void highlight( STSquare square );

	void animate_move_start();
	void animate_move_continue();
	void animate_move_finish();

	STSquare get_drag_square() const { return drag_start_square; }
	STSquare get_end_square() const { return drag_end_square; }
	char get_piece_code() const { return save_piece_code; }

private:
	virtual bool on_configure_event( GdkEventConfigure* event );
	virtual bool on_draw( const Cairo::RefPtr<Cairo::Context>& cr );
	virtual bool on_button_press_event( GdkEventButton* button_event );
    virtual bool on_button_release_event( GdkEventButton* release_event );
    virtual bool on_motion_notify_event( GdkEventMotion* motion_event );

	void paint_board();
	void paint_pieces();
	void paint_edit_pieces();
	void paint_text( Cairo::RefPtr<Cairo::Context>& context, double x, double y, std::string text );
	void paint_info( STInfo& info );

	STSquare adjust_for_reverse( STSquare square );

	STSquare point_to_square( Gdk::Point point );
	Gdk::Point square_to_point( STSquare square );
	Gdk::Point square_to_board_point( STSquare square );

	char point_to_edit_piece( Gdk::Point point );
	Gdk::Point source_point( Gdk::Point point, char piece_code );

	bool on_highlight_timeout();

	void start_dragging( char piece, Gdk::Point start_point );
	void update_dragging( Gdk::Point new_point );
	void stop_dragging();

	Cairo::RefPtr<Cairo::ImageSurface> background_image;
	Cairo::RefPtr<Cairo::ImageSurface> pieces_image;
	Cairo::RefPtr<Cairo::ImageSurface> info_image;
	Cairo::RefPtr<Cairo::ImageSurface> pieces_overlay;

    std::map<char,Gdk::Point> pieces_image_offsets;

	Gdk::Rectangle board_outline;
	Gdk::Rectangle info_outline;
	Gdk::Rectangle edit_outline;

	Gdk::RGBA background_colour;
	Gdk::RGBA foreground_colour;
	Gdk::RGBA white_colour;
	Gdk::RGBA black_colour;

	bool show_bestline;
	bool draw_highlight;
	bool is_dragging;
	bool is_animating;
	bool is_reversed;
	bool is_edit;

	int timeout_counter;

	std::map<STSquare,STPiece> pieces;
	Gdk::Point floating_piece_position;
	Gdk::Point floating_piece_source;
	Gdk::Point annimate_delta;
	Gdk::Point highlight_pos;
	char floating_piece_code;
	char save_piece_code;
	STSquare drag_start_square;
	STSquare drag_end_square;
};

#endif // CHESSBOARD_H
