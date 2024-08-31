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

#include "testobjectsizes.h"

#include "board.h"
#include "piece.h"
#include "ply.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TestObjectSizes );

void TestObjectSizes::Ply_size_is_32_bit()
{
	CPPUNIT_ASSERT_EQUAL( 4ul, (unsigned long)sizeof( Ply ) );
}

void TestObjectSizes::Piece_size_is_8_bit()
{
	CPPUNIT_ASSERT_EQUAL( 1ul, (unsigned long)sizeof( Piece ) );
}

void TestObjectSizes::Board_size_is_64_plus_8_bytes()
{
	CPPUNIT_ASSERT_EQUAL( 72ul, (unsigned long)sizeof( Board ) );
}