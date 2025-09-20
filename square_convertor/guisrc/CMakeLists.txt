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
pkg_check_modules( GTKMM REQUIRED gtkmm-4.0)

SET( LIB_SETUP square_convertor_setup )

# define a setup with all the crud that needs to be added to the gcc call
add_library( ${LIB_SETUP} INTERFACE )
target_include_directories( ${LIB_SETUP} INTERFACE ${GTKMM_INCLUDE_DIRS} )
target_compile_options( ${LIB_SETUP} INTERFACE ${GTKMM_CFLAGS_OTHER} )
target_link_directories( ${LIB_SETUP} INTERFACE ${GTKMM_LIBRARY_DIRS} )
target_link_libraries( ${LIB_SETUP}  INTERFACE ${GTKMM_LIBRARIES} )

#bluprint stuff
find_program(BLUEPRINT_COMPILER blueprint-compiler REQUIRED)

set(BLP_FILE ${CMAKE_CURRENT_SOURCE_DIR}/square_convertor.blp)
set(UI_FILE ${CMAKE_CURRENT_BINARY_DIR}/square_convertor.ui)

add_custom_command(
    OUTPUT ${UI_FILE}
    COMMAND ${BLUEPRINT_COMPILER} compile --output ${UI_FILE} ${BLP_FILE}
    DEPENDS ${BLP_FILE}
    COMMENT "Compiling Blueprint UI"
)

add_custom_target(blueprint_ui ALL DEPENDS ${UI_FILE})

add_executable(
	square_convertor

	main.cc
	controller.cc
	view.cc

	${UI_FILE}
)

add_dependencies( square_convertor blueprint_ui )

target_link_libraries( square_convertor PRIVATE ${LIB_SETUP} square_convertor_logic )
