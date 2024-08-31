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

#ifndef TESTOBJECTSIZES_H
#define TESTOBJECTSIZES_H

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class TestObjectSizes : public CppUnit::TestFixture
{
public:
	CPPUNIT_TEST_SUITE( TestObjectSizes );

	CPPUNIT_TEST( Ply_size_is_32_bit );
	CPPUNIT_TEST( Piece_size_is_8_bit );
	CPPUNIT_TEST( Board_size_is_64_plus_8_bytes );

	CPPUNIT_TEST_SUITE_END();

private:
	void Ply_size_is_32_bit();
	void Piece_size_is_8_bit();
	void Board_size_is_64_plus_8_bytes();
};

#endif // TESTOBJECTSIZES_H