#if !defined(__tedtbrwn_h)              // Sentry, use file only if it's not already included.
#define __tedtbrwn_h

/*  Project chess
    Naivasha Enterprises
    Copyright © 1995. All Rights Reserved.

    SUBSYSTEM:    chess.apx Application
    FILE:         tedtbrwn.h
    AUTHOR:       A.J. Leerling


    OVERVIEW
    ========
    Class definition for TEditBarWindow (TWindow).      
*/

#include <owl\owlpch.h>
#pragma hdrstop


#include "chessapp.rh"            // Definition of all resources.


//{{TWindow = TEditBarWindow}}
class TEditBarWindow : public TWindow {
public:
    TEditBarWindow (TWindow* parent, const char far* title = 0, TModule* module = 0);
    virtual ~TEditBarWindow ();
};    //{{TEditBarWindow}}

#endif                                      // __tedtbrwn_h sentry.

