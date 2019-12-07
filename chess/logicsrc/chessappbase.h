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

#ifndef CHESSAPPBASE_H
#define CHESSAPPBASE_H

#include <string>

#include "pods.h"

class PieceValues;
class FilenameChooser;

class ChessEngine;


/** \brief
 */
class ChessAppBase
{
public:
	ChessAppBase( ChessEngine* engine_init ) :  engine(engine_init)  { };
	virtual ~ChessAppBase() { };

	/* called from main function */;
	virtual int run(  int argc, char *argv[] ) = 0;

	/* Called from logic */
    virtual void set_piece_positions( std::string FEN_string ) = 0;
    virtual void set_info(  STInfo& info ) = 0;
	virtual void animate( STSquare start_square, STSquare end_square, char piece ) = 0;

    ChessEngine* get_engine() const { return engine; };

private:
	ChessEngine* engine;
};

#endif // CHESSAPPBASE_H
