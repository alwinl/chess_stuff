/*  Project chess
    Naivasha Enterprises
    Copyright © 1995. All Rights Reserved.

    SUBSYSTEM:    chess.apx Application
    FILE:         chssendd.cpp
    AUTHOR:       A.J. Leerling


    OVERVIEW
    ========
    Source file for implementation of chessEndDlg (TDialog).      
*/

#include <owl\owlpch.h>
#pragma hdrstop

#include "chssendd.h"


//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(chessEndDlg, TDialog)
//{{chessEndDlgRSP_TBL_BEGIN}}
    EV_BN_CLICKED(IDYES, BtnYes),
    EV_BN_CLICKED(IDNO, BtnNo),
//{{chessEndDlgRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{chessEndDlg Implementation}}


chessEndDlg::chessEndDlg (TWindow* parent, LPSTR lpszMessage, TResId resId, TModule* module ):
	 TDialog(parent, resId, module)
{
	 // INSERT>> Your constructor code here.
   strcpy( EndMessage, lpszMessage );

}


chessEndDlg::~chessEndDlg ()
{
    Destroy();

    // INSERT>> Your destructor code here.

}


void chessEndDlg::SetupWindow ()
{
    TDialog::SetupWindow();

    // INSERT>> Your code here.
   ::SetDlgItemText(HWindow, ID_INPUT, EndMessage);

}


void chessEndDlg::BtnYes ()
{
    // INSERT>> Your code here.
    CloseWindow(IDYES);

}


void chessEndDlg::BtnNo ()
{
    // INSERT>> Your code here.
	 ShutDownWindow();
}

