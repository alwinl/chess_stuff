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

#include "testgameobject.h"

#include "chessgame.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TestGameObject );

TestGameObject::TestGameObject()
{
}

TestGameObject::~TestGameObject()
{
}

void TestGameObject::create_object()
{
    ChessGame game = ChessGame();

    CPPUNIT_ASSERT(true);
}

void TestGameObject::save_load()
{
	ChessGame game;

	ifstream is( "game.pgn" );

	if( !is.good() )
		CPPUNIT_FAIL( "cannot open 'game.pgn'");

	game.load_game( string( istreambuf_iterator<char>(is), istreambuf_iterator<char>() ) );

    is.close();

    string actual = "\"" + game.save_game() + "\"";
    string expected =
		"\"[Event \"F/S Return Match\"]\n"
		"[Site \"Belgrade, Serbia JUG\"]\n"
		"[Date \"1992.11.04\"]\n"
		"[Round \"29\"]\n"
		"[White \"Fischer, Robert J.\"]\n"
		"[Black \"Spassky, Boris V.\"]\n"
		"[Result \"1/2-1/2\"]\n"
		"\n\"";

    CPPUNIT_ASSERT_EQUAL( expected, actual );
}
