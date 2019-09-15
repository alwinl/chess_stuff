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
#include <utility>

class FilenameChooser
{
public:
	FilenameChooser() {};
	virtual ~FilenameChooser() {};

    std::pair<bool,std::string> get_filename( std::string aname, std::string working_dir );

protected:
	virtual void set_working_dir( std::string working_dir ) = 0;
	virtual void set_filename( std::string aname ) = 0;
	virtual bool query_file() = 0;
	virtual std::string	get_filename( ) = 0;
};

#endif // FILENAMECHOOSER_H
