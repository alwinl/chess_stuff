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

ColourChooser::ColourChooser()
{
    colors.bg = "rgb(78,154,6)";
    colors.fg = "rgb(0,0,0)";
    colors.black = "rgb(85,87,83)";
    colors.white = "rgb(238,238,236)";
}

bool ColourChooser::choose_colours()
{
	set_colours( colors );

	bool result = manipulate_colours();

	if( result )
		colors = colours();

	return result;

}


