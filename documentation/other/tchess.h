#if !defined(__tchess_h)              // Sentry, use file only if it's not already included.
#define __tchess_h

/*  Project chss
    Naivasha Enterprises
    Copyright © 1997. All Rights Reserved.

    SUBSYSTEM:    chss.exe Application
    FILE:         tchess.h
    AUTHOR:       A.J. Leerling


    OVERVIEW
    ========
    Class definition for TChess (TApplication).      
*/


#include <owl\owlpch.h>
#pragma hdrstop


#include "tchess.rh"            // Definition of all resources.


//
// FrameWindow must be derived to override Paint for Preview and Print.
//
//{{TDecoratedFrame = SDIDecFrame}}
class SDIDecFrame : public TDecoratedFrame {
public:
    SDIDecFrame (TWindow *parent, const char far *title, TWindow *clientWnd, bool trackMenuSelection = false, TModule *module = 0);
    ~SDIDecFrame ();
};    //{{SDIDecFrame}}


//{{TApplication = TChess}}
class TChess : public TApplication {
private:

private:
    void SetupSpeedBar (TDecoratedFrame *frame);

public:
    TChess ();
    virtual ~TChess ();

    void CreateGadgets (TControlBar *cb, bool server = false);

//{{TChessVIRTUAL_BEGIN}}
public:
    virtual void InitMainWindow ();
//{{TChessVIRTUAL_END}}

//{{TChessRSP_TBL_BEGIN}}
protected:
    void EvNewView (TView& view);
    void EvCloseView (TView& view);
    void CmHelpAbout ();
//{{TChessRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TChess);
};    //{{TChess}}


#endif                                      // __tchess_h sentry.
