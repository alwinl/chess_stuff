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

#include "testfentranslator.h"

#include "fentranslator.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TestFENTranslator );

TestFENTranslator::TestFENTranslator()
{
}

TestFENTranslator::~TestFENTranslator()
{
}


void TestFENTranslator::FENstring_test()
{
    FENTranslator test_object;

    test_object.from_FEN( "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR" );
    CPPUNIT_ASSERT( test_object.to_FEN() == "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR" );
}

void TestFENTranslator::query_test()
{
    FENTranslator test_object;
    STSquare test_square;

    test_object.from_FEN( "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR" );

    test_square.file = 0; test_square.rank = 7;
    CPPUNIT_ASSERT( test_object.query_square(test_square) == 'r' );

    test_square.file = 7; test_square.rank = 7;
    CPPUNIT_ASSERT( test_object.query_square(test_square) == 'r' );

    test_square.file = 7; test_square.rank = 0;
    CPPUNIT_ASSERT( test_object.query_square(test_square) == 'R' );

    test_square.file = 0; test_square.rank = 0;
    CPPUNIT_ASSERT( test_object.query_square(test_square) == 'R' );
}

void TestFENTranslator::blank_test()
{
    FENTranslator test_object;
    STSquare test_square;

    test_object.from_FEN( "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR" );

    test_square.file = 4; test_square.rank = 4;
    CPPUNIT_ASSERT( test_object.query_square(test_square) == ' ' );
}

void TestFENTranslator::remove_test()
{
    FENTranslator test_object;
    STSquare test_square;

    test_object.from_FEN( "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR" );

    test_square.file = 0; test_square.rank = 7;
    CPPUNIT_ASSERT( test_object.remove_from_square(test_square) == true );
    CPPUNIT_ASSERT( test_object.query_square(test_square) == ' ' );
}

void TestFENTranslator::add_test()
{
    FENTranslator test_object;
    STSquare test_square;

    test_object.from_FEN( "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR" );

    test_square.file = 4; test_square.rank = 4;
    CPPUNIT_ASSERT( test_object.add_to_square(test_square, 'n' ) == true );
    CPPUNIT_ASSERT( test_object.query_square(test_square) == 'n' );
}

void TestFENTranslator::invalid_add_test()
{
    FENTranslator test_object;
    STSquare test_square;

    test_object.from_FEN( "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR" );

    test_square.file = 0; test_square.rank = 7;
    CPPUNIT_ASSERT( test_object.add_to_square( test_square, 'P' ) == false );
    CPPUNIT_ASSERT( test_object.query_square(test_square) == 'r' );
}

void TestFENTranslator::invalid_remove_test()
{
    FENTranslator test_object;
    STSquare test_square;

    test_object.from_FEN( "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR" );

    test_square.file = 4; test_square.rank = 4;
    CPPUNIT_ASSERT( test_object.remove_from_square( test_square ) == false );
    CPPUNIT_ASSERT( test_object.query_square(test_square) == ' ' );
}

