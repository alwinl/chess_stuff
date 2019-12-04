/*
 * Copyright 2017 Alwin Leerling <alwin@jambo>
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

#include "colourchooser.h"
#include "pods.h"

bool ColourChooser::choose_colours()
{
	setup( );

	bool ret = manipulate();

	if( ret )
		colors = result();

	return ret;

}

bool ColourChooser::is_colour( STColours& rhs ) const
{
	if( colors.bg != rhs.bg ) return false;
	if( colors.fg != rhs.fg ) return false;
	if( colors.white != rhs.white ) return false;
	if( colors.black != rhs.black ) return false;

	return true;
}


