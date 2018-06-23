/*$T COLORS.H GC 1.123 09/25/01 14:38:10 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __COLORS_H
#define __COLORS_H

#define CL_KILLFOCUS    ( WM_USER + 101 )
class CColorsDialog : public CDialog
{
    enum ScrollBarColors { Red, Green, Blue };

    HBRUSH hWStatic;
    HBRUSH hBStatic;
    HBRUSH hSBBrush[3];
    int RWID;
    int GWID;
    int BWID;
    int RBID;
    int GBID;
    int BBID;
    int WStatic;
    int BStatic;
    HWND BlackSq;
    HWND WhiteSq;
    BYTE WSqColors[3];
    BYTE BSqColors[3];

    void CLSetFocus( HWND );
    BOOL GetColorValue( WORD );

public:
    CColorsDialog( CWnd *pParent = NULL );

    virtual void SetupWindow( void );
    virtual HBRUSH EvCtlColor( HDC, HWND, UINT );
    virtual void EvVScroll( UINT, UINT, HWND );
    virtual void EvDrawItem( UINT, DRAWITEMSTRUCT __far & );
    virtual LRESULT EvCommand( UINT, HWND, UINT );
    virtual LRESULT CLKillFocus( WPARAM, LPARAM );
    virtual void OnOk( void );

    DECLARE_MESSAGE_MAP( void );
};
#endif // __COLORS_H
