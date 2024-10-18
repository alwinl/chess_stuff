#if !defined(__chessapp_h)              // Sentry, use file only if it's not already included.
#define __chessapp_h

/*  Project chess
    Naivasha Enterprises
    Copyright © 1995. All Rights Reserved.

    SUBSYSTEM:    chess.exe Application
    FILE:         chessapp.h
    AUTHOR:       A.J. Leerling


    OVERVIEW
    ========
    Class definition for chessApp (TApplication).      
*/


#include <owl\owlpch.h>
#pragma hdrstop


#include "chessapp.rh"            // Definition of all resources.


//
// FrameWindow must be derived to override Paint for Preview and Print.
//
//{{TDecoratedFrame = SDIDecFrame}}
class SDIDecFrame : public TDecoratedFrame {
public:
    SDIDecFrame (TWindow *parent, const char far *title, TWindow *clientWnd, bool trackMenuSelection = false, TModule *module = 0);
    ~SDIDecFrame ();
};    //{{SDIDecFrame}}


//{{TApplication = chessApp}}
class chessApp : public TApplication {
private:

    void SetupSpeedBar (TDecoratedFrame *frame);
	void SaveGame( );
	void LoadGame( );

public:
    chessApp ();
    virtual ~chessApp ();

    TOpenSaveDialog::TData FileData;                    // Data to control open/saveas standard dialog.
	void OpenFile (const char *fileName = 0 );

//{{chessAppVIRTUAL_BEGIN}}
public:
    virtual void InitMainWindow();
    virtual bool CanClose ();
//{{chessAppVIRTUAL_END}}

//{{chessAppRSP_TBL_BEGIN}}
protected:
    void CmFileNew ();
    void CmFileOpen ();
    void CmHelpAbout ();
    void CmFileSave ();
    void CmFileSaveAs ();
//{{chessAppRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(chessApp);
};    //{{chessApp}}


#endif                                      // __chessapp_h sentry.
