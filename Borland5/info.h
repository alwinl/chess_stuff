// ObjectWindows - (C) Copyright 1992 by Borland International

#ifndef _INFO_H
#define _INFO_H

#include <string.h>

_CLASSDEF(TNoEraseBkGndStatic)
class TNoEraseBkGndStatic : public TStatic
{
public:
    TNoEraseBkGndStatic( PTWindows AParent, int AnId, LPSTR ATitle, int X, int Y, int W, int H, WORD ATextLen, PTModule AModule = NULL );

    virtual void WMEraseBkGnd( RTMessage msg ) = [ WM_FIRST + WM_ERASEBKGND ];

    void SetText( LPSTR text )

private:
    bool DoErase;
    int LastLen;
};

_CLASSDEF(TInfoWindow)
class TInfoWindow: public TWindow
{
private:
    PTStatic Color;
    PTStatic WhiteInfo;
    PTStatic BlackInfo;
    PTNoEraseBkGndStatic Timer;
    PTStatic Level;
    PTStatic Iteration;
    PTStatic Value;
    PTStatic Nodes;
    PTStatic Seconds;
    PTNoEraseBkGndStatic Depth;
    PTStatic BestLine;
    PTStatic Message;
    RECT InfoRect;

    void DrawSFrame( HDC, RECT * );
    void DrawStaticFrames( HDC );

public:
    TInfoWindow(PTWindowsObject AParent, LPSTR ATitle);

    void SetTurnText( LPSTR text ) { Color->SetText( text ); }
    void SetWhiteInfoText( LPSTR text ) { WhiteInfo->SetText( text ); }
    void SetBlackInfoText( LPSTR text ) { BlackInfo->SetText( text ); }
    void SetTimerText( LPSTR text ) { Timer->SetText( text ); }
    void SetLevelText( LPSTR text ) { Level->SetText( text ); }
    void SetIterationText( LPSTR text ) { Iteration->SetText( text ); }
    void SetValueText( LPSTR text ) { Value->SetText( text ); }
    void SetNodeText( LPSTR text ) { Nodes->SetText( text ); }
    void SetSecondsText( LPSTR text ) { Seconds->SetText( text ); }
    void SetDepthText( LPSTR text ) { Depth->SetText( text ); }
    void SetBestLineText( LPSTR text ) { BestLine->SetText( text ); }
    void SetMessageText( LPSTR text ) { Message->SetText( text ); }

    virtual void Paint( HDC PaintDC, PAINTSTRUCT _FAR & PaintInfo );
    virtual void WMControlColor( RTMessage Msg ) = [ WM_FIRST + WM_CTLCOLOR ];

    void DrawInfoFrame(HDC);
    void Reset();
    void IterReset();
};

#endif // _INFO_H
