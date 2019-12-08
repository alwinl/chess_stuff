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

#include "piecevalues.h"


void PieceValues::init_piece_values( PieceValues::STPieceValues values )
{
	save_values = values;
}

bool PieceValues::choose_piece_values( )
{
	setup();
	bool first = manipulate( );
	if( first )
		save_values = result( );

	return first;
}

bool PieceValues::operator==( PieceValues::STPieceValues& rhs ) const
{
    if( save_values.QueenValue != rhs.QueenValue ) return false;
    if( save_values.RookValue != rhs.RookValue ) return false;
    if( save_values.BishopValue != rhs.BishopValue ) return false;
    if( save_values.KnightValue != rhs.KnightValue ) return false;
    if( save_values.PawnValue != rhs.PawnValue ) return false;

    return true;
}

