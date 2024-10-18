/*  Project chss
    Naivasha Enterprises
    Copyright © 1997. All Rights Reserved.

    SUBSYSTEM:    chss.exe Application
    FILE:         tchess.cpp
    AUTHOR:       A.J. Leerling


    OVERVIEW
    ========
    Source file for implementation of TChess (TApplication).      
*/


#include <owl\owlpch.h>
#pragma hdrstop

#include "tchess.h"
#include "tchessvw.h"                        // Definition of client class.       
#include "tabotdlg.h"                        // Definition of about dialog.       


//{{TChess Implementation}}



//{{DOC_VIEW}}
DEFINE_DOC_TEMPLATE_CLASS(TFileDocument, TChessView, DocType1);
//{{DOC_VIEW_END}}

//{{DOC_MANAGER}}
DocType1 __dvt1("Chess Files (*.chs)", "*.CHS", 0, "CHS", dtAutoDelete | dtUpdateDir);
//{{DOC_MANAGER_END}}


//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(TChess, TApplication)
//{{TChessRSP_TBL_BEGIN}}
    EV_OWLVIEW(dnCreate, EvNewView),
    EV_OWLVIEW(dnClose,  EvCloseView),
    EV_COMMAND(CM_HELPABOUT, CmHelpAbout),
//{{TChessRSP_TBL_END}}
END_RESPONSE_TABLE;


//////////////////////////////////////////////////////////
// DChess
// =====
//
TChess::TChess () : TApplication("Chess")
{
    SetDocManager(new TDocManager(dmSDI, this));

    // INSERT>> Your constructor code here.
}


TChess::~TChess ()
{
    // INSERT>> Your destructor code here.
}




void TChess::CreateGadgets (TControlBar *cb, bool server)
{
    if (!server) {
        cb->Insert(*new TButtonGadget(CM_FILENEW, CM_FILENEW));
        cb->Insert(*new TButtonGadget(CM_FILEOPEN, CM_FILEOPEN));
        cb->Insert(*new TButtonGadget(CM_FILESAVE, CM_FILESAVE));
        cb->Insert(*new TSeparatorGadget(6));
    }

    cb->Insert(*new TButtonGadget(CM_EDITCUT, CM_EDITCUT));
    cb->Insert(*new TButtonGadget(CM_EDITCOPY, CM_EDITCOPY));
    cb->Insert(*new TButtonGadget(CM_EDITPASTE, CM_EDITPASTE));
    cb->Insert(*new TSeparatorGadget(6));
    cb->Insert(*new TButtonGadget(CM_EDITUNDO, CM_EDITUNDO));
    cb->Insert(*new TSeparatorGadget(6));
    cb->Insert(*new TButtonGadget(CM_EDITFIND, CM_EDITFIND));
    cb->Insert(*new TButtonGadget(CM_EDITFINDNEXT, CM_EDITFINDNEXT));

    // Add fly-over help hints.
    cb->SetHintMode(TGadgetWindow::EnterHints);
}


void TChess::SetupSpeedBar (TDecoratedFrame *frame)
{ 
    //
    // Create default toolbar New and associate toolbar buttons with commands.
    //   
    TControlBar* cb = new TControlBar(frame);
    CreateGadgets(cb);

    // Setup the toolbar ID used by OLE 2 for toolbar negotiation.
    cb->Attr.Id = IDW_TOOLBAR;

    frame->Insert(*cb, TDecoratedFrame::Top);
}


//////////////////////////////////////////////////////////
// DChess
// =====
// Application intialization.
//
void TChess::InitMainWindow ()
{
    if (nCmdShow != SW_HIDE)
        nCmdShow = (nCmdShow != SW_SHOWMINNOACTIVE) ? SW_SHOWNORMAL : nCmdShow;

    SDIDecFrame *frame = new SDIDecFrame(0, GetName(), 0, true, this);

    //
    // Assign ICON w/ this application.
    //
    frame->SetIcon(this, IDI_SDIAPPLICATION);

    //
    // Menu associated with window and accelerator table associated with table.
    //
    frame->AssignMenu(SDI_MENU);
    
    //
    // Associate with the accelerator table.
    //
    frame->Attr.AccelTable = SDI_MENU;

    SetupSpeedBar(frame);

    TStatusBar *sb = new TStatusBar(frame, TGadget::Recessed,
                                    TStatusBar::CapsLock        |
                                    TStatusBar::NumLock         |
                                    TStatusBar::ScrollLock      |
                                    TStatusBar::Overtype);
    frame->Insert(*sb, TDecoratedFrame::Bottom);
  
    SetMainWindow(frame);

    frame->SetMenuDescr(TMenuDescr(SDI_MENU));

    //
    // Borland Windows custom controls.
    //
    EnableBWCC();
}


//////////////////////////////////////////////////////////
// TChess
// =====
// Response Table handlers:
//
void TChess::EvNewView (TView& view)
{
    GetMainWindow()->SetClientWindow(view.GetWindow());
    if (!view.IsOK())
        GetMainWindow()->SetClientWindow(0);
    else if (view.GetViewMenu())
        GetMainWindow()->MergeMenu(*view.GetViewMenu());
}


void TChess::EvCloseView (TView&)
{
    GetMainWindow()->SetClientWindow(0);
    GetMainWindow()->SetCaption("Chess");
}


//{{SDIDecFrame Implementation}}

SDIDecFrame::SDIDecFrame (TWindow *parent, const char far *title, TWindow *clientWnd, bool trackMenuSelection, TModule *module)
    : TDecoratedFrame(parent, title, clientWnd, trackMenuSelection, module)
{
    // INSERT>> Your constructor code here.

}


SDIDecFrame::~SDIDecFrame ()
{
    // INSERT>> Your destructor code here.

}


//////////////////////////////////////////////////////////
// TChess
// ===========
// Menu Help About chss.exe command
void TChess::CmHelpAbout ()
{
    //
    // Show the modal dialog.
    //
    TAboutDlg(GetMainWindow()).Execute();
}


int OwlMain (int , char* [])
{
    try {
        TChess    app;
        return app.Run();
    }
    catch (xmsg& x) {
        ::MessageBox(0, x.why().c_str(), "Exception", MB_OK);
    }

    return -1;
}
