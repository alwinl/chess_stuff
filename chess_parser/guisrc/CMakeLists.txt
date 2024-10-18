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

add_subdirectory(resources)

find_package(PkgConfig REQUIRED)
pkg_check_modules( GTKMM REQUIRED gtkmm-3.0)

# define a setup with all the crud that needs to be added to the gcc call
add_library( main_setup INTERFACE )
target_include_directories( main_setup INTERFACE ${GTKMM_INCLUDE_DIRS} )
target_compile_options( main_setup INTERFACE ${GTKMM_CFLAGS_OTHER} )
target_link_directories( main_setup INTERFACE ${GTKMM_LIBRARY_DIRS} )
target_link_libraries( main_setup  INTERFACE ${GTKMM_LIBRARIES} )

set_source_files_properties( ${GRESOURCE_C} PROPERTIES GENERATED TRUE )

add_executable(
	chess_parser

	chessparsercontroller.cc

	${GRESOURCE_C}
)

add_dependencies( chess_parser chess-resource )

# target_include_directories( chess_parser PRIVATE ${GTKMM_INCLUDE_DIRS} guisrc ../logicsrc )
target_link_libraries( chess_parser ${GTKMM_LIBRARIES} main_setup chessparser_logic )
# target_compile_definitions( chess_parser PRIVATE GTKMM_DISABLE_DEPRECATED GDKMM_DISABLE_DEPRECATED GLIBMM_DISABLE_DEPRECATED GIOMM_DISABLE_DEPRECATED)
