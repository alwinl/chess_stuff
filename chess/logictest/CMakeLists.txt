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
pkg_check_modules( CPPUNIT REQUIRED cppunit)

file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/game.pgn DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )

add_executable(
	chess_test

	test_runner.cc
	testgameobject.cc
	test_attacktable_creation.cc
)

target_include_directories( chess_test PRIVATE ${CPPUNIT_INCLUDE_DIRS} ../logicsrc )
target_link_libraries( chess_test ${CPPUNIT_LIBRARIES} chess_logic )

add_test( TestGameObject::create_object chess_test TestGameObject::create_object )
add_test( TestGameObject::save_load chess_test TestGameObject::save_load )

add_test( TestAttackTableGen::do_test chess_test TestAttackTableGen::do_test )