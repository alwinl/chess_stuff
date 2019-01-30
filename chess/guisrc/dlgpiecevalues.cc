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

#include "../logicsrc/pods.h"

/** \brief
 */
class DialogPieceValues : public Gtk::Dialog
{
public:
    DialogPieceValues( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model, Gtk::Window& parent );

    void set_piece_values( STPieceValues& values );
    STPieceValues piece_values( );

private:
    Gtk::SpinButton * spnQueen;
    Gtk::SpinButton * spnRook;
    Gtk::SpinButton * spnBishop;
    Gtk::SpinButton * spnKnight;
    Gtk::SpinButton * spnPawn;
};

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

    ui_model->get_widget( "spnQueen", spnQueen );
    ui_model->get_widget( "spnRook", spnRook );
    ui_model->get_widget( "spnBishop", spnBishop );
    ui_model->get_widget( "spnKnight", spnKnight );
    ui_model->get_widget( "spnPawn", spnPawn );
}

void DialogPieceValues::set_piece_values( STPieceValues& values )
{
    spnQueen->set_value( values.QueenValue );
    spnRook->set_value( values.RookValue );
    spnBishop->set_value( values.BishopValue );
    spnKnight->set_value( values.KnightValue );
    spnPawn->set_value( values.PawnValue );
}

STPieceValues DialogPieceValues::piece_values( )
{
    STPieceValues values;

    values.QueenValue = int( spnQueen->get_value() );
    values.RookValue = int( spnRook->get_value() );
    values.BishopValue = int( spnBishop->get_value() );
    values.KnightValue = int( spnKnight->get_value() );
    values.PawnValue = int( spnPawn->get_value() );

    return values;
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
    dlg->set_piece_values( values );
}

bool GUIPieceValues::manipulate_values()
{
    int response = dlg->run();

    dlg->hide();

    return response == Gtk::RESPONSE_OK;
}

STPieceValues GUIPieceValues::piece_values( )
{
    return dlg->piece_values();
}
