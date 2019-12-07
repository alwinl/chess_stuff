/*
 * Copyright 2019 Alwin Leerling <dna.leerling@gmail.com>
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

#ifndef TESTFILENAMECHOOSER_H
#define TESTFILENAMECHOOSER_H

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class TestFilenameChooser : public CppUnit::TestFixture
{
public:
    TestFilenameChooser() {};
    virtual ~TestFilenameChooser() {};

	CPPUNIT_TEST_SUITE( TestFilenameChooser );

	CPPUNIT_TEST( test_new_file );
	CPPUNIT_TEST( test_load_file );
	CPPUNIT_TEST( test_save_file );
	CPPUNIT_TEST( test_save_file_as );
	CPPUNIT_TEST( test_save_file_empty );
	CPPUNIT_TEST( test_load_file_empty );

	CPPUNIT_TEST_SUITE_END();

private:
	void test_new_file();
	void test_load_file();
	void test_save_file();
	void test_save_file_as();
	void test_save_file_empty();
	void test_load_file_empty();
};

#endif // TESTFILENAMECHOOSER_H
