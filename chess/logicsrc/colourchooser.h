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

class STColours;

/** \brief
 */
class ColourChooser
{
public:
    ColourChooser() {};
    virtual ~ColourChooser() {};

    std::pair<bool,STColours> choose_colours( STColours local_colours );

protected:
    virtual void set_colours( STColours& colours ) = 0;
    virtual bool manipulate_colours( ) = 0;
    virtual STColours colours( ) = 0;
};


#endif // COLOURCHOOSER_H
