/*
 * Copyright 2019 Alwin Leerling <dna.leerling@gmail.com>
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

#ifndef PLY_H
#define PLY_H

#include <string>

#include "pods.h"


class Ply
{
public:
	enum class Colour { WHITE, BLACK };
    Ply( Colour col = Colour::WHITE, int moveno_ = -1, std::string SAN_text = "" ) : colour( col ), moveno(moveno_), SAN(SAN_text) {};

    void add_comment( std::string the_comment ) { comment = the_comment; }

    STSquare get_start_square() const { return start_square; }
    STSquare get_end_square() const { return end_square; }

protected:

private:
	Colour colour;
	int moveno;
	std::string SAN;
	std::string comment;
	STSquare start_square;
	STSquare end_square;
};

#endif // PLY_H
