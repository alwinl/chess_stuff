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

#ifndef CHESSBOARD_H_INCLUDED
#define CHESSBOARD_H_INCLUDED

#include <map>
#include <string>

#pragma GCC diagnostic ignored "-Wparentheses"
#include <gtkmm.h>
#pragma GCC diagnostic warning "-Wparentheses"

#include "pods.h"

class ChessApplication;


/**-----------------------------------------------------------------------------
 * \brief Chess board area
 *
 * This class encapsulates the drawing of a complete chessboard with background
 * on a DrawingArea widget.
 */
class ChessBoard : public Gtk::DrawingArea
{
private:
	static const int SQUARE_SIZE = 36;

public:
	ChessBoard( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model, ChessApplication& app );

    void set_colours( Gdk::RGBA bg, Gdk::RGBA white, Gdk::RGBA black );
    void set_piece_positions( std::string FEN_string );
    void set_drag_piece( char piece ) { drag_code = piece; update(); };
    bool toggle_reverse();

private:
	virtual bool on_configure_event( GdkEventConfigure* event );
	virtual bool on_draw( const Cairo::RefPtr<Cairo::Context>& cr );
	virtual bool on_button_press_event(GdkEventButton* button_event);
    virtual bool on_button_release_event( GdkEventButton* release_event );
    virtual bool on_motion_notify_event( GdkEventMotion* motion_event );

	void update();
	STSquare calc_square_from_point( Gdk::Point point );

	ChessApplication& controller;
	Gdk::Rectangle outline_in_pixels;
	Cairo::RefPtr<Cairo::ImageSurface> background_image_;
	Cairo::RefPtr<Cairo::ImageSurface> pieces_surface_;
	std::map< char, Gdk::Point > source_offsets;
	Gdk::RGBA background_colour;
	Gdk::RGBA white_colour;
	Gdk::RGBA black_colour;
    std::string piece_positions;
	bool reversed;
	char drag_code;
	Gdk::Point drag_point;
};

#endif // CHESSBOARD_H_INCLUDED
