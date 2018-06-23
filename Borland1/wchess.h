/*$T WCHESS.H GC 1.123 09/26/01 08:43:41 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


// ObjectWindows - (C) Copyright 1992 by Borland International
#ifndef __WCHESS_H
#define __WCHESS_H

#define IDM_BEGIN       24350
#define IDM_MOVEPIECE   24350
#define IDM_MOVETIME    24351
#define IDM_TOTALTIME   24352
#define IDM_DEMO        24353
#define IDM_INFINITE    24354
#define IDM_PLY         24355
#define IDM_MATE        24356
#define IDM_SINGLE      24357
#define IDM_TWOPLAYER   24358
#define IDM_AUTO        24359
#define IDM_REVERSE     24360
#define CM_UNDO         24361
#define CM_REDO         24362
#define CM_STOP         24363
#define IDM_EASY        24364
#define IDM_HINT        24365
#define IDM_PASS        24366
#define IDM_EXIT        24371
#define IDM_MATCHING    24372
#define IDM_WHITETURN   24373
#define IDM_BLACKTURN   24374
#define IDM_COLORS      24375
#define IDM_BESTLINE    24376
#define IDM_MOUSEDRAG   24377
#define IDM_MOUSESELECT 24378
#define IDM_PIECEVALUES 24379
#define IDM_SOUND       24380

#define EM_ERROR        24344
#define EM_CANCEL       24346
#define EM_CLEAR        24347
#define EM_DONE         24348
#define SD_ENDGAMEDLG   24349
#define SD_CHESSERROR   24345

#define IDD_WRED        24854
#define IDD_WGREEN      24855
#define IDD_WBLUE       24856
#define IDD_WEDITRED    24857
#define IDD_WEDITGREEN  24858
#define IDD_WEDITBLUE   24859
#define IDD_BRED        24860
#define IDD_BGREEN      24861
#define IDD_BBLUE       24862
#define IDD_BEDITRED    24863
#define IDD_BEDITGREEN  24864
#define IDD_BEDITBLUE   24865
#define IDD_BLACKCOLOR  24866
#define IDD_WHITECOLOR  24867

#define IDD_QUEEN       24868
#define IDD_ROOK        24869
#define IDD_BISHOP      24870
#define IDD_KNIGHT      24871
#define IDD_PAWN        24872
#define IDD_EQUEEN      24873
#define IDD_EROOK       24874
#define IDD_EBISHOP     24875
#define IDD_EKNIGHT     24876
#define IDD_EPAWN       24877
#define IDD_DEFAULT     201

#define TIMEID          202

//
// =======================================================================================================================
//    TChess
// =======================================================================================================================
//
class CChessApp : public CWinApp
{
public:
    CChessApp( void );

    // Overrides ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CChessApp)
    public:
    virtual BOOL InitInstance()
    {
        if( !AfxOleInit() ) {
            AfxMessageBox( "OLE Init failed" );
            return( FALSE );
        }

                    AfxEnableControlContainer();

    #ifdef _AFXDLL
                    Enable3dControls();         // Call this when using MFC in a shared DLL
    #else
                    Enable3dControlsStatic();   // Call this when linking to MFC statically
    #endif

                    SetRegistryKey(_T("Naivasha Enterprises\\Chess"));

                    HINSTANCE hInst = AfxGetResourceHandle();
                    m_hMenu  = ::LoadMenu(hInst, MAKEINTRESOURCE(IDR_MAINFRAME));
                    m_hAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_MAINFRAME) /* "ChessCommands"*/);

                    CFrameWnd* pFrame = new CAppFrame( m_hMDIMenu, m_hMDIAccel );
                    m_pMainWnd = pFrame;

                    // create main MDI frame window
        if (!pFrame->LoadFrame(IDR_MAINFRAME, WS_OVERLAPPEDWINDOW )) {
            AfxMessageBox(_T("Could not load frame"),  MB_ICONHAND|MB_OK|MB_DEFBUTTON1);
                        return ( FALSE)
        ;
        }

                    // The main window has been initialized, so show and update it.
                    pFrame->ShowWindow(m_nCmdShow);
                    pFrame->UpdateWindow();

                    return ( TRUE)
    ;
    }

    virtual BOOL ExitInstance()
    {
                    if (m_hMenu != NULL)
                        FreeResource(m_hMenu);

                    if (m_hAccel != NULL)
                        FreeResource(m_hAccel);

                    return ( CWinApp::ExitInstance())
    ;
    }
    //}}AFX_VIRTUAL
//
// -----------------------------------------------------------------------------------------------------------------------
//    Implementation
// -----------------------------------------------------------------------------------------------------------------------
//
protected:
    HMENU   m_hMenu;
    HACCEL  m_hAccel;

public:
    //{{AFX_MSG(CChessApp)
    afx_msg void OnAppAbout();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//
// =======================================================================================================================
//    TMessageDialog This creates a BWCC style message box using a static template from the RC file. A more versatile
//    version would dynamically create the message box to fit the size of each message, and allow the user to specify
//    what type of buttons to display, much like the windows version of MessageBox
// =======================================================================================================================
//
class CMessageDialog : public CDialog
{
    LPSTR           NewMessage;
    LPSTR           NewTitle;
public:
    TMessageDialog( PTWindowsObject AParent, int ResourceId, LPSTR Message, LPSTR Title );
    virtual void    SetupWindow( void );
};

//
// =======================================================================================================================
//    CEndDialog
// =======================================================================================================================
//
class CEndDialog : public CDialog
{
    LPSTR                       EndMessage;

public:
    TEndDialog( CWnd *pParent, int ResourceId, LPSTR Message );
    virtual void                SetupWindow( void );
    inline void TEndDialog::    Yes( UINT )     { CloseWindow( IDYES ); }
    inline void TEndDialog::    No( UINT )      { ShutDownWindow(); }
    DECLARE_RESPONSE_TABLE( TEndDialog );
};

//
// =======================================================================================================================
//    TChessWindow
// =======================================================================================================================
//
class CChessWindow : public CFrameWindow
{
    enum TURNTYPE { player, computer };

    PTInfoWindow TInfo;         // pointer to the information window class
    TEditBarWindow *TEditBar;   // pointer to the edit bar class
    BOOL NewGame;               // a new game is being set up?
    char *FileName;             // last saved game
    RECT MainWndRect;           // coordinates for the main window
    RECT InfoAreaRect;          // coordinates for the information window
    HMENU hMenu;                // handle to the main menu
    HMENU hThinkMenu;           // handle to the short (stop) menu
    HMENU hEditMenu;            // handle to the edit (arrange) menu
    TURNTYPE WhoseTurn;         // players or computers turn?
    SQUARETYPE MoveStartSquare; // starting square for move
    SQUARETYPE MoveEndSquare;   // ending square for move
    COLORTYPE CurPlayer;        // current player
    BOOL GotStartSquare;        // did the user select a valid piece to move?
    HBRUSH hBKBrush;            // handle to brush to paint main window backgnd
    HBITMAP hBKBrushBmp;        // small bitmap used to paint main window bkgnd
    BOOL Modified;              // has the board been changed in edit mode?
    BOOL EditingBoard;          // is the user editing the board
    BOOL Dragging;              // in drag mode?
    BOARDTYPE *SaveBoard;       // saved board setup in case user cancels edit

public:
    TChessWindow( PTWindowsObject AParent, LPSTR ATitle );
    ~TChessWindow( void );
    virtual void    SetupWindow( void );
    virtual void GetWindowClass( WNDCLASS &WndClass ) {
        //
        // Again, call the ancestor version first, as must also be done with SetupWindow
        // and InitInstance
        //
        TFrameWindow::GetWindowClass( WndClass );

        //
        // this allows you to specify your own values for the window class, which OWL will
        // register for you
        //
        ::hArrowCursor = WndClass.hCursor = LoadCursor( 0, IDC_ARROW );
        hBKBrushBmp = LoadBitmap( GetApplication()->hInstance, "BKBrushBmp" );
        hBKBrush = CreatePatternBrush( hBKBrushBmp );

        WndClass.hbrBackground = hBKBrush;
        WndClass.lpszMenuName = "TChessMenu";
        WndClass.hIcon = LoadIcon( GetApplication()->hInstance, "ChessIcon" );
    }

    virtual void                Paint( HDC PaintDC, PAINTSTRUCT &PaintInfo );
    void                        EvLButtonDown( UINT, TPoint & );
    void                        EvMouseMove( UINT, TPoint & );
    void                        CMNewGame( void );
    void                        CMRestoreGame( void );
    void                        CMSaveGame( void );
    void                        CMSaveGameAs( void );
    inline void TChessWindow::  Exit( void )    { CloseWindow(); }
    void                        UndoMove( void );
    void                        RedoUndo( void );
    void                        IDMMovePieces( void );
    void                        IDMMoveTime( void );
    void                        IDMTotalTime( void );
    void                        IDMDemo( void );
    void                        IDMInfiniteSearch( void );
    void                        IDMPlySearch( void );
    void                        IDMMateSearch( void );
    void                        IDMSinglePlayer( void );
    void                        IDMTwoPlayer( void );
    void                        IDMAutoPlay( void );
    void                        IDMReverseBoard( void );
    void                        IDMEasy( void );
    void                        IDMHint( void );
    void                        IDMPass( void );
    void                        EMDone( void );
    void                        EMClear( void );
    void                        EMCancel( void );
    void                        EMError( void );
    void                        IDMMatching( void );
    void                        IDMWhiteTurn( void );
    void                        IDMBlackTurn( void );
    void                        IDMColors( void );
    void                        IDMPieceValues( void );
    void                        IDMBestLine( void );
    void                        IDMMouseDrag( void );
    void                        IDMMouseSelect( void );
    void                        IDMSound( void );
    void                        EvTimer( UINT );
    void                        EndGame( void );
    void                        DoEdit( void );
    void                        SaveGameAs( void );
    void                        ComputersTurn( void );
    void                        UnCheckLevelMenu( LEVELTYPE );
    void                        NormalSetup( void );
    void                        CheckLevelMenu( LEVELTYPE );

    void                        OnEMDone( void );
    void                        OnEMClear( void );
    void                        OnEMCancel( void );
    void                        EMError( void );

    virtual BOOL CanClose( void ) {
        if( ComputerThinking || AutoPlay ) {
            ::PostMessage( HWindow, WM_COMMAND, CM_STOP, 0L );
            ::PostMessage( HWindow, WM_COMMAND, IDM_EXIT, 0L );
            return( FALSE );
        }

        QuitProgram();
        return( TRUE );
    }
};
#endif // __WCHESS_H
