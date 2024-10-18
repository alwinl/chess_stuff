/*  Project chess
	 Naivasha Enterprises
	 Copyright © 1995. All Rights Reserved.

	 SUBSYSTEM:    chess.apx Application
	 FILE:         tinfwndw.cpp
	 AUTHOR:       A.J. Leerling


	 OVERVIEW
	 ========
	 Source file for implementation of TInfoWindow (TWindow).
*/

#include <owl\owlpch.h>
#pragma hdrstop

#include "tinfwndw.h"
#include "externs.h"

short LEFTMARGIN = 6;
short SEPERATOR = 4;

#define MAXLINE    (INFOXSIZE - LEFTMARGIN -1)
#define COLORX     (LEFTMARGIN + CHARSIZE * 6)
#define COLORY     (SEPERATOR + MYFRAMESIZE)
#define COLORW     (CHARSIZE * 6)
#define TIMERX     (LEFTMARGIN + CHARSIZE * 6)
#define TIMERY     (COLORY + LINESIZE + SEPERATOR)
#define TIMERW     (MAXLINE - TIMERX)
#define WHITEINFOX (LEFTMARGIN + CHARSIZE * 7)
#define WHITEINFOY (TIMERY + LINESIZE + SEPERATOR * 2)
#define WHITEINFOW (MAXLINE - WHITEINFOX)
#define BLACKINFOX (WHITEINFOX)
#define BLACKINFOY (WHITEINFOY + LINESIZE + SEPERATOR)
#define BLACKINFOW (WHITEINFOW)
#define LEVELX     (LEFTMARGIN + CHARSIZE * 7)
#define LEVELY     (BLACKINFOY + LINESIZE + SEPERATOR)
#define LEVELW     (MAXLINE - LEVELX)
#define VALUEX     (LEFTMARGIN + CHARSIZE * 7)
#define VALUEY     (LEVELY + LINESIZE + SEPERATOR * 2)
#define VALUEW     (MAXLINE - VALUEX)
#define NODEX      (LEFTMARGIN + CHARSIZE * 8)
#define NODEY      (VALUEY + LINESIZE + SEPERATOR)
#define NODEW      (MAXLINE - NODEX)
#define SECX       (LEFTMARGIN + CHARSIZE * 7)
#define SECY       (NODEY + LINESIZE + SEPERATOR)
#define SECW       (MAXLINE - SECX)
#define DEPTHY     (SECY + LINESIZE + SEPERATOR)
#define DEPTHX     (LEFTMARGIN + CHARSIZE * 7)
#define DEPTHW     (MAXLINE - DEPTHX)
#define STATBESTY  (DEPTHY + LINESIZE + SEPERATOR * 2)
#define BESTX      (LEFTMARGIN)
#define BESTY      (STATBESTY + LINESIZE + 1)
#define BESTW      (MAXLINE - LEFTMARGIN)
#define BESTH      (LINESIZE * 3 + SEPERATOR * 3 - 9)
#define MESSX      (LEFTMARGIN)
#define MESSY      (BESTY + BESTH + SEPERATOR * 2)
#define MESSW      (MAXLINE - MESSX)
#define EmptyStr   ""

//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(TInfoWindow, TWindow)
//{{TInfoWindowRSP_TBL_BEGIN}}	 EV_WM_CTLCOLOR,
//{{TInfoWindowRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{TInfoWindow Implementation}}

TInfoWindow::TInfoWindow (TWindow* parent, const char far* title, TModule* module):
	 TWindow(parent, title, module)
{
	 // Change the window's background color
	 SetBkgndColor(RGB(0xc0, 0xc0, 0xc0));

	 // INSERT>> Your constructor code here.
	short temp;

	Attr.Style |= WS_CHILD | WS_VISIBLE;
	Attr.X = (3 * BORDERSIZE) + (MAXBDSIZE * SQUARE_SIZE) + (2 * MYFRAMESIZE);
	Attr.Y = BORDERSIZE;
	LEFTMARGIN= MYFRAMESIZE + CHARSIZE/2;
	SEPERATOR = LINESIZE/4;
	INFOXSIZE = (28 * CHARSIZE) + (2 * LEFTMARGIN);
	INFOYSIZE = MESSY + LINESIZE + SEPERATOR + MYFRAMESIZE;
	temp = MAXBDSIZE * SQUARE_SIZE + (2 * MYFRAMESIZE);
	INFOYSIZE = (temp > INFOYSIZE) ? temp : INFOYSIZE;
	InfoRect.right = Attr.W = INFOXSIZE;
	InfoRect.bottom = Attr.H = INFOYSIZE;
	InfoRect.left = InfoRect.top =  0;

	new TStatic( this, -1, " Turn:",     LEFTMARGIN,   COLORY, CHARSIZE * 6, LINESIZE, 5);
	new TStatic( this, -1, " White:",    LEFTMARGIN,   WHITEINFOY, CHARSIZE * 7, LINESIZE, 4);
	new TStatic( this, -1, " Black:",    LEFTMARGIN,   BLACKINFOY, CHARSIZE * 7, LINESIZE, 6);
	new TStatic( this, -1, " Time:",     LEFTMARGIN,   TIMERY, CHARSIZE * 6, LINESIZE, 5);
	new TStatic( this, -1, " Level:",    LEFTMARGIN,   LEVELY, CHARSIZE * 13, LINESIZE, 12);
	new TStatic( this, -1, " Value:",    LEFTMARGIN,   VALUEY, CHARSIZE * 7, LINESIZE, 6);
	new TStatic( this, -1, " Nodes:",    LEFTMARGIN,   NODEY, CHARSIZE * 8, LINESIZE, 7);
	new TStatic( this, -1, " N/Sec:",    LEFTMARGIN,   SECY, CHARSIZE * 9, LINESIZE, 9);
	new TStatic( this, -1, " Depth:",    LEFTMARGIN,   DEPTHY, CHARSIZE * 7, LINESIZE, 6);
	new TStatic( this, -1, " Bestline:", LEFTMARGIN-2, STATBESTY, MAXLINE - 2, LINESIZE, 9);

	Color     = new TStatic( this, -1, EmptyStr, COLORX, COLORY, COLORW, LINESIZE, 6);
	WhiteInfo = new TStatic( this, -1, EmptyStr, WHITEINFOX, WHITEINFOY, WHITEINFOW, LINESIZE, WHITEINFOW/CHARSIZE);
	BlackInfo = new TStatic( this, -1, EmptyStr, BLACKINFOX, BLACKINFOY, BLACKINFOW, LINESIZE, BLACKINFOW / CHARSIZE);
	Timer     = new TNoEraseBkGndStatic( this, -1, EmptyStr, TIMERX, TIMERY, TIMERW, LINESIZE, TIMERW /CHARSIZE);
	Level     = new TStatic( this, -1, EmptyStr, LEVELX, LEVELY, LEVELW, LINESIZE, LEVELW / CHARSIZE);
	Value     = new TStatic( this, -1, EmptyStr, VALUEX, VALUEY, VALUEW, LINESIZE, VALUEW / CHARSIZE);
	Nodes     = new TStatic( this, -1, EmptyStr, NODEX, NODEY, NODEW, LINESIZE, NODEW / CHARSIZE);
	Seconds   = new TStatic( this, -1, EmptyStr, SECX, SECY, SECW, LINESIZE, SECW / CHARSIZE);
	Depth     = new TNoEraseBkGndStatic( this, -1, EmptyStr, DEPTHX, DEPTHY, DEPTHW, LINESIZE, DEPTHW /CHARSIZE);
	BestLine  = new TStatic( this, -1, EmptyStr, BESTX, BESTY, BESTW, BESTH, ((BESTW / CHARSIZE) * 3));
	Message   = new TStatic( this, -1, EmptyStr, MESSX, MESSY, MESSW, LINESIZE, MESSW / CHARSIZE);
}


TInfoWindow::~TInfoWindow ()
{
	 Destroy();

	 // INSERT>> Your destructor code here.

}


//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(TNoEraseBkGndStatic, TStatic)
//{{TNoEraseBkGndStaticRSP_TBL_BEGIN}}
	 EV_WM_ERASEBKGND,
//{{TNoEraseBkGndStaticRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{TNoEraseBkGndStatic Implementation}}

TNoEraseBkGndStatic::TNoEraseBkGndStatic (TWindow* parent, int id, const char far* title, int x, int y, int w, int h, uint textLen, TModule* module):
	 TStatic(parent, id, title, x, y, w, h, textLen, module)
{
	 // Change the window's background color
	 SetBkgndColor(RGB(0xff, 0xff, 0xff));

	 // INSERT>> Your constructor code here.

	DoErase = TRUE;
	LastLen = 0;
}


TNoEraseBkGndStatic::~TNoEraseBkGndStatic ()
{
	 Destroy();

	 // INSERT>> Your destructor code here.

}

bool TNoEraseBkGndStatic::EvEraseBkgnd (HDC dc)
{
	 bool result;

//    result = TStatic::EvEraseBkgnd(dc);

	 // INSERT>> Your code here.

	if (DoErase) {
		RECT Rect;
		HBRUSH hOldBrush;

		hOldBrush = (HBRUSH)SelectObject(dc, GetStockObject(LTGRAY_BRUSH));
		::GetClientRect(HWindow, &Rect);
		PatBlt(dc, Rect.left, Rect.top, Rect.right - Rect.left, Rect.bottom - Rect.top, PATCOPY);
		SelectObject(dc, hOldBrush);
		DoErase = FALSE;
		result = TRUE;
	} else
		result = FALSE;

	 return result;
}

void TNoEraseBkGndStatic::SetErase()
{
	DoErase = TRUE;
}

void TNoEraseBkGndStatic::SetText(LPSTR text)
{
	int Len = strlen(text);
	if (Len < LastLen)
		SetErase();

	TStatic::SetText(text);
	LastLen = Len;
}


void TInfoWindow::Paint (TDC& dc, bool erase, TRect& rect)
{
	 TWindow::Paint(dc, erase, rect);

	 // INSERT>> Your code here.
	DrawInfoFrame( dc );
	DrawStaticFrames( dc );
}


HBRUSH TInfoWindow::EvCtlColor (HDC hDC, HWND hWndChild, uint ctlType)
{
	 HBRUSH result;

	 result = TWindow::EvCtlColor(hDC, hWndChild, ctlType);

	 // INSERT>> Your code here.
	SetBkColor( hDC, RGB(192, 192, 192));
	result = (HBRUSH)GetStockObject(LTGRAY_BRUSH);

	return result;
}

void TInfoWindow::Reset()
{
	Color->SetText("White");
	WhiteInfo->Clear();
	BlackInfo->Clear();
	Level->Clear();
	Value->Clear();
	Nodes->Clear();
	Seconds->Clear();
	BestLine->Clear();
	Message->Clear();
}

void TInfoWindow::IterReset()
{
	Color->SetText("White");
	Value->Clear();
	Nodes->Clear();
	Seconds->Clear();
	BestLine->Clear();
}

void TInfoWindow::DrawInfoFrame(HDC hDC)
{
	DrawFrame(hDC, InfoRect);
}

void TInfoWindow::DrawSFrame(HDC hDC, RECT *rect)
{
	int x1, x2, y1, y2;
	HPEN hPen, hOldPen;
	HBRUSH hOldBrush;

	POINT pArray[3];

	x1 = rect->left;
	x2 = rect->right;
	y1 = rect->top;
	y2 = rect->bottom;

	hOldBrush = (HBRUSH)SelectObject(hDC, GetStockObject(LTGRAY_BRUSH));

	PatBlt(hDC, x1, y1, x2 - x1, y2 - y1, PATCOPY);
	SelectObject(hDC, hOldBrush);

	hPen = CreatePen(PS_SOLID, 1, RGB(128, 128, 128));

	hOldPen = (HPEN)SelectObject(hDC, hPen);

	pArray[1].x = pArray[0].x = x1;
	pArray[0].y = y2;
	pArray[2].y = pArray[1].y = y1;
	pArray[2].x = x2;

	Polyline(hDC, pArray, 3);

	DeleteObject(SelectObject(hDC, GetStockObject(WHITE_PEN)));

	pArray[1].x = x2;
	pArray[1].y = y2;
	pArray[2].y = y1 - 1;

	Polyline(hDC, pArray, 3);
	SelectObject(hDC, hOldPen);
}

void TInfoWindow::DrawStaticFrames(HDC hDC)
{
	RECT rect;

	rect.left = LEFTMARGIN - 1;
	rect.right = MAXLINE + 1;

	rect.top = COLORY - 1;
	rect.bottom = COLORY + LINESIZE + 1;
	DrawSFrame(hDC, &rect);

	rect.top = WHITEINFOY - 1;
	rect.bottom = WHITEINFOY + LINESIZE + 1;
	DrawSFrame(hDC, &rect);

	rect.top = BLACKINFOY - 1;
	rect.bottom = BLACKINFOY + LINESIZE + 1;
	DrawSFrame(hDC, &rect);

	rect.top = TIMERY - 1;
	rect.bottom = TIMERY + LINESIZE + 1;
	DrawSFrame(hDC, &rect);

	rect.top = LEVELY - 1;
	rect.bottom = LEVELY + LINESIZE + 1;
	DrawSFrame(hDC, &rect);

	rect.top = VALUEY - 1;
	rect.bottom = VALUEY + LINESIZE + 1;
	DrawSFrame(hDC, &rect);

	rect.top = NODEY - 1;
	rect.bottom = NODEY + LINESIZE + 1;
	DrawSFrame(hDC, &rect);

	rect.top = SECY - 1;
	rect.bottom = SECY + LINESIZE + 1;
	DrawSFrame(hDC, &rect);

	rect.top = DEPTHY - 1;
	rect.bottom = DEPTHY + LINESIZE + 1;
	DrawSFrame(hDC, &rect);

	rect.top = BESTY - 1;
	rect.bottom = BESTY + BESTH + 1;
	DrawSFrame(hDC, &rect);

	rect.top = MESSY - 1;
	rect.bottom = MESSY + LINESIZE + 1;
	DrawSFrame(hDC, &rect);
}



