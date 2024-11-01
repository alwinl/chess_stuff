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

add_executable(
	square_convertor_test

	test_runner.cc
	test_square_convertorengine.cc
)

target_include_directories( square_convertor_test PRIVATE ${CPPUNIT_INCLUDE_DIRS} ../logicsrc )
target_link_libraries( square_convertor_test ${CPPUNIT_LIBRARIES} square_convertor_logic )

# add_test( TestGameObject::create_object test_chess TestGameObject::create_object )
# add_test( TestGameObject::save_load test_chess TestGameObject::save_load )