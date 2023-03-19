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

#ifndef TESTEVALUATION_H
#define TESTEVALUATION_H

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class TestEvaluation : public CppUnit::TestFixture
{
public:
	CPPUNIT_TEST_SUITE( TestEvaluation );

	CPPUNIT_TEST( piece_scores_depend_on_color );
	CPPUNIT_TEST( test_square_parsing );
    CPPUNIT_TEST( square_table_values );
    CPPUNIT_TEST( test_queen_asymmetry );
    CPPUNIT_TEST( standard_board_eval_is_zero );
    CPPUNIT_TEST( check_all_first_moves );
    CPPUNIT_TEST( test_alpha_beta );
    CPPUNIT_TEST( queen_should_not_capture_rook );

	CPPUNIT_TEST_SUITE_END();

private:
	void piece_scores_depend_on_color();
	void test_square_parsing();
	void square_table_values();
	void test_queen_asymmetry();
	void standard_board_eval_is_zero();
	void check_all_first_moves();
	void test_alpha_beta();
	void queen_should_not_capture_rook();
};

#endif // TESTEVALUATION_H
