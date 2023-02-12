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

#include "piecevalues.h"
#include "pods.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TestPieceValues );

/*****************************************************************************
 * Helper classes to test the piece value logic
 */

class MockPieceValuesOk : public PieceValues
{
public:
    MockPieceValuesOk() : PieceValues() {};

    bool operator==( const MockPieceValuesOk& rhs ) const
    {
    	for( const std::pair<const ePieceIdx,int> entry : values ) {

			std::map<ePieceIdx,int>::const_iterator it = rhs.values.find( entry.first );
			if( it == rhs.values.end() )
				return false;

			if( entry.second != it->second )
				return false;
    	}

		return true;
    }

private:
    virtual void setup( ) { };
    virtual bool manipulate() { return true; };
    virtual void result( ) { };
};

class MockPieceValuesCancel : public PieceValues
{
public:
    MockPieceValuesCancel() : PieceValues() {};

    bool operator==( const MockPieceValuesCancel& rhs ) const
    {
    	for( const std::pair<const ePieceIdx,int> entry : values ) {

			std::map<ePieceIdx,int>::const_iterator it = rhs.values.find( entry.first );
			if( it == rhs.values.end() )
				return false;

			if( entry.second != it->second )
				return false;
    	}

		return true;
    }

private:
    virtual void setup( ) { };
    virtual bool manipulate() { return false; };
    virtual void result( ) { };
};



/*****************************************************************************
 * TestColourChooser class implementation
 */

void TestPieceValues::no_change_exit_ok()
{
    //PieceValues::STPieceValues current_values = { .QueenValue = 1, .RookValue = 2, .BishopValue = 3, .KnightValue = 4, .PawnValue = 5 };

    MockPieceValuesOk test_object;

    CPPUNIT_ASSERT_EQUAL( true, test_object.choose_piece_values( ) );
    CPPUNIT_ASSERT( test_object == MockPieceValuesOk() );
}

void TestPieceValues::no_change_exit_cancel()
{
    //PieceValues::STPieceValues current_values = { .QueenValue = 1, .RookValue = 2, .BishopValue = 3, .KnightValue = 4, .PawnValue = 5 };

    MockPieceValuesCancel test_object;

    CPPUNIT_ASSERT_EQUAL( false, test_object.choose_piece_values( ) );
}

void TestPieceValues::change_exit_ok()
{
    //PieceValues::STPieceValues new_values = { .QueenValue = 1, .RookValue = 2, .BishopValue = 3, .KnightValue = 4, .PawnValue = 5 };
    //PieceValues::STPieceValues current_values = { .QueenValue = 6, .RookValue = 7, .BishopValue = 8, .KnightValue = 9, .PawnValue = 10 };

    MockPieceValuesOk test_object;

    CPPUNIT_ASSERT_EQUAL( true, test_object.choose_piece_values( ) );
    CPPUNIT_ASSERT( test_object == MockPieceValuesOk() );
}

void TestPieceValues::change_exit_cancel()
{
    //PieceValues::STPieceValues new_values = { .QueenValue = 1, .RookValue = 2, .BishopValue = 3, .KnightValue = 4, .PawnValue = 5 };
    //PieceValues::STPieceValues current_values = { .QueenValue = 6, .RookValue = 7, .BishopValue = 8, .KnightValue = 9, .PawnValue = 10 };

    MockPieceValuesCancel test_object;

    //test_object.init_piece_values( current_values );

    CPPUNIT_ASSERT_EQUAL( false, test_object.choose_piece_values( ) );
}

