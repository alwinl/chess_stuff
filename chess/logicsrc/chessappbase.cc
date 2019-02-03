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

#include "chessappbase.h"
#include "chessengine.h"
#include "pods.h"

ChessAppBase::ChessAppBase( ChessEngine* engine_init ) :  engine(engine_init)
{
}

ChessAppBase::~ChessAppBase()
{
}

void ChessAppBase::start_move( STSquare square ) { engine->start_move(square); };
void ChessAppBase::do_move( STSquare start_square, STSquare end_square ){ engine->do_move( start_square, end_square ); };
void ChessAppBase::cancel_move() { engine->cancel_move(); };
void ChessAppBase::advance() { engine->advance(); };

void ChessAppBase::arrange_start() { engine->arranging_start(); };
void ChessAppBase::arrange_clear() { engine->arranging_clear(); };
void ChessAppBase::arrange_drop( STSquare square, char piece ) { engine->arranging_drop( square, piece ); };
void ChessAppBase::arrange_end( bool canceled ) { engine->arranging_end(canceled); };


void ChessAppBase::piece_value_changes() { engine->piece_value_changes(); };

void ChessAppBase::new_game() { engine->new_game(); };
void ChessAppBase::end_app() { engine->quit(); };
void ChessAppBase::open_file() { engine->open_file(); };
void ChessAppBase::save_file() { engine->save_file(); };
void ChessAppBase::save_as() { engine->save_as(); };


