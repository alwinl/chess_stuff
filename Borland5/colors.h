// ObjectWindows - (C) Copyright 1992 by Borland International

#ifndef __COLORS_H
#define __COLORS_H

#define CL_KILLFOCUS    (WM_USER + 101)

_CLASSDEF(TColorsDialog)

class TColorsDialog : public TDialog
{
public:
    TColorsDialog( PTWindowsObject AParent, LPSTR AName );
    ~TColorsDialog();

    virtual void SetupWindow();
    virtual void WMControlColor( RTMessage /*unused*/ ) = [ WM_FIRST + WM_CTLCOLOR ];
    virtual void WMVScroll( RTMessage /*unused*/ );
    virtual void WMCommand( RTMessage /*unused*/ );
    virtual void WMDrawItem( RTMessage /*unused*/ ) = [ WM_FIRST + WM_DRAWITEM ];
    virtual void Ok( RTMessage /*unused*/ );
    virtual void CLKillFocus( RTMessage /*unused*/ ) = [ WM_FIRST + CL_KILLFOCUS ];

private:
    enum ScrollBarColors { Red, Green, Blue };

    HBRUSH hWStatic, hBStatic, hSBBrush[3];
    int RWID, GWID, BWID, RBID, GBID, BBID;
    int WStatic, BStatic;
    HWND BlackSq, WhiteSq;
    BYTE WSqColors[3];
    BYTE BSqColors[3];

    void CLSetFocus(HWND);
    BOOL GetColorValue(WORD);
};

#endif // __COLORS_H
