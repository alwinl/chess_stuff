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

#ifndef TESTTIMEINPUTTER_H
#define TESTTIMEINPUTTER_H

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

// test cases
// 1 user initiates dialog, makes no changes and exits pressing ok
// 2 user initiates dialog, makes no changes and exits pressing cancel
// 3 user initiates dialog, makes changes and exits pressing ok
// 4 user initiates dialog, makes changes and exits pressing cancel

class TestTimeInputter : public CppUnit::TestFixture
{
public:
    TestTimeInputter() {};
    virtual ~TestTimeInputter() {};

	CPPUNIT_TEST_SUITE( TestTimeInputter );

	CPPUNIT_TEST( no_change_exit_ok );
	CPPUNIT_TEST( no_change_exit_cancel );
	CPPUNIT_TEST( change_exit_ok );
	CPPUNIT_TEST( change_exit_cancel );

	CPPUNIT_TEST_SUITE_END();

private:
	void no_change_exit_ok();
	void no_change_exit_cancel();
	void change_exit_ok();
	void change_exit_cancel();
};

#endif // TESTTIMEINPUTTER_H
