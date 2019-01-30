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

#ifndef TESTSTHALVEMOVE_H
#define TESTSTHALVEMOVE_H

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

/** \brief
 */
class TestSTHalveMove : public CppUnit::TestFixture
{
public:
    TestSTHalveMove();
    virtual ~TestSTHalveMove();

	CPPUNIT_TEST_SUITE( TestSTHalveMove );

	CPPUNIT_TEST( test_toLAN );
	//CPPUNIT_TEST( InvalidTests );

	CPPUNIT_TEST_SUITE_END();

private:

    void test_toLAN();

};

#endif // TESTSTHALVEMOVE_H
