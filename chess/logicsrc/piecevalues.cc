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

void PieceValues::push_stpiece_values( STPieceValues values )
{
	save_values = values;
}

bool PieceValues::get_new_piece_values( )
{
	set_values( save_values );
	bool first = manipulate_values( );
	if( first )
		save_values = piece_values( );

	return first;
}

STPieceValues PieceValues::pull_stpiece_values()
{
	return save_values;
}
