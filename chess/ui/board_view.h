/*
 * new_board.h Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include <array>

class BoardView : public Gtk::DrawingArea
{
public:
    BoardView(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);

    void toggle_bestline() { show_bestline = !show_bestline; queue_draw(); }
    void toggle_reversed() { reversed = ! reversed; queue_draw(); }
    void set_highlight( uint16_t square ) { highlight_square = square; queue_draw(); }
    void set_editing( bool on ) { is_editing = on; queue_draw(); }
    void set_piece_code_at( uint16_t square, char code ) { piece_positions[square] = code; queue_draw(); }
    void set_floating( std::pair<float,float> pos, char code ) { floating_pos = pos; floating_piece = code; queue_draw(); };
    void move_floating_by( std::pair<float,float> delta ) { if( floating_piece != ' ') { floating_pos.first += delta.first; floating_pos.second += delta.second; queue_draw(); } }
    void set_colours( std::array<std::string,4> new_colours ) { 
        background_colour = Gdk::RGBA(new_colours[0]);
        foreground_colour = Gdk::RGBA(new_colours[1]);
        white_colour = Gdk::RGBA(new_colours[2]);
        black_colour = Gdk::RGBA(new_colours[3]);
        queue_draw();
    }

    void set_turn( bool is_white ) { std_info[0] = is_white ? "white": "black"; queue_draw(); }
    void set_white_move( std::string move ) { std_info[1] = move; queue_draw(); }
    void set_black_move( std::string move ) { std_info[2] = move; queue_draw(); }
    void set_time( std::string elapsed ) { std_info[3] = elapsed; queue_draw(); }
    void set_level( std::string level ) { std_info[4] = level; queue_draw(); }
    void set_value( int value ) { std_info[5] = std::to_string( value ); queue_draw(); }
    void set_nodes( int nodes ) { std_info[6] = std::to_string( nodes ); queue_draw(); }
    void set_node_speed( int speed ) { std_info[7] = std::to_string( speed ); queue_draw(); }
    void set_depth( int depth ) { std_info[8] = std::to_string( depth ); queue_draw(); }
    void set_bestline( std::string move ) { std_info[9] = move; queue_draw(); }

private:
	Gdk::Rectangle board_outline;
	Gdk::Rectangle info_outline;
	Gdk::Rectangle edit_outline;

	Cairo::RefPtr<Cairo::ImageSurface> assets_image;

    std::array<std::string,10> std_info;
    Gdk::RGBA background_colour;
	Gdk::RGBA foreground_colour;
	Gdk::RGBA white_colour;
	Gdk::RGBA black_colour;
    std::array<char, 64> piece_positions;
    std::pair<float,float> floating_pos;
    char floating_piece = ' ';
    uint16_t highlight_square = (uint16_t)-1;
    bool is_editing = false;
    bool show_bestline = true;
    bool reversed = false;

    void calculate_outlines( int width, int height );

    void draw( const Cairo::RefPtr<Cairo::Context>& cr, int width, int height );

    void paint_background( const Cairo::RefPtr<Cairo::Context>& cr ) const;
    void paint_board( const Cairo::RefPtr<Cairo::Context>& cr ) const;
    void paint_pieces( const Cairo::RefPtr<Cairo::Context> &cr ) const;
    void paint_editwindow( const Cairo::RefPtr<Cairo::Context> &cr ) const;
    void paint_infowindow( const Cairo::RefPtr<Cairo::Context> &cr ) const;
    void paint_floating( const Cairo::RefPtr<Cairo::Context> &cr ) const;
    void paint_highlight( const Cairo::RefPtr<Cairo::Context> &cr ) const;
};
