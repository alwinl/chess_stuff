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

find_package(Doxygen)

function( add_doxygen_target config_file )
	
	set( doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/${config_file} )
	set( doxyfile_out ${CMAKE_CURRENT_BINARY_DIR}/${config_file} )

	configure_file( ${doxyfile_in} ${doxyfile_out} @ONLY )

	add_custom_target(
		${config_file}

		COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile_out}
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		COMMENT "Generating API documentation"
		VERBATIM
	)

endfunction( add_doxygen_target )


if( DOXYGEN_FOUND )

	add_doxygen_target( chess_api )
	add_doxygen_target( clic_api )

endif()
