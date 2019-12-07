/*
 * Copyright 2017 Alwin Leerling <alwin@jambo>
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
 *
 *
 */

#include "filenamechooser.h"

bool FilenameChooser::new_file()
{
	// Check if dirty and ask to save

	filename.clear();
	return true;
}

std::string FilenameChooser::load_file()
{
	// Check if dirty and ask to save

	filename = get_load_name();

	return filename;
}

std::string FilenameChooser::save_file()
{
	if( filename.empty() )
		filename = get_save_name();

	if( ! filename.empty() )
		if( filename.find( ".chess") == std::string::npos )     // no .chess added
			filename += std::string(".chess");

	return filename;
}

std::string FilenameChooser::save_file_as()
{
	filename = get_save_name();

	if( ! filename.empty() )
		if( filename.find( ".chess") == std::string::npos )     // no .chess added
			filename += std::string(".chess");

	return filename;
}
