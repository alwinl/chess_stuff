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

#ifndef PIECEVALUES_H
#define PIECEVALUES_H

#include <map>

/**-----------------------------------------------------------------------------
 * \brief
 */
class PieceValues
{
public:
	enum ePieceIdx { QUEEN, ROOK, BISHOP, KNIGHT, PAWN, KING, PIECECOUNT };
	enum ePieceColour { WHITE, BLACK, COLOURCOUNT };

    PieceValues( );
    virtual ~PieceValues() {};

    bool choose_piece_values( );
    int get_piece_value( char index ) const;

    int get_piece_value( PieceValues::ePieceIdx index, PieceValues::ePieceColour colour ) const;

protected:
    virtual void setup( ) = 0;
    virtual bool manipulate() = 0;
    virtual void result( ) = 0;

	std::map<ePieceIdx,int> values;
};

#endif // PIECEVALUES_H
