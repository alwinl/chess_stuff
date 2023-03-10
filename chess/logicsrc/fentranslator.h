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

class STSquare;
class STPiece;

/** \brief
 */
class FENTranslator
{
public:
    static std::map<STSquare,STPiece> from_FEN( std::string FENstring );
    static std::string to_FEN( std::map<STSquare, STPiece> content );
};

#endif // FENTRANSLATOR_H
