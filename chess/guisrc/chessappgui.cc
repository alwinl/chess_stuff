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

#include "chessappgui.h"
#include "chesscontroller.h"

#include "../logicsrc/pods.h"

ChessAppGUI::ChessAppGUI( ChessEngine* engine_init ) : ChessAppBase(engine_init)
{
}

ChessAppGUI::~ChessAppGUI()
{
}

/* called from main function */;
int ChessAppGUI::run(  int argc, char *argv[] )
{
	controller = ChessController::create( this );
	return controller->run( argc, argv );
}

/* Called from logic */
void ChessAppGUI::set_piece_positions( std::string FEN_string )
	{ controller->set_piece_positions( FEN_string ); }

void ChessAppGUI::set_info( STInfo& info )
	{ controller->set_info( info ); }

void ChessAppGUI::push_statusbar_text( std::string message )
	{ controller->push_statusbar_text( message ); }

void ChessAppGUI::message_dialog( std::string message )
	{ controller->message_dialog( message ); }

void ChessAppGUI::start_arranging()
	{ controller->start_edit_mode(); }

void ChessAppGUI::end_arranging()
	{ controller->end_edit_mode(); }

STPieceValues ChessAppGUI::edit_piecevalues( STPieceValues& current )
	{ return controller->run_piece_value_dialog( current ); }

void ChessAppGUI::animate( STSquare start_square, STSquare end_square, char piece )
	{ controller->animate( start_square, end_square, piece ); }

std::string ChessAppGUI::open_filename( std::string filename, std::string working_dir )
	{ return controller->open_filename( filename, working_dir ); }

std::string ChessAppGUI::save_filename( std::string filename, std::string working_dir )
	{ return controller->save_filename( filename, working_dir ); }

void ChessAppGUI::quit()
	{ controller->quit(); }
