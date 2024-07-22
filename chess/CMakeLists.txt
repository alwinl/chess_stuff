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

find_package(gtkmm-3.0)
find_package(cppunit)

add_executable( chess
	guisrc/chesscontroller.cc
	guisrc/chessboard.cc
	guisrc/dialogs.cc
	${CMAKE_CURRENT_BINARY_DIR}/guisrc/resources.c

	logicsrc/chessengine.cc
	logicsrc/chessgame.cc
	logicsrc/gamestate.cc
	logicsrc/piece.cc
	logicsrc/piece_square_tables.cc
	logicsrc/ply.cc
)

find_program( GLIB_COMPILE_RESOURCES NAMES glib-compile-resources REQUIRED )

set(GRESOURCE_C   guisrc/resources.c )
set(GRESOURCE_XML guisrc/chess.gresource.xml)

add_custom_command(
    OUTPUT ${GRESOURCE_C}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND ${GLIB_COMPILE_RESOURCES}
    ARGS
        --target=${CMAKE_CURRENT_BINARY_DIR}/${GRESOURCE_C}
		--generate-source
        ${GRESOURCE_XML}
    VERBATIM
    MAIN_DEPENDENCY ${GRESOURCE_XML}
    DEPENDS
        guisrc/appui.glade
)

add_custom_target( chess-resource DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${GRESOURCE_C} )
set_source_files_properties( ${CMAKE_CURRENT_BINARY_DIR}/${GRESOURCE_C} PROPERTIES GENERATED TRUE )
add_dependencies( chess chess-resource )


target_include_directories( chess PRIVATE ${GTKMM_INCLUDE_DIRS} guisrc logicsrc )
target_link_libraries( chess ${GTKMM_LIBRARIES} )

target_compile_definitions( chess PRIVATE GTKMM_DISABLE_DEPRECATED GDKMM_DISABLE_DEPRECATED GLIBMM_DISABLE_DEPRECATED GIOMM_DISABLE_DEPRECATED)


add_executable( test_chess
	logictest/test_runner.cc
	logictest/testgameobject.cc

	logicsrc/chessgame.cc
	logicsrc/piece.cc
	logicsrc/ply.cc
	logicsrc/gamestate.cc
	logicsrc/piece_square_tables.cc
)

target_include_directories( test_chess PRIVATE ${CPPUNIT_INCLUDE_DIRS} logictest logicsrc )
target_link_libraries( test_chess ${CPPUNIT_LIBRARIES} )

add_test( TestGameObject::create_object test_chess TestGameObject::create_object )
add_test( TestGameObject::save_load test_chess TestGameObject::save_load )