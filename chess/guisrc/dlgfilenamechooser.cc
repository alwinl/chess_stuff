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

#include "dlgfilenamechooser.h"

GUIFilenameChooser::GUIFilenameChooser( Gtk::Window& parent, Gtk::FileChooserAction action ) : FilenameChooser()
{
	std::string title = ( action == Gtk::FILE_CHOOSER_ACTION_OPEN ) ? "Restore chess game" : "Save chess game";

    dlg = new Gtk::FileChooserDialog( parent, title, action );

	dlg->add_button( Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL );
	dlg->add_button( ( action == Gtk::FILE_CHOOSER_ACTION_OPEN ) ? Gtk::Stock::OPEN : Gtk::Stock::SAVE, Gtk::RESPONSE_OK );

	Glib::RefPtr<Gtk::FileFilter> filter_chess = Gtk::FileFilter::create();
	filter_chess->set_name( "Chess Files" );
	filter_chess->add_pattern( "*.chess" );
	dlg->add_filter( filter_chess );

	if( action == Gtk::FILE_CHOOSER_ACTION_OPEN ) {
		Glib::RefPtr<Gtk::FileFilter> filter_all = Gtk::FileFilter::create();
		filter_all->set_name( "All Files" );
		filter_all->add_pattern( "*.*" );
		dlg->add_filter( filter_all );
	}
}

void GUIFilenameChooser::set_working_dir( std::string working_dir )
{
    dlg->set_current_folder( working_dir );
}

void GUIFilenameChooser::set_filename( std::string aname )
{
	if( dlg->get_action() == Gtk::FILE_CHOOSER_ACTION_SAVE )
		dlg->set_current_name( aname );
}

bool GUIFilenameChooser::query_file()
{
    int response = dlg->run();

    dlg->hide();

    return response == Gtk::RESPONSE_OK;
}

std::string	GUIFilenameChooser::result( )
{
    return dlg->get_filename( );
}
