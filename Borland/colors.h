// ObjectWindows - (C) Copyright 1992 by Borland International

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
