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

ChessAppBase::ChessAppBase( ChessEngine* engine_init ) :  engine(engine_init)  { }

ChessAppBase::~ChessAppBase() { }

void ChessAppBase::do_move( STSquare start_square, STSquare end_square ){ engine->do_move( start_square, end_square ); };
void ChessAppBase::cancel_move() { engine->cancel_move(); };
void ChessAppBase::advance() { engine->advance(); };
STSquare ChessAppBase::hint() { return engine->hint(); };
void ChessAppBase::arrange_start() { engine->arranging_start(); };
void ChessAppBase::arrange_clear() { engine->arranging_clear(); };
void ChessAppBase::put_piece_on_square( STSquare square, char piece ) { engine->put_piece_on_square( square, piece ); };
void ChessAppBase::arrange_end( bool canceled ) { engine->arranging_end(canceled); };
void ChessAppBase::new_game() { engine->new_game(); };
bool ChessAppBase::can_quit() { return engine->can_quit(); };
bool ChessAppBase::open_file( std::string name ) { return engine->open_file( name ); };
bool ChessAppBase::save_file( std::string name ) { return engine->save_file( name ); };
STPieceValues ChessAppBase::get_piece_values() { return engine->get_piece_values(); };
void ChessAppBase::set_piece_values( STPieceValues piece_values ) { engine->set_piece_values(piece_values); };
void ChessAppBase::undo() { engine->undo(); }
void ChessAppBase::redo() { engine->redo(); }
void ChessAppBase::stop_thinking() { engine->stop_thinking(); }
char ChessAppBase::get_piece( STSquare square ) { return engine->get_piece( square ); }
void ChessAppBase::change_level( eLevels new_level, int time_parameter ) { engine->change_level( new_level, time_parameter ); }
void ChessAppBase::arrange_turn( eTurns new_turn ) { engine->arrange_turn( new_turn ); }
