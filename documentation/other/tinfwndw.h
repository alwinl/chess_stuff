#if !defined(__tinfwndw_h)              // Sentry, use file only if it's not already included.
#define __tinfwndw_h

/*  Project chess
    Naivasha Enterprises
	 Copyright © 1995. All Rights Reserved.

	 SUBSYSTEM:    chess.apx Application
	 FILE:         tinfwndw.h
	 AUTHOR:       A.J. Leerling


	 OVERVIEW
	 ========
	 Class definition for TInfoWindow (TWindow).
*/

#include <owl\owlpch.h>
#pragma hdrstop

#include <owl\static.h>

#include "chessapp.rh"            // Definition of all resources.

//{{TStatic = TNoEraseBkGndStatic}}
class TNoEraseBkGndStatic : public TStatic {

	BOOL DoErase;
	int LastLen;

public:
	TNoEraseBkGndStatic (TWindow* parent, int id, const char far* title, int x, int y, int w, int h, uint textLen = 0, TModule* module = 0);
	virtual ~TNoEraseBkGndStatic ();

	void SetErase();
	void SetText(LPSTR text);

//{{TNoEraseBkGndStaticRSP_TBL_BEGIN}}protected:
	 bool EvEraseBkgnd (HDC dc);
//{{TNoEraseBkGndStaticRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TNoEraseBkGndStatic);
};    //{{TNoEraseBkGndStatic}}


//{{TWindow = TInfoWindow}}
class TInfoWindow : public TWindow {

	TStatic * Color;
	TStatic * WhiteInfo;
	TStatic * BlackInfo;
	TNoEraseBkGndStatic * Timer;
	TStatic * Level;
	TStatic * Iteration;
	TStatic * Value;
	TStatic * Nodes;
	TStatic * Seconds;
	TNoEraseBkGndStatic * Depth;
	TStatic * BestLine;
	TStatic * Message;
	RECT InfoRect;

	void DrawSFrame(HDC, RECT *);
	void DrawStaticFrames(HDC);

public:
	 TInfoWindow (TWindow* parent, const char far* title = 0, TModule* module = 0);
	 virtual ~TInfoWindow ();

	void SetTurnText(LPSTR text)      { Color->SetText(text);     }
	void SetWhiteInfoText(LPSTR text) { WhiteInfo->SetText(text); }
	void SetBlackInfoText(LPSTR text) { BlackInfo->SetText(text); }
	void SetTimerText(LPSTR text)     { Timer->SetText(text);     }
	void SetLevelText(LPSTR text)     { Level->SetText(text);     }
	void SetIterationText(LPSTR text) { Iteration->SetText(text); }
	void SetValueText(LPSTR text)     { Value->SetText(text);     }
	void SetNodeText(LPSTR text)      { Nodes->SetText(text);     }
	void SetSecondsText(LPSTR text)   { Seconds->SetText(text);   }
	void SetDepthText(LPSTR text)     { Depth->SetText(text);     }
	void SetBestLineText(LPSTR text)  { BestLine->SetText(text);  }
	void SetMessageText(LPSTR text)   { Message->SetText(text);   }

	void DrawInfoFrame(HDC);
	void Reset();
	void IterReset();

//{{TInfoWindowRSP_TBL_BEGIN}}
protected:
	 HBRUSH EvCtlColor (HDC hDC, HWND hWndChild, uint ctlType);
//{{TInfoWindowVIRTUAL_BEGIN}}
	 virtual void Paint (TDC& dc, bool erase, TRect& rect);
//{{TInfoWindowVIRTUAL_END}}
//{{TInfoWindowRSP_TBL_END}}

DECLARE_RESPONSE_TABLE(TInfoWindow);

};    //{{TInfoWindow}}

#endif                                      // __tinfwndw_h sentry.
