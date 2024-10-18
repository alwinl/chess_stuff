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

find_package(PkgConfig REQUIRED)
pkg_check_modules(CPPUNIT REQUIRED cppunit)

file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/game.pgn DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )

include_directories( ${CPPUNIT_INCLUDE_DIRS} )
link_directories( ${CPPUNIT_LIBRARY_DIRS} )
link_libraries( ${CPPUNIT_LIBRARIES} )

add_executable( clic_test
	testclic.cc
	testgameobject.cc
	testboard.cc
	testboard.h
	testevaluation.cc
	testevaluation.h
	testobjectsizes.cc
	testobjectsizes.h
)

target_include_directories( clic_test PUBLIC ../logicsrc )
target_link_libraries( clic_test PUBLIC clic_logic )

add_test( TestBoard::read_write_FEN clic_test TestBoard::read_write_FEN )
add_test( TestBoard::test_pawn_first_move clic_test TestBoard::test_pawn_first_move )
add_test( TestBoard::test_pawn_capture clic_test TestBoard::test_pawn_capture )
add_test( TestBoard::test_pawn_promotion clic_test TestBoard::test_pawn_promotion )
add_test( TestBoard::test_promo_match clic_test TestBoard::test_promo_match )

add_test( TestEvaluation::piece_scores_depend_on_color clic_test TestEvaluation::piece_scores_depend_on_color )
add_test( TestEvaluation::test_square_parsing clic_test TestEvaluation::test_square_parsing )
add_test( TestEvaluation::square_table_values clic_test TestEvaluation::square_table_values )
add_test( TestEvaluation::test_queen_asymmetry clic_test TestEvaluation::test_queen_asymmetry )
add_test( TestEvaluation::standard_board_eval_is_zero clic_test TestEvaluation::standard_board_eval_is_zero )
add_test( TestEvaluation::check_all_first_moves clic_test TestEvaluation::check_all_first_moves )
add_test( TestEvaluation::test_alpha_beta clic_test TestEvaluation::test_alpha_beta )
add_test( TestEvaluation::queen_should_not_capture_rook clic_test TestEvaluation::queen_should_not_capture_rook )

add_test( TestObjectSizes::Ply_size_is_32_bit clic_test TestObjectSizes::Ply_size_is_32_bit )
add_test( TestObjectSizes::Piece_size_is_8_bit clic_test TestObjectSizes::Piece_size_is_8_bit )
add_test( TestObjectSizes::Board_size_is_64_plus_8_bytes clic_test TestObjectSizes::Board_size_is_64_plus_8_bytes )

add_test( TestGameObject::create_object clic_test TestGameObject::create_object )
add_test( TestGameObject::save_load clic_test TestGameObject::save_load )