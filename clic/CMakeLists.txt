#
# CMakeLists.txt Copyright 2024 Alwin Leerling dna.leerling@gmail.com
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
# MA 02110-1301, USA.
#

add_executable( clic
	board.cc
	board.h
	chessgame.cc
	chessgame.h
	display.cc
	display.h
	main.cc
	piece.cc
	piece.h
	piece_square_tables.cc
	ply.cc
	ply.h
)

find_package( cppunit REQUIRED )

include_directories( ${CPPUNIT_INCLUDE_DIRS} )
link_directories( ${CPPUNIT_LIBRARY_DIRS} )
link_libraries( ${CPPUNIT_LIBRARIES} )

add_executable( test_runner
	board.cc
	board.h
	chessgame.cc
	chessgame.h
	display.cc
	display.h
	piece.cc
	piece.h
	piece_square_tables.cc
	ply.cc
	ply.h
	testclic.cc
	testboard.cc
	testboard.h
	testevaluation.cc
	testevaluation.h
	testobjectsizes.cc
	testobjectsizes.h
)

add_test( TestBoard::read_write_FEN test_runner TestBoard::read_write_FEN )
add_test( TestBoard::test_pawn_first_move test_runner TestBoard::test_pawn_first_move )
add_test( TestBoard::test_pawn_capture test_runner TestBoard::test_pawn_capture )
add_test( TestBoard::test_pawn_promotion test_runner TestBoard::test_pawn_promotion )

add_test( TestEvaluation::piece_scores_depend_on_color test_runner TestEvaluation::piece_scores_depend_on_color )
add_test( TestEvaluation::test_square_parsing test_runner TestEvaluation::test_square_parsing )
add_test( TestEvaluation::square_table_values test_runner TestEvaluation::square_table_values )
add_test( TestEvaluation::test_queen_asymmetry test_runner TestEvaluation::test_queen_asymmetry )
add_test( TestEvaluation::standard_board_eval_is_zero test_runner TestEvaluation::standard_board_eval_is_zero )
add_test( TestEvaluation::check_all_first_moves test_runner TestEvaluation::check_all_first_moves )
add_test( TestEvaluation::test_alpha_beta test_runner TestEvaluation::test_alpha_beta )
add_test( TestEvaluation::queen_should_not_capture_rook test_runner TestEvaluation::queen_should_not_capture_rook )

add_test( TestObjectSizes::Ply_size_is_32_bit test_runner TestObjectSizes::Ply_size_is_32_bit )
add_test( TestObjectSizes::Piece_size_is_16_bit test_runner TestObjectSizes::Piece_size_is_16_bit )
add_test( TestObjectSizes::Board_size_is_64_x_16_bit test_runner TestObjectSizes::Board_size_is_64_x_16_bit )
