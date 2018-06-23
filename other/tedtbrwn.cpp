/*  Project chess
    Naivasha Enterprises
    Copyright © 1995. All Rights Reserved.

    SUBSYSTEM:    chess.apx Application
    FILE:         tedtbrwn.cpp
    AUTHOR:       A.J. Leerling


    OVERVIEW
    ========
    Source file for implementation of TEditBarWindow (TWindow).      
*/

#include <owl\owlpch.h>
#pragma hdrstop

#include "tedtbrwn.h"


//{{TEditBarWindow Implementation}}


TEditBarWindow::TEditBarWindow (TWindow* parent, const char far* title, TModule* module):
    TWindow(parent, title, module)
{
    // Change the window's background color
    SetBkgndColor(RGB(0xc0, 0xc0, 0xc0));

    // INSERT>> Your constructor code here.

}


TEditBarWindow::~TEditBarWindow ()
{
    Destroy();

    // INSERT>> Your destructor code here.

}

