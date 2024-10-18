#if !defined(__tabotdlg_h)              // Sentry, use file only if it's not already included.
#define __tabotdlg_h

/*  Project chss
    Naivasha Enterprises
    Copyright © 1997. All Rights Reserved.

    SUBSYSTEM:    chss.exe Application
    FILE:         tabotdlg.h
    AUTHOR:       A.J. Leerling


    OVERVIEW
    ========
    Class definition for TAboutDlg (TDialog).      
*/


#include <owl\owlpch.h>
#pragma hdrstop

#include "tchess.rh"                  // Definition of all resources.


//{{TDialog = TAboutDlg}}
class TAboutDlg : public TDialog {
public:
    TAboutDlg (TWindow *parent, TResId resId = IDD_ABOUT, TModule *module = 0);
    virtual ~TAboutDlg ();

//{{TAboutDlgVIRTUAL_BEGIN}}
public:
    void SetupWindow ();
//{{TAboutDlgVIRTUAL_END}}
};    //{{TAboutDlg}}

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


#endif                                      // __tabotdlg_h sentry.
