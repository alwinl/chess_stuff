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

ChessAppBase::ChessAppBase( ChessEngine* engine_init ) :  engine(engine_init)  { }

ChessAppBase::~ChessAppBase() { }

void ChessAppBase::do_move( STSquare start_square, STSquare end_square ){ engine->do_move( start_square, end_square ); };
void ChessAppBase::cancel_move() { engine->cancel_move(); };
void ChessAppBase::advance() { engine->advance(); };
void ChessAppBase::hint() { engine->hint(); };
void ChessAppBase::arrange_start() { engine->arranging_start(); };
void ChessAppBase::arrange_clear() { engine->arranging_clear(); };
//void ChessAppBase::remove_piece_from_square( STSquare square ) { engine->remove_piece_from_square( square ); };
void ChessAppBase::put_piece_on_square( STSquare square, char piece ) { engine->put_piece_on_square( square, piece ); };
void ChessAppBase::arrange_end( bool canceled ) { engine->arranging_end(canceled); };
void ChessAppBase::piece_value_changes() { engine->piece_value_changes(); };
void ChessAppBase::new_game() { engine->new_game(); };
void ChessAppBase::end_app() { engine->quit(); };
void ChessAppBase::open_file() { engine->open_file(); };
void ChessAppBase::save_file() { engine->save_file(); };
void ChessAppBase::save_as() { engine->save_as(); };

void ChessAppBase::undo() { engine->undo(); }
void ChessAppBase::redo() { engine->redo(); }
void ChessAppBase::stop_thinking() { engine->stop_thinking(); }
char ChessAppBase::get_piece( STSquare square ) { return engine->get_piece( square ); }
void ChessAppBase::change_level( eLevels new_level ) { engine->change_level( new_level ); }
void ChessAppBase::arrange_turn( eTurns new_turn ) { engine->arrange_turn( new_turn ); }


