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

#ifndef _INFO_H
#define _INFO_H

#include <static.h>
#include <filedial.h>
#include <inputdia.h>
#include <string.h>

_CLASSDEF( TNoEraseBkGndStatic )

class TNoEraseBkGndStatic : public TStatic
{
public:
    TNoEraseBkGndStatic( PTWindows AParent, int AnId, LPSTR ATitle, int X,
                         int Y, int W, int H, WORD ATextLen, PTModule AModule = NULL );

    void SetText( LPSTR text );

private:
    virtual void WMEraseBkGnd( RTMessage msg ) = [ WM_FIRST + WM_ERASEBKGND ];

    bool    DoErase;
    int     LastLen;
};

_CLASSDEF( TInfoWindow )

class TInfoWindow: public TWindow
{
public:
    TInfoWindow( PTWindowsObject AParent, LPSTR ATitle );

    void SetTurnText( LPSTR text ) { Color->SetText( text ); }
    void SetWhiteInfoText( LPSTR text ) { WhiteInfo->SetText( text ); }
    void SetBlackInfoText( LPSTR text ) { BlackInfo->SetText( text ); }
    void SetTimerText( LPSTR text ) { Timer->SetText( text ); }
    void SetLevelText( LPSTR text ) { txtLevel->SetText( text ); }
    void SetIterationText( LPSTR text ) { Iteration->SetText( text ); }
    void SetValueText( LPSTR text ) { Value->SetText( text ); }
    void SetNodeText( LPSTR text ) { txtNodes->SetText( text ); }
    void SetSecondsText( LPSTR text ) { Seconds->SetText( text ); }
    void SetDepthText( LPSTR text ) { Depth->SetText( text ); }
    void SetBestLineText( LPSTR text ) { BestLine->SetText( text ); }
    void SetMessageText( LPSTR text ) { txtMessage->SetText( text ); }

    void Reset( void );

protected:
    virtual void Paint( HDC PaintDC, PAINTSTRUCT _FAR &PaintInfo );
    virtual void WMControlColor( TMessage & Msg ) = [WM_FIRST + WM_CTLCOLOR];

    PTStatic                Color;
    PTStatic                WhiteInfo;
    PTStatic                BlackInfo;
    PTNoEraseBkGndStatic    Timer;
    PTStatic                txtLevel;
    PTStatic                Iteration;
    PTStatic                Value;
    PTStatic                txtNodes;
    PTStatic                Seconds;
    PTNoEraseBkGndStatic    Depth;
    PTStatic                BestLine;
    PTStatic                txtMessage;
    RECT                    InfoRect;

    void DrawSFrame( HDC, RECT * );
    void DrawStaticFrames( HDC );

    void IterReset( void );
};

#endif // _INFO_H
