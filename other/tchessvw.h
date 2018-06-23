#if !defined(__tchessvw_h)              // Sentry, use file only if it's not already included.
#define __tchessvw_h

/*  Project chss
    Naivasha Enterprises
    Copyright © 1997. All Rights Reserved.

    SUBSYSTEM:    chss.exe Application
    FILE:         tchessvw.h
    AUTHOR:       A.J. Leerling


    OVERVIEW
    ========
    Class definition for TChessView (TWindowView).      
*/


#include <owl\owlpch.h>
#pragma hdrstop


#include "tchess.rh"            // Definition of all resources.


//{{TWindowView = TChessView}}class TChessView : public TWindowView {
public:
    TChessView (TDocument& doc, TWindow* parent = 0);
    virtual ~TChessView ();
};    //{{TChessView}}


#endif                                      // __tchessvw_h sentry.
