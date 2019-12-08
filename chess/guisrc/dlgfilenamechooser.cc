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

/**-----------------------------------------------------------------------------
 * \brief
 *
 * \return std::string
 */
std::string GUIFilenameChooser::get_load_name()
{
	Gtk::FileChooserDialog * dlg = new Gtk::FileChooserDialog( parent, "Restore chess game", Gtk::FILE_CHOOSER_ACTION_OPEN );

	dlg->add_button( Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL );
	dlg->add_button( Gtk::Stock::OPEN, Gtk::RESPONSE_OK );

	Glib::RefPtr<Gtk::FileFilter> filter_chess = Gtk::FileFilter::create();
	filter_chess->set_name( "Chess Files" );
	filter_chess->add_pattern( "*.chess" );
	dlg->add_filter( filter_chess );

	Glib::RefPtr<Gtk::FileFilter> filter_all = Gtk::FileFilter::create();
	filter_all->set_name( "All Files" );
	filter_all->add_pattern( "*.*" );
	dlg->add_filter( filter_all );

	std::string ret;

	if( dlg->run() == Gtk::RESPONSE_OK )
		ret = dlg->get_filename( );

	delete dlg;

	return ret;
}

/**-----------------------------------------------------------------------------
 * \brief
 *
 * \return std::string
 */
std::string GUIFilenameChooser::get_save_name()
{
    Gtk::FileChooserDialog * dlg = new Gtk::FileChooserDialog( parent, "Save chess game", Gtk::FILE_CHOOSER_ACTION_SAVE );

	dlg->add_button( Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL );
	dlg->add_button( Gtk::Stock::SAVE, Gtk::RESPONSE_OK );

	Glib::RefPtr<Gtk::FileFilter> filter_chess = Gtk::FileFilter::create();
	filter_chess->set_name( "Chess Files" );
	filter_chess->add_pattern( "*.chess" );
	dlg->add_filter( filter_chess );

	std::string ret;

	if( dlg->run() == Gtk::RESPONSE_OK )
		ret = dlg->get_filename( );

	delete dlg;

	return ret;
}
