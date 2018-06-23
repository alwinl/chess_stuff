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

#ifndef INFOAREA_H_INCLUDED
#define INFOAREA_H_INCLUDED

#include <string>
#include <vector>
#include <utility>

#pragma GCC diagnostic ignored "-Wparentheses"
#include <gtkmm.h>
#pragma GCC diagnostic warning "-Wparentheses"

/**-----------------------------------------------------------------------------
 * \brief Information area
 *
 * This class encapsulates drawing information on a DrawingArea widget
 */
class ChessInfo : public Gtk::DrawingArea
{
public:
	ChessInfo( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model );

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

	void set_colours( Gdk::RGBA bg, Gdk::RGBA fg );
    bool toggle_bestline();

private:
	virtual bool on_configure_event( GdkEventConfigure* event );
	virtual bool on_draw( const Cairo::RefPtr<Cairo::Context>& cr );

	void update();
	void set_text( int index, std::string text );

	std::pair<int,int> screen_size_;
	Gdk::RGBA background_colour;
	Gdk::RGBA foreground_colour;
	bool show_bestline_info;
	std::vector< std::pair<std::string,std::string> > info_data;
};

#endif // INFOAREA_H_INCLUDED
