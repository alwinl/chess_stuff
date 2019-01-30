// ObjectWindows - (C) Copyright 1992 by Borland International

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
