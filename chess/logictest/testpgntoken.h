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

#include <string>
#include <vector>

class PGNToken;

class TestPNGToken : public CppUnit::TestFixture
{
public:
	CPPUNIT_TEST_SUITE( TestPNGToken );

	CPPUNIT_TEST( check_EOFToken );
	CPPUNIT_TEST( check_EOSToken );
	CPPUNIT_TEST( check_symbols );
	CPPUNIT_TEST( check_strings );
	CPPUNIT_TEST( check_integers );
	CPPUNIT_TEST( check_self_terminators );
	CPPUNIT_TEST( check_nags );
	CPPUNIT_TEST( check_tagpair );
	CPPUNIT_TEST( check_tagpair_section );

	CPPUNIT_TEST_SUITE_END();

private:
	void check_EOFToken();
	void check_EOSToken();
	void check_symbols();
	void check_strings();
	void check_integers();
	void check_self_terminators();
	void check_nags();
	void check_tagpair();
	void check_tagpair_section();

	std::vector<PGNToken> process_string( std::string input_data );

};

#endif // TESTPNGTOKEN_H
