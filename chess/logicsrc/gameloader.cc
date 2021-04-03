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

#include "gameloader.h"

bool GameLoader::new_file()
{
	// Check if dirty and ask to save

	filename.clear();
	return true;
}

bool GameLoader::load_file( ChessGame& game )
{
	// Check if dirty and ask to save

	filename = get_load_name();

#if 0
    if( open_name.empty() )
        return false;

    if( model->load_game( open_name ) == -1 ) {    // load the file and build the DS in the model_ member
        return false;
    }

    return true;
#endif

	return true;
}

bool GameLoader::save_file( ChessGame& game )
{
	if( filename.empty() )
		filename = get_save_name();

	if( ! filename.empty() )
		if( filename.find( ".chess") == std::string::npos )     // no .chess added
			filename += std::string(".chess");

	return true;
}

bool GameLoader::save_file_as( ChessGame& game )
{
	filename = get_save_name();

	if( ! filename.empty() )
		if( filename.find( ".chess") == std::string::npos )     // no .chess added
			filename += std::string(".chess");

	return true;
}
