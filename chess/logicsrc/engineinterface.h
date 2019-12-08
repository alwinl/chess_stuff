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

#ifndef ENGINEINTERFACE_H_INCLUDED
#define ENGINEINTERFACE_H_INCLUDED

#include "chessengine.h"

/**-----------------------------------------------------------------------------
 * \brief Simplified look of the engine for the ChessBoard class
 *
 * This little class serves to unclutter the engine interface
 * for the ChessBoard class.
 *
 * Of course I can pass a pointer (reference) to the engine object
 * the controller holds but this emphasises the limited dependency
 * the ChessBoard class has on the engine.
 */
class EngineInterface
{
public:
	EngineInterface(ChessEngine& init) : engine(init) {};
	virtual ~EngineInterface() {};

	void put_piece_on_square( STSquare square, char piece )
		{ engine.put_piece_on_square( square, piece ); }

	void make_move(  STSquare start_square, STSquare end_square )
		{ engine.do_move( start_square, end_square ); }

private:
	ChessEngine& engine;
};

#endif // ENGINEINTERFACE_H_INCLUDED
