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

class ChessApplication;
class STSquare;


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

	bool set_edit( bool on );

    void set_colours( Gdk::RGBA bg, Gdk::RGBA white, Gdk::RGBA black, Gdk::RGBA fg );
    void set_piece_positions( std::string FEN_string );
    void set_drag_piece( char piece ) { drag_code = piece; update(); };
    bool toggle_reverse();

    void set_turn_text( std::string text ) { set_text( 0, text); };
    void set_white_text( std::string text ) { set_text( 1, text); };
    void set_black_text( std::string text ) { set_text( 2, text); };
    void set_time_text( std::string text ) { set_text( 3, text); };
    void set_level_text( std::string text ) { set_text( 4, text); };
    void set_value_text( std::string text ) { set_text( 5, text); };
    void set_nodes_text( std::string text ) { set_text( 6, text); };
    void set_n_sec_text( std::string text ) { set_text( 7, text); };
    void set_depth_text( std::string text ) { set_text( 8, text); };
    void set_bestline_text( std::string text ) { set_text( 9, text); };
    bool toggle_bestline();


private:
	virtual bool on_configure_event( GdkEventConfigure* event );
	virtual bool on_draw( const Cairo::RefPtr<Cairo::Context>& cr );
	virtual bool on_button_press_event(GdkEventButton* button_event);
    virtual bool on_button_release_event( GdkEventButton* release_event );
    virtual bool on_motion_notify_event( GdkEventMotion* motion_event );

	void update();
	void set_text( int index, std::string text );
	STSquare calc_square_from_point( Gdk::Point point );

	bool draw_board( const Cairo::RefPtr<Cairo::Context>& cr );
	bool draw_pieces( const Cairo::RefPtr<Cairo::Context>& cr );
	bool draw_info( const Cairo::RefPtr<Cairo::Context>& cr );
	bool draw_edit( const Cairo::RefPtr<Cairo::Context>& cr );
	bool draw_drag_piece( const Cairo::RefPtr<Cairo::Context>& cr );

	Cairo::RefPtr<Cairo::ImageSurface> background_image_;
	Cairo::RefPtr<Cairo::ImageSurface> pieces_surface_;
	Gdk::Rectangle board_outline;
	Gdk::Rectangle info_outline;
	Gdk::Rectangle edit_outline;
	Gdk::RGBA background_colour;
	Gdk::RGBA white_colour;
	Gdk::RGBA black_colour;
	Gdk::RGBA foreground_colour;
	Gdk::Point drag_point;
	std::map< char, Gdk::Point > source_offsets;
    std::string piece_positions;
	bool reversed;
	char drag_code;
	bool is_edit;


	bool show_bestline_info;
	std::vector< std::pair<std::string,std::string> > info_data;


	ChessApplication& controller;
};

#endif // CHESSBOARD_H_INCLUDED
