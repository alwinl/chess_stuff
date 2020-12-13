/*
 * Copyright 2014 Alwin Leerling <alwin@jambo>
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

#include "dlgpiecevalues.h"

/**-----------------------------------------------------------------------------
 * \brief
 */
class DialogPieceValues : public Gtk::Dialog
{
public:
    DialogPieceValues( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model, Gtk::Window& parent );

	void set_value( PieceValues::ePieceIdx idx, int new_value );
	int get_value( PieceValues::ePieceIdx idx );

private:
    void on_revert_clicked();

	struct PieceData {
		Gtk::SpinButton * spnButton;
		int orig_value;
	};

	PieceData piece_data[PieceValues::PIECECOUNT];
	static std::string spinNames[PieceValues::PIECECOUNT];
};

std::string DialogPieceValues::spinNames[] = { "spnQueen", "spnRook", "spnBishop", "spnKnight", "spnPawn" };

DialogPieceValues::DialogPieceValues( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model, Gtk::Window& parent )
    : Gtk::Dialog( cobject )
{
    set_transient_for( parent );

    for( int idx = 0; idx < PieceValues::KING; ++idx )
		ui_model->get_widget( spinNames[idx], piece_data[idx].spnButton );

    Gtk::Button * btnRevert;
    ui_model->get_widget( "btnRevert", btnRevert );
    btnRevert->signal_clicked().connect( sigc::mem_fun( *this, &DialogPieceValues::on_revert_clicked ));
}

void DialogPieceValues::set_value( PieceValues::ePieceIdx idx, int new_value )
{
	piece_data[idx].spnButton->set_value( new_value );
	piece_data[idx].orig_value = new_value;
}

int DialogPieceValues::get_value( PieceValues::ePieceIdx idx )
{
	return int( piece_data[idx].spnButton->get_value() );
}

void DialogPieceValues::on_revert_clicked()
{
    for( int idx = 0; idx < PieceValues::KING; ++idx )
		piece_data[idx].spnButton->set_value( piece_data[idx].orig_value );
}

/**-----------------------------------------------------------------------------
 * \brief
 *
 * \param ui_model Glib::RefPtr<Gtk::Builder>&
 * \param parent Gtk::Window&
 *
 */
GUIPieceValues::GUIPieceValues( Glib::RefPtr<Gtk::Builder>& ui_model, Gtk::Window& parent ) : PieceValues()
{
    ui_model->get_widget_derived( "dlgPieceValue", dlg, parent );
}

void GUIPieceValues::setup()
{
	dlg->set_value( QUEEN,  values[QUEEN] >> 4 );
    dlg->set_value( ROOK,   values[ROOK] >> 4 );
    dlg->set_value( BISHOP, values[BISHOP] >> 4 );
    dlg->set_value( KNIGHT, values[KNIGHT] >> 4 );
    dlg->set_value( PAWN,   values[PAWN] >> 4 );
}

bool GUIPieceValues::manipulate()
{
    int response = dlg->run();

    dlg->hide();

    return response == Gtk::RESPONSE_OK;
}

void GUIPieceValues::result( )
{
    values[QUEEN]  = dlg->get_value(QUEEN ) << 4;
    values[ROOK]   = dlg->get_value(ROOK  ) << 4;
    values[BISHOP] = dlg->get_value(BISHOP) << 4;
    values[KNIGHT] = dlg->get_value(KNIGHT) << 4;
    values[PAWN]   = dlg->get_value(PAWN  ) << 4;
}
