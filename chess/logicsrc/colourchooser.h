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

#ifndef COLOURCHOOSER_H
#define COLOURCHOOSER_H

#include <utility>
#include <string>

/** \brief
 */
class ColourChooser
{
public:
	/**-----------------------------------------------------------------------------
	 * \brief Struct to hold the colours we need in this application
	 *
	 * Note: each string holds a colour in the notation: rgb(r,g,b) where each colour
	 * component is in the range 0..255
	 */
	struct STColours
	{
		std::string bg;
		std::string fg;
		std::string white;
		std::string black;
	};

    ColourChooser();
    virtual ~ColourChooser() {};

    void init_colours( STColours& init ) { colors = init; };

    bool choose_colours();

	bool is_colour( STColours& rhs ) const
	{
		if( colors.bg != rhs.bg ) return false;
		if( colors.fg != rhs.fg ) return false;
		if( colors.white != rhs.white ) return false;
		if( colors.black != rhs.black ) return false;

		return true;
	}

	STColours get_colours() const { return colors; };

protected:
    virtual void set_colours( STColours& colours ) = 0;
    virtual bool manipulate_colours( ) = 0;
    virtual STColours colours( ) = 0;

	STColours colors;
};


#endif // COLOURCHOOSER_H
