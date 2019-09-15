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

#ifndef __COLORS_H
#define __COLORS_H

#define CL_KILLFOCUS    (WM_USER + 101)

_CLASSDEF( TColorsDialog )

class TColorsDialog : public TDialog
{
public:
    TColorsDialog( PTWindowsObject AParent, LPSTR AName );
    ~TColorsDialog();

    virtual void SetupWindow();
    virtual void WMControlColor( RTMessage ) = [WM_FIRST + WM_CTLCOLOR];
    virtual void WMVScroll( RTMessage );
    virtual void WMCommand( RTMessage );
    virtual void WMDrawItem( RTMessage ) = [WM_FIRST + WM_DRAWITEM];
    virtual void Ok( RTMessage );
    virtual void CLKillFocus( RTMessage ) = [WM_FIRST + CL_KILLFOCUS];
    virtual void CLSetFocus( HWND hWnd );

private:
    enum ScrollBarColors { Red, Green, Blue };

    HBRUSH hWStatic;
    HBRUSH hBStatic;
    HBRUSH hSBBrush[3];
    int RWID;
    int GWID;
    int BWID;
    int RBID;
    int GBID;
    int BBID;
    int WStatic;
    int BStatic;
    HWND BlackSq;
    HWND WhiteSq;
    char WSqColors[3];
    char BSqColors[3];

    void CLSetFocus( HWND );
    bool GetColorValue( WORD );
};

#endif // __COLORS_H
