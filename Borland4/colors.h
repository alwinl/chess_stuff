// ObjectWindows - (C) Copyright 1992 by Borland International

#ifndef __COLORS_H
#define __COLORS_H

#define CL_KILLFOCUS    (WM_USER + 101)

_CLASSDEF(TColorsDialog)

class TColorsDialog : public TDialog
{
   enum ScrollBarColors { Red, Green, Blue };
   HBRUSH hWStatic, hBStatic, hSBBrush[3];
   int RWID, GWID, BWID, RBID, GBID, BBID;
	int WStatic, BStatic;
	HWND BlackSq, WhiteSq;
	BYTE WSqColors[3];
	BYTE BSqColors[3];

	void CLSetFocus(HWND);
	BOOL GetColorValue(WORD);

 public:
		TColorsDialog(TWindow * AParent, LPSTR AName);
		~TColorsDialog();
		virtual void    SetupWindow();
		virtual HBRUSH  EvCtlColor( HDC, HWND, UINT );
		virtual void    EvVScroll( UINT, UINT, HWND );
		virtual void    EvDrawItem( UINT, DRAWITEMSTRUCT __far & );
		virtual LRESULT EvCommand( UINT, HWND, UINT );
		virtual LRESULT CLKillFocus( WPARAM, LPARAM );
		virtual void    Ok( UINT );

		DECLARE_RESPONSE_TABLE(TColorsDialog);
};


#endif // __COLORS_H
