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

namespace chess_gui {


DialogColours::DialogColours( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model )
    : Gtk::Window( cobject )
{
	Gtk::Button * button;

	ui_model->get_widget<Gtk::Button>( "btnColoursOK" )->signal_clicked().connect( [this](){ result = eResult::OK; hide(); } );
	ui_model->get_widget<Gtk::Button>( "btnColoursCancel" )->signal_clicked().connect( [this](){ result = eResult::CANCEL; hide(); } );

	btnBackground  = ui_model->get_widget<Gtk::ColorButton>( "btnBackgroundColour");
	btnForeground  = ui_model->get_widget<Gtk::ColorButton>( "btnForegroundColour");
	btnWhiteColour = ui_model->get_widget<Gtk::ColorButton>( "btnWhiteColour"     );
	btnBlackColour = ui_model->get_widget<Gtk::ColorButton>( "btnBlackColour"     );
}

void DialogColours::set_colours( ColorArray  colours )
{
	initial_colours = colours;

    btnBackground->set_rgba( Gdk::RGBA(colours[0]) );
    btnForeground->set_rgba( Gdk::RGBA(colours[1]) );
    btnWhiteColour->set_rgba( Gdk::RGBA(colours[2]) );
    btnBlackColour->set_rgba( Gdk::RGBA(colours[3]) );
}

ColorArray DialogColours::get_colours( )
{
	return ( result == eResult::OK ) ?
		ColorArray {
			btnBackground->get_rgba().to_string(),
			btnForeground->get_rgba().to_string(),
			btnWhiteColour->get_rgba().to_string(),
			btnBlackColour->get_rgba().to_string(),
		}
		: initial_colours;
}

DialogPieceValues::DialogPieceValues( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model )
    : Gtk::Window( cobject )
{
	spnButton[0]  = ui_model->get_widget<Gtk::SpinButton>( "spnQueen" );
	spnButton[1]  = ui_model->get_widget<Gtk::SpinButton>( "spnRook"  );
	spnButton[2]  = ui_model->get_widget<Gtk::SpinButton>( "spnBishop");
	spnButton[3]  = ui_model->get_widget<Gtk::SpinButton>( "spnKnight");
	spnButton[4]  = ui_model->get_widget<Gtk::SpinButton>( "spnPawn"  );

	ui_model->get_widget<Gtk::Button>( "btnPieceValuesOK" )->signal_clicked().connect( [this](){ result = eResult::OK; hide(); } );
	ui_model->get_widget<Gtk::Button>( "btnPieceValuesCancel" )->signal_clicked().connect( [this](){ result = eResult::CANCEL; hide(); } );
    ui_model->get_widget<Gtk::Button>( "btnPieceValuesRevert" )->signal_clicked().connect(
		[this](){
			spnButton[0]->set_value( orig_value[0] );
			spnButton[1]->set_value( orig_value[1] );
			spnButton[2]->set_value( orig_value[2] );
			spnButton[3]->set_value( orig_value[3] );
			spnButton[4]->set_value( orig_value[4] );
		}
	);
}

void DialogPieceValues::set_values( std::map<char, int> values )
{
	orig_value[0] = values.at('Q') >> 4;
	orig_value[1] = values.at('R') >> 4;
	orig_value[2] = values.at('B') >> 4;
	orig_value[3] = values.at('N') >> 4;
	orig_value[4] = values.at('P') >> 4;

	spnButton[0]->set_value( orig_value[0] );
	spnButton[1]->set_value( orig_value[1] );
	spnButton[2]->set_value( orig_value[2] );
	spnButton[3]->set_value( orig_value[3] );
	spnButton[4]->set_value( orig_value[4] );
}

std::map<char, int> DialogPieceValues::get_values()
{
	if( result == eResult::OK ) {
		orig_value[0] = int( spnButton[0]->get_value() ) << 4;
		orig_value[1] = int( spnButton[1]->get_value() ) << 4;
		orig_value[2] = int( spnButton[2]->get_value() ) << 4;
		orig_value[3] = int( spnButton[3]->get_value() ) << 4;
		orig_value[4] = int( spnButton[4]->get_value() ) << 4;
	}

	return {
		{ 'Q', orig_value[0] },
		{ 'R', orig_value[1] },
		{ 'B', orig_value[2] },
		{ 'N', orig_value[3] },
		{ 'P', orig_value[4] },
		{ 'K', 0 },
		{ 'q', -orig_value[0] },
		{ 'r', -orig_value[1] },
		{ 'b', -orig_value[2] },
		{ 'n', -orig_value[3] },
		{ 'p', -orig_value[4] },
		{ 'k', 0 },
	};
}

DialogInput::DialogInput( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model )
    : Gtk::Window( cobject )
{
	lblPrompt = ui_model->get_widget<Gtk::Label>( "lblInputPrompt" );
	txtEntry  = ui_model->get_widget<Gtk::Entry>( "txtInputEntry" );
}

void DialogInput::dlg_setup( std::string title, std::string prompt, int value )
{
    set_title( title );
    lblPrompt->set_text( prompt );
    txtEntry->set_text( std::to_string( value ) );
}

void DialogInput::dlg_setup( std::string title, std::string prompt, std::string value )
{
    set_title( title );
    lblPrompt->set_text( prompt );
    txtEntry->set_text( value );
}

int DialogInput::get_input()
{
    return std::stoi( txtEntry->get_text() );
}

DialogNewGame::DialogNewGame( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model )
	: Gtk::Window( cobject )
{
	Gtk::Button * button;

	button = ui_model->get_widget<Gtk::Button>( "btnHumanvsAI" );
	button->signal_clicked().connect( sigc::bind( sigc::mem_fun(*this, &DialogNewGame::make_choice), 1 ) );

	button = ui_model->get_widget<Gtk::Button>( "btnAIvsHuman" );
	button->signal_clicked().connect( sigc::bind( sigc::mem_fun(*this, &DialogNewGame::make_choice), 2 ) );

	button = ui_model->get_widget<Gtk::Button>( "btnAIvsAI" );
	button->signal_clicked().connect( sigc::bind( sigc::mem_fun(*this, &DialogNewGame::make_choice), 3 ) );

	button = ui_model->get_widget<Gtk::Button>( "btnHumanvsHuman" );
	button->signal_clicked().connect( sigc::bind( sigc::mem_fun(*this, &DialogNewGame::make_choice), 4 ) );
}

DialogLevel::DialogLevel( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model )
	: Gtk::Window( cobject )
{
	ui_model->get_widget<Gtk::Button>( "btnLevelOK" )->signal_clicked().connect( [this](){ result = eResult::OK; hide(); } );
	ui_model->get_widget<Gtk::Button>( "btnLevelCancel" )->signal_clicked().connect( [this](){ result = eResult::CANCEL; hide(); } );

	// ui_model->get_widget( "chkLevelEasy", chkLevelEasy );
	// ui_model->get_widget( "chkLevelTimed", chkLevelTimed );
	// ui_model->get_widget( "chkLevelTotalTime", chkLevelTotalTime );
	// ui_model->get_widget( "chkLevelInfinite", chkLevelInfinite );
	// ui_model->get_widget( "chkLevelPlySearch", chkLevelPlySearch );
	// ui_model->get_widget( "chkLevelMateSearch", chkLevelMateSearch );
	// ui_model->get_widget( "chkLevelMatching", chkLevelMatching );

}

}