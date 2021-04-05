/*
 * Copyright 2021 Alwin Leerling <alwin@jambo>
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

#ifndef TESTPNGTOKEN_H
#define TESTPNGTOKEN_H

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class TestPNGToken : public CppUnit::TestFixture
{
public:
    TestPNGToken();
    virtual ~TestPNGToken();

	CPPUNIT_TEST_SUITE( TestPNGToken );

	CPPUNIT_TEST( check_EOFToken );
	CPPUNIT_TEST( check_EOSToken );
	CPPUNIT_TEST( check_symbol );
	CPPUNIT_TEST( check_self_terminators );
	CPPUNIT_TEST( check_nag );
	CPPUNIT_TEST( check_tagpair );
	CPPUNIT_TEST( check_linefeed );
	CPPUNIT_TEST( check_tagpair_section );

	CPPUNIT_TEST_SUITE_END();

private:
	void check_EOFToken();
	void check_EOSToken();
	void check_symbol();
	void check_self_terminators();
	void check_nag();
	void check_tagpair();
	void check_linefeed();
	void check_tagpair_section();
};

#endif // TESTPNGTOKEN_H
