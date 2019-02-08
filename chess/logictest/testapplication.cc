/*
 * [program] Copyright 2014 Alwin Leerling <alwin@jambo>
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

#include "testapplication.h"

#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <iostream>

#include "../logicsrc/pods.h"

TestApplication::TestApplication( ChessEngine* engine_init ) : ChessAppBase( engine_init )
{
    //ctor
}

int TestApplication::run( int argc, char *argv[] )
{
	 // Get the top level suite from the registry
	CppUnit::Test *suite = CppUnit::TestFactoryRegistry::getRegistry().makeTest();

	// Adds the test to the list of test to run
	CppUnit::TextUi::TestRunner runner;
	runner.addTest( suite );

	// Change the default outputter to a compiler error format outputter
	runner.setOutputter( new CppUnit::CompilerOutputter( &runner.result(), std::cerr ) );

	return runner.run() ? 0 : 1;		// Return error code 1 if the one of test failed.
}

void TestApplication::set_piece_positions( std::string FEN_string ) {}
void TestApplication::set_info( STInfo& info ) {}
void TestApplication::push_statusbar_text( std::string message ) {}
void TestApplication::message_dialog( std::string message ) {}
STPieceValues TestApplication::edit_piecevalues( STPieceValues& current ) { return current; }
void animate( STSquare start_square, STSquare end_square, char piece ) {}

void TestApplication::start_arranging() {};
void TestApplication::end_arranging() {};

std::string TestApplication::open_filename( std::string filename, std::string working_dir ) { return "open_name.chess"; }
std::string TestApplication::save_filename( std::string filename, std::string working_dir ) { return "save_name.chess"; }

void TestApplication::quit() {}
