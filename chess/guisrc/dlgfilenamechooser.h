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

#ifndef GUIFILENAMECHOOSER_H
#define GUIFILENAMECHOOSER_H

#include <../logicsrc/filenamechooser.h>

#include <gtkmm.h>

class GUIFilenameChooser : public FilenameChooser
{
public:
	GUIFilenameChooser( Gtk::Window& parent, Gtk::FileChooserAction action );
	virtual ~GUIFilenameChooser() { delete dlg; }

protected:
	virtual void set_working_dir( std::string working_dir );
	virtual bool query_file();
	virtual void set_filename( std::string filename );
	virtual std::string	get_filename( );

private:
	Gtk::FileChooserDialog * dlg;
};

#endif // GUIFILENAMECHOOSER_H