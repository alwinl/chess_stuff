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

#ifndef FILENAMECHOOSER_H
#define FILENAMECHOOSER_H

#include <string>

class ChessGame;

/** \brief Encapsulation of filename
 *
 * This class is the interface to filename encapsulation
 * The idea is that an implementation class actually interacts
 * with either a user (through a GUI or a CLI) or a system
 * to manipulate the filename entity.
 */
class GameLoader
{
public:
	GameLoader() {};
	virtual ~GameLoader() {};

	bool new_file();
	bool load_file( ChessGame& game );
	bool save_file( ChessGame& game );
	bool save_file_as( ChessGame& game );

protected:
	virtual std::string get_load_name() = 0;
	virtual std::string get_save_name() = 0;

private:
	std::string filename;
};

#endif // FILENAMECHOOSER_H
