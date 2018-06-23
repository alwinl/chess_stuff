#if !defined(__chssendd_h)              // Sentry, use file only if it's not already included.
#define __chssendd_h

/*  Project chess
    Naivasha Enterprises
    Copyright © 1995. All Rights Reserved.

    SUBSYSTEM:    chess.apx Application
    FILE:         chssendd.h
    AUTHOR:       A.J. Leerling


    OVERVIEW
    ========
    Class definition for chessEndDlg (TDialog).      
*/

#include <owl\owlpch.h>
#pragma hdrstop


#include "chessapp.rh"            // Definition of all resources.


//{{TDialog = chessEndDlg}}
class chessEndDlg : public TDialog {

	char EndMessage[ 128 ];
public:
    chessEndDlg (TWindow* parent, LPSTR lpszMessage, TResId resId = SD_ENDGAMEDLG, TModule* module = 0 );
    virtual ~chessEndDlg ();

//{{chessEndDlgVIRTUAL_BEGIN}}
public:
    virtual void SetupWindow ();
//{{chessEndDlgVIRTUAL_END}}

//{{chessEndDlgRSP_TBL_BEGIN}}
protected:
    void BtnYes ();
    void BtnNo ();
//{{chessEndDlgRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(chessEndDlg);
};    //{{chessEndDlg}}


#endif                                      // __chssendd_h sentry.

