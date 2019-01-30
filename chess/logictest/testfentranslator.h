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

#ifndef TESTFENTRANSLATOR_H
#define TESTFENTRANSLATOR_H

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class TestFENTranslator : public CppUnit::TestFixture
{
public:
    TestFENTranslator();
    virtual ~TestFENTranslator();

	CPPUNIT_TEST_SUITE( TestFENTranslator );

	CPPUNIT_TEST( FENstring_test );
	CPPUNIT_TEST( query_test );
	CPPUNIT_TEST( blank_test );
	CPPUNIT_TEST( remove_test );
	CPPUNIT_TEST( add_test );
	CPPUNIT_TEST( invalid_add_test );
	CPPUNIT_TEST( invalid_remove_test );

	CPPUNIT_TEST_SUITE_END();

private:
    void FENstring_test();
    void query_test();
    void blank_test();
    void remove_test();
    void add_test();
    void invalid_add_test();
    void invalid_remove_test();
};

#endif // TESTFENTRANSLATOR_H
