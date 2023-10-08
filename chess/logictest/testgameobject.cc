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

    string actual = game.save_game();
    string expected =
		"[Event \"F/S Return Match\"]\n"
		"[Site \"Belgrade, Serbia JUG\"]\n"
		"[Date \"1992.11.04\"]\n"
		"[Round \"29\"]\n"
		"[White \"Fischer, Robert J.\"]\n"
		"[Black \"Spassky, Boris V.\"]\n"
		"[Result \"1/2-1/2\"]\n"
		"\n"
		"1. e4 e5 2. Nf3 Nc6 3. Bb5 a6 4. Ba4 Nf6 5. O-O Be7 6. Re1 b5 7. Bb3 d6 8. c3\n"
		"O-O 9. h3 Nb8 10. d4 Nbd7 11. c4 c6 12. cxb5 axb5 13. Nc3 Bb7 14. Bg5 b4 15.\n"
		"Nb1 h6 16. Bh4 c5 17. dxe5 Nxe4 18. Bxe7 Qxe7 19. exd6 Qf6 20. Nbd2 Nxd6 21.\n"
		"Nc4 Nxc4 22. Bxc4 Nb6 23. Ne5 Rae8 24. Bxf7+ Rxf7 25. Nxf7 Rxe1+ 26. Qxe1 Kxf7\n"
		"27. Qe3 Qg5 28. Qxg5 hxg5 29. b3 Ke6 30. a3 Kd6 31. axb4 cxb4 32. Ra5 Nd5 33.\n"
		"f3 Bc8 34. Kf2 Bf5 35. Ra7 g6 36. Ra6+ Kc5 37. Ke1 Nf4 38. g3 Nxh3 39. Kd2 Kb5\n"
		"40. Rd6 Kc5 41. Ra6 Nf2 42. g4 Bd3 43. Re6 1/2-1/2\n";


    CPPUNIT_ASSERT_EQUAL( expected, actual );
}

