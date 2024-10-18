#if !defined(__chssabtd_h)              // Sentry, use file only if it's not already included.
#define __chssabtd_h

/*  Project chess
    Naivasha Enterprises
    Copyright © 1995. All Rights Reserved.

    SUBSYSTEM:    chess.exe Application
    FILE:         chssabtd.h
    AUTHOR:       A.J. Leerling


    OVERVIEW
    ========
    Class definition for chessAboutDlg (TDialog).      
*/


#include <owl\owlpch.h>
#pragma hdrstop

#include "chessapp.rh"                  // Definition of all resources.


//{{TDialog = chessAboutDlg}}
class chessAboutDlg : public TDialog {
public:
    chessAboutDlg (TWindow *parent, TResId resId = IDD_ABOUT, TModule *module = 0);
    virtual ~chessAboutDlg ();

//{{chessAboutDlgVIRTUAL_BEGIN}}
public:
    void SetupWindow ();
//{{chessAboutDlgVIRTUAL_END}}
};    //{{chessAboutDlg}}


// Reading the VERSIONINFO resource.
class ProjectRCVersion {
public:
    ProjectRCVersion (TModule *module);
    virtual ~ProjectRCVersion ();

    bool GetProductName (LPSTR &prodName);
    bool GetProductVersion (LPSTR &prodVersion);
    bool GetCopyright (LPSTR &copyright);
    bool GetDebug (LPSTR &debug);

protected:
    LPBYTE      TransBlock;
    void FAR    *FVData;

private:
    // Don't allow this object to be copied.
    ProjectRCVersion (const ProjectRCVersion &);
    ProjectRCVersion & operator =(const ProjectRCVersion &);
};


#endif                                      // __chssabtd_h sentry.
