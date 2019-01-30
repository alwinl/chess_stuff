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

#include "chessinfo.h"

using namespace std;

/**-----------------------------------------------------------------------------
 * \brief
 *
 * \param cobject BaseObjectType*
 * \param ui_model const Glib::RefPtr<Gtk::Builder>&
 * \param model_init AppModel&
 *
 */
ChessInfo::ChessInfo( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model ) : Gtk::DrawingArea(cobject)
{
    info_data.push_back( pair<string,string>("Turn", "empty") );
    info_data.push_back( pair<string,string>("White", "empty") );
    info_data.push_back( pair<string,string>("Black", "empty") );
    info_data.push_back( pair<string,string>("Time", "empty") );
    info_data.push_back( pair<string,string>("Level", "empty") );
    info_data.push_back( pair<string,string>("Value", "empty") );
    info_data.push_back( pair<string,string>("Nodes", "empty") );
    info_data.push_back( pair<string,string>("N/Sec", "empty") );
    info_data.push_back( pair<string,string>("Depth", "empty") );
    info_data.push_back( pair<string,string>("Bestline", "empty") );

    show_bestline_info = true;
}

/**-----------------------------------------------------------------------------
 * \brief Called when screen size changes
 *
 * Recalculate our screen real estate. Can be used as a trigger to
 * recalculate other drawing parameters.
 *
 * \param GdkEventConfigure*event
 * \return bool
 *
 */
bool ChessInfo::on_configure_event( GdkEventConfigure*event )
{
	Gtk::Allocation allocation = get_allocation();

	screen_size_ = pair<int,int>( allocation.get_width(), allocation.get_height() );

	return false;
}

/**-----------------------------------------------------------------------------
 * \brief Paint the information
 *
 * \param cr const Cairo::RefPtr<Cairo::Context>&
 * \return bool
 *
 */
bool ChessInfo::on_draw( const Cairo::RefPtr<Cairo::Context>& cr )
{
    Cairo::TextExtents extents;

    //cr->select_font_face( "Sans", Cairo::FontSlant::FONT_SLANT_NORMAL, Cairo::FontWeight::FONT_WEIGHT_NORMAL );
    cr->set_font_size( 12 );

	// draw background
	cr->set_source_rgb( background_colour.get_red(), background_colour.get_green(), background_colour.get_blue() );
 	cr->paint();

 	// Draw a frame
    cr->get_text_extents( info_data[0].first, extents ); // Need it for the height of the frame

    int height = (show_bestline_info ? 12 : 11 ) * (extents.height + 5);

    cr->set_source_rgb(  foreground_colour.get_red(), foreground_colour.get_green(), foreground_colour.get_blue() );
	cr->rectangle( 5, 10, screen_size_.first - 10, height );
	cr->stroke();

	// blank the area behind the word "Information"
    cr->get_text_extents( "Information", extents );

	cr->set_source_rgb( background_colour.get_red(), background_colour.get_green(), background_colour.get_blue() );
	cr->rectangle( 10, 5, extents.width + 10, extents.height );
	cr->fill();

	// now paint the word "Information"
    cr->set_source_rgb(  foreground_colour.get_red(), foreground_colour.get_green(), foreground_colour.get_blue() );
    cr->move_to( 15, extents.height + 5 );
    cr->show_text( "Information" );

	// paint the information
    cr->get_text_extents( info_data[9].first, extents ); // "Bestline" is the longest word

    double offset_y = extents.height + 25;
    double label_offset_x = 10;
    double info_offset_x = 10 + extents.width + 10;

    vector< pair<string,string> >::iterator last_item = info_data.end();

    if( ! show_bestline_info )
        --last_item;

    for( vector< pair<string,string> >::iterator it = info_data.begin(); it != last_item; ++it ) {
        cr->move_to( label_offset_x, offset_y );
        cr->show_text( (*it).first );
        cr->move_to( info_offset_x, offset_y );
        cr->show_text( (*it).second );
        offset_y += extents.height + 5;
    }

    return false;
}


/**-----------------------------------------------------------------------------
 * \brief Invalidating the window causes the on_draw function to be called
 *
 * \return void
 */
void ChessInfo::update()
{
    Glib::RefPtr<Gdk::Window> win = get_window();
    if( !win )      // window has not been realised yet
        return;

    win->invalidate( true);
}

/**-----------------------------------------------------------------------------
 * \brief
 *
 * \param bg Gdk::RGBA
 * \param fg Gdk::RGBA
 * \return void
 */
void ChessInfo::set_colours( Gdk::RGBA bg, Gdk::RGBA fg )
{
    background_colour = bg;
    foreground_colour = fg;
    update();
}

/**-----------------------------------------------------------------------------
 * \brief
 *
 * \return bool
 */
bool ChessInfo::toggle_bestline()
{
    show_bestline_info = ! show_bestline_info;
    update();

    return show_bestline_info;
}

/**-----------------------------------------------------------------------------
 * \brief
 *
 * \param index int
 * \param text std::string
 * \return void
 */
void ChessInfo::set_text( int index, string text )
{
    info_data[index].second = text;
    update();
}

