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

#include "teststhalvemove.h"

#include "../logicsrc/chessgame.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TestSTHalveMove );

TestSTHalveMove::TestSTHalveMove()
{
    //ctor
}

TestSTHalveMove::~TestSTHalveMove()
{
    //dtor
}

void TestSTHalveMove::test_toLAN()
{
    STMove test;

    test.castling = 'K'; CPPUNIT_ASSERT( move_to_LAN(test) == "0 - 0" );         // king side castling
    test.castling = 'k'; CPPUNIT_ASSERT( move_to_LAN(test) == "0 - 0" );         // king side castling
    test.castling = 'Q'; CPPUNIT_ASSERT( move_to_LAN(test) == "0 - 0 - 0" );     // Queen side castling
    test.castling = 'q'; CPPUNIT_ASSERT( move_to_LAN(test) == "0 - 0 - 0" );     // Queen side castling

    test.castling = '-';    // castling testing is done

    test.is_capture = false;    // set all the flags to false
    test.pawn_promo = false;
    test.attack_king = false;
    test.check_mate = false;

    test.orig_square.file = 0; test.orig_square.rank = 1;
    test.new_square.file = 0; test.new_square.rank = 3;

    // Try a couple of different pieces
    test.piece_type = 'P'; CPPUNIT_ASSERT( move_to_LAN(test) == "a2-a4" );
    test.piece_type = 'K'; CPPUNIT_ASSERT( move_to_LAN(test) == "Ka2-a4" );
    test.piece_type = 'Q'; CPPUNIT_ASSERT( move_to_LAN(test) == "Qa2-a4" );

    // Test capture
    test.is_capture = true;
    test.orig_square.file = 5; test.orig_square.rank = 3;
    test.new_square.file = 7; test.new_square.rank = 7;

    test.piece_type = 'P'; CPPUNIT_ASSERT( move_to_LAN(test) == "f4xh8" );
    test.piece_type = 'K'; CPPUNIT_ASSERT( move_to_LAN(test) == "Kf4xh8" );
    test.piece_type = 'Q'; CPPUNIT_ASSERT( move_to_LAN(test) == "Qf4xh8" );

    // Test pawn promotion
    test.pawn_promo = true;
    test.piece_type = 'R'; CPPUNIT_ASSERT( move_to_LAN(test) == "f4xh8R" );

    test.is_capture = false;
    test.piece_type = 'Q'; CPPUNIT_ASSERT( move_to_LAN(test) == "f4-h8Q" );

    test.pawn_promo = false;
    test.attack_king = true;
    test.piece_type = 'Q'; CPPUNIT_ASSERT( move_to_LAN(test) == "Qf4-h8+" );

    test.attack_king = false;
    test.check_mate = true;
    test.piece_type = 'Q'; CPPUNIT_ASSERT( move_to_LAN(test) == "Qf4-h8++" );


	CPPUNIT_ASSERT(true);
}
