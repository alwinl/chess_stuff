// ObjectWindows - (C) Copyright 1992 by Borland International

#ifndef _EDITBAR_H
#define _EDITBAR_H


_CLASSDEF( TEditBarWindow )

class TEditBarWindow: public TWindow
{
public:
    TEditBarWindow( PTWindowsObject AParent, LPSTR ATitle );

    virtual void Paint( HDC PaintDC, PAINTSTRUCT _FAR & PaintInfo );
    virtual void WMLButtonUp( TMessage& ) = [WM_FIRST + WM_LBUTTONUP];
    virtual void GetWindowClass( WNDCLASS& WndClass )

    int GetSelectedItem()
    {
        return SelectedItem;
    }

private:
    int SelectedItem;
    RECT EditBarRect;
};


#endif // _EDITBAR_H
