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

/** \brief
 */
class DialogPieceValues : public Gtk::Dialog
{
public:
	enum ePieceIdx { QUEEN, ROOK, BISHOP, KNIGHT, PAWN, PIECECOUNT };

    DialogPieceValues( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model, Gtk::Window& parent );

	void set_widgets( PieceValues::STPieceValues values );
	PieceValues::STPieceValues get_widgets( );

private:
    void on_revert_clicked();

	struct PieceData {
		Gtk::SpinButton * spnButton;
		int orig_value;
	};

	PieceData piece_data[PIECECOUNT];
	static std::string spinNames[PIECECOUNT];
};

std::string DialogPieceValues::spinNames[] = { "spnQueen", "spnRook", "spnBishop", "spnKnight", "spnPawn" };

/** \brief
 *
 * \param cobject BaseObjectType*
 * \param ui_model const Glib::RefPtr<Gtk::Builder>&
 * \param parent Gtk::Window&
 *
 */
DialogPieceValues::DialogPieceValues( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model, Gtk::Window& parent )
    : Gtk::Dialog( cobject )
{
    set_transient_for( parent );

    for( int idx = 0; idx < PIECECOUNT; ++idx )
		ui_model->get_widget( spinNames[idx], piece_data[idx].spnButton );

    Gtk::Button * btnRevert;
    ui_model->get_widget( "btnRevert", btnRevert );
    btnRevert->signal_clicked().connect( sigc::mem_fun( *this, &DialogPieceValues::on_revert_clicked ));
}

void DialogPieceValues::set_widgets( PieceValues::STPieceValues values )
{
	piece_data[QUEEN].spnButton->set_value( values.QueenValue );
	piece_data[ROOK].spnButton->set_value( values.RookValue );
	piece_data[BISHOP].spnButton->set_value( values.BishopValue );
	piece_data[KNIGHT].spnButton->set_value( values.KnightValue );
	piece_data[PAWN].spnButton->set_value( values.PawnValue );

	piece_data[QUEEN].orig_value = values.QueenValue;
	piece_data[ROOK].orig_value = values.RookValue;
	piece_data[BISHOP].orig_value = values.BishopValue;
	piece_data[KNIGHT].orig_value = values.KnightValue;
	piece_data[PAWN].orig_value = values.PawnValue;
}

PieceValues::STPieceValues DialogPieceValues::get_widgets( )
{
	PieceValues::STPieceValues values;

    values.QueenValue = int( piece_data[QUEEN].spnButton->get_value() );
    values.RookValue = int( piece_data[ROOK].spnButton->get_value() );
    values.BishopValue = int( piece_data[BISHOP].spnButton->get_value() );
    values.KnightValue = int( piece_data[KNIGHT].spnButton->get_value() );
    values.PawnValue = int( piece_data[PAWN].spnButton->get_value() );

	return values;
}

void DialogPieceValues::on_revert_clicked()
{
    for( int idx = 0; idx < PIECECOUNT; ++idx )
		piece_data[idx].spnButton->set_value( piece_data[idx].orig_value );
}

/** \brief
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
    dlg->set_widgets( get_piece_values() );
}

bool GUIPieceValues::manipulate()
{
    int response = dlg->run();

    dlg->hide();

    return response == Gtk::RESPONSE_OK;
}

PieceValues::STPieceValues GUIPieceValues::result( )
{
	return dlg->get_widgets( );
}
