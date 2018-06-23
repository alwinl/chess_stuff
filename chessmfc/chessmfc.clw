; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CWndInfo
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "chessmfc.h"
LastPage=0

ClassCount=9
Class1=CChessDoc
Class2=CChessFrame
Class3=CChessApp
Class4=CAboutDlg
Class5=CChessView
Class6=CSquare

ResourceCount=6
Resource1=IDD_INPUT
Class7=CDlgColor
Resource2=IDR_EDIT
Resource3=IDR_THINKING
Resource4=IDD_ABOUTBOX (English (U.S.))
Class8=CDlgInput
Resource5=IDR_MAINFRAME (English (U.S.))
Class9=CWndInfo
Resource6=IDD_INFO

[CLS:CChessDoc]
Type=0
BaseClass=CDocument
HeaderFile=ChessDoc.h
ImplementationFile=ChessDoc.cpp
Filter=N
VirtualFilter=DC
LastObject=CChessDoc

[CLS:CChessFrame]
Type=0
BaseClass=CFrameWnd
HeaderFile=ChessFrm.h
ImplementationFile=ChessFrm.cpp
Filter=T
VirtualFilter=fWC
LastObject=ID_EDITDONE

[CLS:CChessApp]
Type=0
BaseClass=CWinApp
HeaderFile=chessmfc.h
ImplementationFile=chessmfc.cpp
Filter=N

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=chessmfc.cpp
ImplementationFile=chessmfc.cpp
Filter=D

[CLS:CChessView]
Type=0
BaseClass=CView
HeaderFile=ChessView.h
ImplementationFile=ChessView.cpp
Filter=C
LastObject=CChessView
VirtualFilter=VWC

[CLS:CSquare]
Type=0
BaseClass=CObject
HeaderFile=Square.h
ImplementationFile=Square.cpp
Filter=N
LastObject=CSquare

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg

[TB:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_FILE_PRINT
Command8=ID_APP_ABOUT
CommandCount=8

[MNU:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=CChessView
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_SAVE_AS
Command5=ID_GAME_PLAY
Command6=ID_GAME_HINT
Command7=ID_FILE_MRU_FILE1
Command8=ID_APP_EXIT
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_REDO
Command11=ID_EDIT_ARRANGE
Command12=ID_LEVEL_EASY
Command13=ID_LEVEL_TIMEPERMOVE
Command14=ID_LEVEL_TOTALTIME
Command15=ID_LEVEL_MATCHING
Command16=ID_LEVEL_INFINITE
Command17=ID_LEVEL_PLAYSEARCH
Command18=ID_LEVEL_MATESEARCH
Command19=ID_OPTIONS_TWOPLAYER
Command20=ID_OPTIONS_DEMOMODE
Command21=ID_OPTIONS_PIECEVALUES
Command22=ID_OPTIONS_MOVEMENT_DRAG
Command23=ID_OPTIONS_MOVEMENT_SELECT
Command24=ID_OPTIONS_SOUND
Command25=ID_DISPLAY_COLORS_BACKGROUND
Command26=ID_DISPLAY_COLORS_BLACKSQUARECOLOR
Command27=ID_DISPLAY_COLORS_WHITESQUARECOLOR
Command28=ID_DISPLAY_REVERSEBOARD
Command29=ID_DISPLAY_SHOWBESTLINE
Command30=ID_VIEW_TOOLBAR
Command31=ID_VIEW_STATUS_BAR
Command32=ID_APP_ABOUT
CommandCount=32

[ACL:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_EDIT_COPY
Command2=ID_FILE_NEW
Command3=ID_FILE_OPEN
Command4=ID_FILE_SAVE
Command5=ID_EDIT_PASTE
Command6=ID_EDIT_REDO
Command7=ID_EDIT_UNDO
Command8=ID_EDIT_CUT
Command9=ID_NEXT_PANE
Command10=ID_PREV_PANE
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
Command13=ID_EDIT_CUT
Command14=ID_EDIT_UNDO
CommandCount=14

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=?
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[CLS:CDlgColor]
Type=0
HeaderFile=DlgColor.h
ImplementationFile=DlgColor.cpp
BaseClass=CDialog
Filter=D
LastObject=ID_EDIT_ARRANGE
VirtualFilter=dWC

[MNU:IDR_THINKING]
Type=1
Class=?
Command1=ID_STOP
CommandCount=1

[MNU:IDR_EDIT]
Type=1
Class=?
Command1=ID_EDITDONE
Command2=ID_EDITCLEAR
Command3=ID_EDITTURN_WHITE
Command4=ID_EDITTURN_BLACK
Command5=ID_EDITCANCEL
CommandCount=5

[DLG:IDD_INPUT]
Type=1
Class=CDlgInput
ControlCount=4
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_VALUE,edit,1350631552
Control4=IDC_PROMPT,static,1342308352

[CLS:CDlgInput]
Type=0
HeaderFile=DlgInput.h
ImplementationFile=DlgInput.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDC_PROMPT

[DLG:IDD_INFO]
Type=1
Class=CWndInfo
ControlCount=21
Control1=IDC_STATIC,static,1342308352
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC,static,1342308352
Control11=IDC_INFOTURN,static,1342312448
Control12=IDC_INFOWHITE,static,1342312448
Control13=IDC_INFOBLACK,static,1342312448
Control14=IDC_INFOTIME,static,1342312448
Control15=IDC_INFOLEVEL,static,1342312448
Control16=IDC_INFOVALUE,static,1342312448
Control17=IDC_INFONODES,static,1342312448
Control18=IDC_INFONSEC,static,1342312448
Control19=IDC_INFODEPTH,static,1342312448
Control20=IDC_INFOBESTLINE,static,1342312448
Control21=IDC_INFOMESSAGE,static,1342308352

[CLS:CWndInfo]
Type=0
HeaderFile=WndInfo.h
ImplementationFile=WndInfo.cpp
BaseClass=CDialog
Filter=D
LastObject=CWndInfo
VirtualFilter=dWC

