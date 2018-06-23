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

#ifndef CHESSEDIT_H
#define CHESSEDIT_H

#include <map>

#pragma GCC diagnostic ignored "-Wparentheses"
#include <gtkmm.h>
#pragma GCC diagnostic warning "-Wparentheses"

class ChessApplication;


class ChessEdit : public Gtk::DrawingArea
{
private:
	static const int SQUARE_SIZE = 36;

public:
	ChessEdit( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model, ChessApplication& app );

    void set_colours( Gdk::RGBA bg );

private:
	virtual bool on_configure_event( GdkEventConfigure* event );
	virtual bool on_draw( const Cairo::RefPtr<Cairo::Context>& cr );
	virtual bool on_button_press_event(GdkEventButton* button_event);
    virtual bool on_button_release_event( GdkEventButton* release_event );
    virtual bool on_motion_notify_event( GdkEventMotion* motion_event );

	void update();

	ChessApplication& controller;
	Gdk::Rectangle outline_in_pixels;
	Cairo::RefPtr<Cairo::ImageSurface> pieces_surface_;
	std::map< char, Gdk::Point > source_offsets;
	Gdk::RGBA background_colour;
	char drag_code;
	Gdk::Point drag_point;
};

#endif // CHESSEDIT_H
