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
#include <vector>
#include <utility>

#include <gtkmm.h>

#include "../logicsrc/pods.h"

class ChessController;

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

public:
	ChessBoard( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model, ChessController& app );

	void set_edit( bool on );
    void set_colours( Gdk::RGBA bg, Gdk::RGBA white, Gdk::RGBA black, Gdk::RGBA fg );
    void set_piece_positions( std::string FEN_string );
	void set_info( STInfo& info );
    void toggle_reverse();
    void toggle_bestline();
	void animate_start( STSquare start_square, STSquare end_square, char piece );
	void animate_step( );
	void animate_stop();
	void highlight_start( STSquare square );
	void highlight_flash( bool on );

private:
	virtual bool on_configure_event( GdkEventConfigure* event );
	virtual bool on_draw( const Cairo::RefPtr<Cairo::Context>& cr );
	virtual bool on_button_press_event(GdkEventButton* button_event);
    virtual bool on_button_release_event( GdkEventButton* release_event );
    virtual bool on_motion_notify_event( GdkEventMotion* motion_event );

	void update();

	STSquare calc_square_from_point( Gdk::Point point );
	char calc_piece_from_point( Gdk::Point point );
	char calc_piece_from_square( STSquare square );

	bool draw_board( const Cairo::RefPtr<Cairo::Context>& cr );
	bool draw_pieces( const Cairo::RefPtr<Cairo::Context>& cr );
	bool draw_info( const Cairo::RefPtr<Cairo::Context>& cr );
	bool draw_edit( const Cairo::RefPtr<Cairo::Context>& cr );
	bool draw_floating_piece( const Cairo::RefPtr<Cairo::Context>& cr );
	bool draw_square_highlight( const Cairo::RefPtr<Cairo::Context>& cr );

	Cairo::RefPtr<Cairo::ImageSurface> background_image_;
	Cairo::RefPtr<Cairo::ImageSurface> pieces_surface_;
	Gdk::Rectangle board_outline;
	Gdk::Rectangle info_outline;
	Gdk::Rectangle edit_outline;
	Gdk::RGBA background_colour;
	Gdk::RGBA white_colour;
	Gdk::RGBA black_colour;
	Gdk::RGBA foreground_colour;

	Gdk::Point floating_piece_position;
	char floating_piece_code;
	STSquare drag_start_square;
	bool is_dragging;
	bool is_animating;
	Gdk::Point annimate_delta;

	Gdk::Point highlight_pos;
	bool draw_highlight;

	std::map< char, Gdk::Point > source_offsets;
	bool reversed;
	bool is_edit;
	bool show_bestline_info;
	std::vector< std::pair<std::string,std::string> > info_data;
	std::map<STSquare,char> pieces;

	ChessController& controller;
};

#endif // CHESSBOARD_H_INCLUDED
