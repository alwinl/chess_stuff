/*
 * Copyright 2022 Alwin Leerling <dna.leerling@gmail.com>
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
 */

#ifndef TESTBOARD_H
#define TESTBOARD_H

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class TestBoard : public CppUnit::TestFixture
{
public:
	CPPUNIT_TEST_SUITE( TestBoard );

    CPPUNIT_TEST( read_write_FEN );
    CPPUNIT_TEST( test_pawn_first_move );
    CPPUNIT_TEST( test_pawn_capture );
    CPPUNIT_TEST( test_pawn_promotion );

	CPPUNIT_TEST_SUITE_END();

private:
    void read_write_FEN();
	void test_pawn_first_move();
	void test_pawn_capture();
	void test_pawn_promotion();
};

#endif // TESTBOARD_H
