#if !defined(__chsswndw_h)              // Sentry, use file only if it's not already included.
#define __chsswndw_h

/*  Project chess
	 Naivasha Enterprises
	 Copyright © 1995. All Rights Reserved.

	 SUBSYSTEM:    chess.exe Application
	 FILE:         chsswndw.h
	 AUTHOR:       A.J. Leerling


	 OVERVIEW
	 ========
	 Class definition for chessWindow (TWindow).
*/


#include <owl\owlpch.h>
#pragma hdrstop

#include "tinfwndw.h"
#include "tedtbrwn.h"
#include "externs.h"
#include "chessapp.rh"            // Definition of all resources.


//{{TWindow = chessWindow}}class chessWindow : public TWindow {

	enum TURNTYPE { player, computer };
	TInfoWindow * TInfo;       // pointer to the information window class
   TEditBarWindow *TEditBar; // pointer to the edit bar class
   BOOL NewGame;             // a new game is being set up?
   char *FileName;           // last saved game
   RECT MainWndRect;         // coordinates for the main window
   RECT InfoAreaRect;        // coordinates for the information window
   HMENU hMenu;              // handle to the main menu
   HMENU hThinkMenu;         // handle to the short (stop) menu
   HMENU hEditMenu;          // handle to the edit (arrange) menu
   TURNTYPE WhoseTurn;       // players or computers turn?
   SQUARETYPE MoveStartSquare; // starting square for move
   SQUARETYPE MoveEndSquare; // ending square for move
   COLORTYPE CurPlayer;      // current player
   BOOL GotStartSquare;      // did the user select a valid piece to move?
   HBRUSH hBKBrush;          // handle to brush to paint main window backgnd
   HBITMAP hBKBrushBmp;      // small bitmap used to paint main window bkgnd
   BOOL Modified;            // has the board been changed in edit mode?
   BOOL EditingBoard;        // is the user editing the board
   BOOL Dragging;            // in drag mode?
   BOARDTYPE *SaveBoard;     // saved board setup in case user cancels edit

public:
    chessWindow (TWindow* parent, const char far* title = 0, TModule* module = 0);
    virtual ~chessWindow ();

	void ComputersTurn();

//{{chessWindowVIRTUAL_BEGIN}}
public:
    virtual void SetupWindow ();
    virtual void Paint (TDC& dc, bool erase, TRect& rect);
//{{chessWindowVIRTUAL_END}}

//{{chessWindowRSP_TBL_BEGIN}}
protected:
    void EvLButtonDown (uint modKeys, TPoint& point);
    void IDMMovePiece ();
    void EvMouseMove (uint modKeys, TPoint& point);
    void CMNewGame ();
    void CMRestoreGame ();
//{{chessWindowRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(chessWindow);
};    //{{chessWindow}}

#endif                                      // __chsswndw_h sentry.
