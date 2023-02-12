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

#include "testtimeinputter.h"

#include "timeinputter.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TestTimeInputter );


/*****************************************************************************
 * Helper class to test the colour chooser logic
 */

class MockTimeInputterOk : public TimeInputter
{
public:
    MockTimeInputterOk( int return_value ) : TimeInputter(), save_value(return_value) {};

private:
    virtual void setup( const std::string& title, const std::string& prompt, int& value ) { };
    virtual bool manipulate( ) { return true; };
    virtual int result( ) { return save_value; };	/* return whatever this object is initialised with */

	int save_value;
};

class MockTimeInputterCancel : public TimeInputter
{
public:
    MockTimeInputterCancel( ) : TimeInputter() {};

private:
    virtual void setup( const std::string& title, const std::string& prompt, int& value ) {};
    virtual bool manipulate( ) { return false; };
    virtual int result( ) { return -1; };		/* this should not be called, so give it an invalid value */
};



/*****************************************************************************
 * TestTimeInputter class implementation
 */

void TestTimeInputter::no_change_exit_ok()
{
	MockTimeInputterOk test_object( 60 );

	test_object.time_per_move( 60 );

	CPPUNIT_ASSERT_EQUAL( 60, test_object.get_time() );
}

void TestTimeInputter::no_change_exit_cancel()
{
	MockTimeInputterCancel test_object;

	test_object.time_per_move( 60 );

	CPPUNIT_ASSERT_EQUAL( 60, test_object.get_time() );
}

void TestTimeInputter::change_exit_ok()
{
	MockTimeInputterOk test_object( 40 );

	test_object.time_per_move( 60 );

	CPPUNIT_ASSERT_EQUAL( 40, test_object.get_time() );
}

void TestTimeInputter::change_exit_cancel()
{
	MockTimeInputterCancel test_object;

	test_object.time_per_move( 60 );

	CPPUNIT_ASSERT_EQUAL( 60, test_object.get_time() );
}
