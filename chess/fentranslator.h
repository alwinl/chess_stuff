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

#ifndef FENTRANSLATOR_H
#define FENTRANSLATOR_H

#include <string>
#include <map>

#include "pods.h"

class FENTranslator
{
public:
    FENTranslator();

    bool from_FEN( std::string FENstring );
    std::string to_FEN();

    bool remove_from_square( STSquare square );
    bool add_to_square( STSquare square, char piece );
    char query_square( STSquare square );

private:
    std::map<STSquare, char> content;
};

#endif // FENTRANSLATOR_H
