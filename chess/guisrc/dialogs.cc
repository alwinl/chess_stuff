/*
 * Copyright 2022 Alwin Leerling <dna.leerling@gmail.com>
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

#include "dialogs.h"

DialogColours::DialogColours( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model, Gtk::Window& parent )
    : Gtk::Dialog( cobject )
{
    set_transient_for( parent );

	ui_model->get_widget( "btnBackgroundColour", btnBackground );
	ui_model->get_widget( "btnForegroundColour", btnForeground );
	ui_model->get_widget( "btnWhiteColour", btnWhiteColour );
	ui_model->get_widget( "btnBlackColour", btnBlackColour );
}

void DialogColours::set_colours( std::array<std::string,4>  colours )
{
    btnBackground->set_rgba( Gdk::RGBA(colours[0]) );
    btnForeground->set_rgba( Gdk::RGBA(colours[1]) );
    btnWhiteColour->set_rgba( Gdk::RGBA(colours[2]) );
    btnBlackColour->set_rgba( Gdk::RGBA(colours[3]) );
}

std::array<std::string,4>  DialogColours::get_colours( )
{
	std::array<std::string,4> colors = {
		btnBackground->get_rgba().to_string(),
		btnForeground->get_rgba().to_string(),
		btnWhiteColour->get_rgba().to_string(),
		btnBlackColour->get_rgba().to_string(),
	};

    return colors;
}

DialogPieceValues::DialogPieceValues( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model, Gtk::Window& parent )
    : Gtk::Dialog( cobject )
{
    set_transient_for( parent );

	ui_model->get_widget( "spnQueen",  new_piece_data[0].spnButton );
	ui_model->get_widget( "spnRook",   new_piece_data[1].spnButton );
	ui_model->get_widget( "spnBishop", new_piece_data[2].spnButton );
	ui_model->get_widget( "spnKnight", new_piece_data[3].spnButton );
	ui_model->get_widget( "spnPawn",   new_piece_data[4].spnButton );

    Gtk::Button * btnRevert;
    ui_model->get_widget( "btnRevert", btnRevert );
    btnRevert->signal_clicked().connect( sigc::mem_fun( *this, &DialogPieceValues::on_revert_clicked ));
}

void DialogPieceValues::set_values( std::map<char, int> values )
{
	new_piece_data[0].spnButton->set_value( values.at('Q') >> 4 );
	new_piece_data[1].spnButton->set_value( values.at('R') >> 4 );
	new_piece_data[2].spnButton->set_value( values.at('B') >> 4 );
	new_piece_data[3].spnButton->set_value( values.at('N') >> 4 );
	new_piece_data[4].spnButton->set_value( values.at('P') >> 4 );

	new_piece_data[0].orig_value = values.at('Q') >> 4;
	new_piece_data[1].orig_value = values.at('R') >> 4;
	new_piece_data[2].orig_value = values.at('B') >> 4;
	new_piece_data[3].orig_value = values.at('N') >> 4;
	new_piece_data[4].orig_value = values.at('P') >> 4;
}

std::map<char, int> DialogPieceValues::get_values()
{
	std::map<char, int> result = {
		{ 'Q', int( new_piece_data[0].spnButton->get_value() ) << 4 },
		{ 'R', int( new_piece_data[1].spnButton->get_value() ) << 4 },
		{ 'B', int( new_piece_data[2].spnButton->get_value() ) << 4 },
		{ 'N', int( new_piece_data[3].spnButton->get_value() ) << 4 },
		{ 'P', int( new_piece_data[4].spnButton->get_value() ) << 4 },
		{ 'K', 0 },
		{ 'q', -(int( new_piece_data[0].spnButton->get_value() ) << 4) },
		{ 'r', -(int( new_piece_data[1].spnButton->get_value() ) << 4) },
		{ 'b', -(int( new_piece_data[2].spnButton->get_value() ) << 4) },
		{ 'n', -(int( new_piece_data[3].spnButton->get_value() ) << 4) },
		{ 'p', -(int( new_piece_data[4].spnButton->get_value() ) << 4) },
		{ 'k', 0 },
	};

	return result;
}

void DialogPieceValues::on_revert_clicked()
{
    for( unsigned int idx = 0; idx < 5; ++idx )
		new_piece_data[idx].spnButton->set_value( new_piece_data[idx].orig_value );
}

DialogInput::DialogInput( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model, Gtk::Window& parent )
    : Gtk::Dialog( cobject )
{
    set_transient_for( parent );

	ui_model->get_widget( "lblInputPrompt", lblPrompt );
	ui_model->get_widget( "txtInputEntry", txtEntry );
}

void DialogInput::dlg_setup( std::string title, std::string prompt, int value )
{
    set_title( title );
    lblPrompt->set_text( prompt );
    txtEntry->set_text( std::to_string( value ) );
}

int DialogInput::get_input()
{
    return std::stoi( txtEntry->get_text() );
}