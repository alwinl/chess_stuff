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

#ifndef __PVALUE_H
#define __PVALUE_H

#define PV_KILLFOCUS     (WM_USER + 100)

_CLASSDEF( TPieceValueDialog )

class TPieceValueDialog : public TDialog
{
public:
    TPieceValueDialog( PTWindowsObject AParent, LPSTR AName );

    virtual void SetupWindow();

    virtual void Ok( RTMessage );
    virtual void WMHScroll( RTMessage );
    virtual void WMCommand( RTMessage );

    virtual void PVKillFocus( RTMessage ) = [WM_FIRST + PV_KILLFOCUS];
    virtual void IDDDefault( RTMessage ) = [ID_FIRST + IDD_DEFAULT];

private:
    enum ePIECES { pvqueen, pvrook, pvbishop, pvknight, pvpawn };

    int Values[5];
    HWND hScroller[5];
    unsigned int ScollerIDs[5];

    void PVSetFocus( HWND );
    bool GetColorValue( WORD );
};

#endif // __PVALUE_H
