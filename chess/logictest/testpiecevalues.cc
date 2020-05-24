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

#include "testpiecevalues.h"
#include "../logicsrc/piecevalues.h"
#include "../ADTsrc/pods.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TestPieceValues );

/*****************************************************************************
 * Helper classes to test the piece value logic
 */

class MockPieceValuesOk : public PieceValues
{
public:
    MockPieceValuesOk( STPieceValues& values ) : PieceValues(), new_values(values) {};

private:
    STPieceValues new_values;

    virtual void setup( ) { };
    virtual bool manipulate() { return true; };
    virtual STPieceValues result( ) { return new_values; };
};

class MockPieceValuesCancel : public PieceValues
{
public:
    MockPieceValuesCancel(  STPieceValues& values ) : PieceValues(), the_values(values) {};

private:
    STPieceValues the_values;

    virtual void setup( ) { };
    virtual bool manipulate() { return false; };
    virtual STPieceValues result( ) { return the_values; };
};



/*****************************************************************************
 * TestColourChooser class implementation
 */

bool operator==( PieceValues::STPieceValues& lhs, PieceValues::STPieceValues& rhs )
{
    if( lhs.QueenValue != rhs.QueenValue ) return false;
    if( lhs.RookValue != rhs.RookValue ) return false;
    if( lhs.BishopValue != rhs.BishopValue ) return false;
    if( lhs.KnightValue != rhs.KnightValue ) return false;
    if( lhs.PawnValue != rhs.PawnValue ) return false;

    return true;
}


void TestPieceValues::no_change_exit_ok()
{
    PieceValues::STPieceValues current_values = { .QueenValue = 1, .RookValue = 2, .BishopValue = 3, .KnightValue = 4, .PawnValue = 5 };

    MockPieceValuesOk test_object( current_values );

    test_object.init_piece_values( current_values );

    CPPUNIT_ASSERT_EQUAL( true, test_object.choose_piece_values( ) );
    CPPUNIT_ASSERT( test_object == current_values );
}

void TestPieceValues::no_change_exit_cancel()
{
    PieceValues::STPieceValues current_values = { .QueenValue = 1, .RookValue = 2, .BishopValue = 3, .KnightValue = 4, .PawnValue = 5 };

    MockPieceValuesCancel test_object( current_values );

    test_object.init_piece_values( current_values );

    CPPUNIT_ASSERT_EQUAL( false, test_object.choose_piece_values( ) );
}

void TestPieceValues::change_exit_ok()
{
    PieceValues::STPieceValues new_values = { .QueenValue = 1, .RookValue = 2, .BishopValue = 3, .KnightValue = 4, .PawnValue = 5 };
    PieceValues::STPieceValues current_values = { .QueenValue = 6, .RookValue = 7, .BishopValue = 8, .KnightValue = 9, .PawnValue = 10 };

    MockPieceValuesOk test_object( new_values );

    test_object.init_piece_values( current_values );

    CPPUNIT_ASSERT_EQUAL( true, test_object.choose_piece_values( ) );
    CPPUNIT_ASSERT( test_object == new_values );
}

void TestPieceValues::change_exit_cancel()
{
    PieceValues::STPieceValues new_values = { .QueenValue = 1, .RookValue = 2, .BishopValue = 3, .KnightValue = 4, .PawnValue = 5 };
    PieceValues::STPieceValues current_values = { .QueenValue = 6, .RookValue = 7, .BishopValue = 8, .KnightValue = 9, .PawnValue = 10 };

    MockPieceValuesCancel test_object( new_values );

    test_object.init_piece_values( current_values );

    CPPUNIT_ASSERT_EQUAL( false, test_object.choose_piece_values( ) );
}

