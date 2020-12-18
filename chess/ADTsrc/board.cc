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

#include "board.h"
#include "fentranslator.h"

#include "../ADTsrc/pods.h"

Board::Board()
{
	//ctor
}

Board Board::standard_opening_board()
{
	pieces = FENTranslator().from_FEN( "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR" );

	return *this;
}

Board Board::add_piece( STSquare square, STPiece new_piece )
{
	pieces.insert( std::pair<STSquare, STPiece>( square, new_piece) );

	return *this;
}

Board Board::remove_piece( STSquare square )
{
	return *this;
}

Board Board::move_piece( STSquare old_square, STSquare new_square )
{
	return *this;
}

Board Board::capture_piece( STSquare old_square, STSquare new_square )
{
	return *this;
}
