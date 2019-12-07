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

#include <vector>

/** \brief
 */
class DialogPieceValues : public Gtk::Dialog
{
public:
	enum ePieceIdx { QUEEN, ROOK, BISHOP, KNIGHT, PAWN, PIECECOUNT };

    DialogPieceValues( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model, Gtk::Window& parent );

	void set_piece_values( std::vector<int> new_values );
	std::vector<int> piece_values( );

private:
    void on_revert_clicked();

	static std::string spinNames[PIECECOUNT];
    Gtk::SpinButton * spnBtns[PIECECOUNT];
    std::vector<int> orig_values;
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
		ui_model->get_widget( spinNames[idx], spnBtns[idx] );

    Gtk::Button * btnRevert;
    ui_model->get_widget( "btnRevert", btnRevert );
    btnRevert->signal_clicked().connect( sigc::mem_fun( *this, &DialogPieceValues::on_revert_clicked ));
}

void DialogPieceValues::set_piece_values( std::vector<int> new_values )
{
	orig_values.clear();

    for( int idx = 0; idx < PIECECOUNT; ++idx ) {
		spnBtns[idx]->set_value( new_values[idx] );
		orig_values.push_back( new_values[idx] );
    }
}

std::vector<int> DialogPieceValues::piece_values( )
{
	std::vector<int> new_values;

    for( int idx = 0; idx < PIECECOUNT; ++idx )
		new_values.push_back( int( spnBtns[idx]->get_value() ) );

	return new_values;
}

void DialogPieceValues::on_revert_clicked()
{
    for( int idx = 0; idx < PIECECOUNT; ++idx )
		spnBtns[idx]->set_value( orig_values[idx] );
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

void GUIPieceValues::set_values( STPieceValues& values )
{
	std::vector<int> new_values = { values.QueenValue, values.RookValue, values.BishopValue, values.KnightValue, values.PawnValue };

    dlg->set_piece_values( new_values );
}

bool GUIPieceValues::manipulate_values()
{
    int response = dlg->run();

    dlg->hide();

    return response == Gtk::RESPONSE_OK;
}

STPieceValues GUIPieceValues::piece_values( )
{
	STPieceValues values;
	std::vector<int> new_values;

	new_values = dlg->piece_values( );

    values.QueenValue = new_values[DialogPieceValues::QUEEN];
    values.RookValue = new_values[DialogPieceValues::ROOK];
    values.BishopValue = new_values[DialogPieceValues::BISHOP];
    values.KnightValue = new_values[DialogPieceValues::KNIGHT];
    values.PawnValue = new_values[DialogPieceValues::PAWN];

    return values;
}
