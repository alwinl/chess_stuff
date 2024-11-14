/*
 * square_convertor Copyright 2022 Alwin Leerling dna.leerling@gmail.com
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
 */

#pragma once

#include <gtkmm.h>

class square_convertorEngine;

class square_convertorController : public Gtk::Application
{
public:
	square_convertorController();

private:
	void on_activate() override;

	bool on_action_quit();
	void on_action_about();

	Gtk::Label *lblSAN;
	Gtk::Label *lblUint;
	Gtk::Entry *fldSAN;
	Gtk::Entry *fldUint;

	std::unique_ptr<square_convertorEngine> engine;
	std::unique_ptr<Gtk::AboutDialog> about_dlg;
};
