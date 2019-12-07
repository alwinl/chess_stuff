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

#include <utility>


/** \brief
 */
class PieceValues
{
public:
	/**-----------------------------------------------------------------------------
	 * \brief Struct to hold
	 *
	 */
	struct STPieceValues
	{
		int QueenValue;
		int RookValue;
		int BishopValue;
		int KnightValue;
		int PawnValue;
	};

    PieceValues( ) {};
    virtual ~PieceValues() {};

    //void push_map( std::map<char,int> new_map );
    //std::map<char,int> pull_map( );

    void init_piece_values( STPieceValues values );
    bool choose_piece_values( );
    STPieceValues get_piece_values() const { return save_values; };

    bool operator==( STPieceValues& rhs );

protected:
    virtual void setup() = 0;
    virtual bool manipulate() = 0;
    virtual STPieceValues result( ) = 0;

private:
    STPieceValues save_values;
};

#endif // PIECEVALUES_H
