// ObjectWindows - (C) Copyright 1992 by Borland International

#ifndef __EXTERNS_H
#define __EXTERNS_H

#ifndef __WCDEFS_H
#include "wcdefs.h"
#endif

extern ENUMCOLOR     Opponent;
extern PIECETAB      PieceTab[][16];
extern BOARDTYPE     Board[0x78];
extern char          WhiteSquareColors[];
extern char          BlackSquareColors[];
extern char          buf[];

/*
extern PTInfoWindow  TInfo;
extern HBRUSH        hBlackBrush;
extern HBRUSH        hWhiteBrush;
extern HWND          hWndMain;
extern HBITMAP       PieceBmpArray[][2];
extern HBITMAP       MaskArray[];
extern HCURSOR       hWaitCursor;
extern HMENU         ThinkMenu;
extern HMENU         MainMenu;
*/

extern bool          ShowBestLine;
extern bool          Turned;
extern bool          Editing;
extern bool          MultiMove, AutoPlay, SingleStep;
extern bool          Analysis, Opan;
extern bool          ComputerThinking;
extern bool          Running;
extern bool          GotValidMove;
extern bool          GameOver;
extern bool          NoComputerMove;
extern bool          SoundOn;

extern int           BORDERSIZE;
extern short         CHARSIZE;
extern short         LINESIZE;
extern short         INFOXSIZE;
extern short         INFOYSIZE;



extern ENUMCOLOR     RunColor;
extern ENUMCOLOR     ComputerColor;
extern ENUMCOLOR     Player;
extern ENUMCOLOR     ProgramColor;
extern ENUMLEVEL     Level;
extern MOVESTRUCT *  MovTab;
extern MOVESTRUCT    ZeroMove;
extern MOVESTRUCT    KeyMove;
extern MOVESTRUCT    HintLine[MAXPLY];
extern MOVESTRUCT    Next;
extern MOVESTRUCT    PlayerMove;
extern MOVESTRUCT    MainLine[MAXPLY];
extern CASTMOVETYPE  CastMove[2][2];
extern double        Nodes;
extern double        AverageTime;
extern char          MaxLevel;
extern int           MainEvalu;
extern int           DirTab[];
extern int           KnightDir[];
extern short         Depth;
extern int           OfficerNo[];
extern int           PawnNo[];
extern int           MoveNo;
extern int           RootValue;
extern int           HintEvalu;
extern int           MaxDepth;
extern char          EndGameMessage[];


/* board.cpp */

/* display.cpp */

/* drag.cpp */
/* movgen.cpp */
void CalcAttackTab( void );
short PieceAttacks( ENUMPIECE apiece, ENUMCOLOR acolor, int asquare, int square );
short Attacks( ENUMCOLOR acolor, int square );
void CalcCastling( ENUMCOLOR incolor,  ENUMCASTDIR *cast );
unsigned int FiftyMoveCnt( void );
unsigned int Repetition( short immediate );
short KillMovGen( MOVESTRUCT *amove );
void InitMovGen( void );
void MovGen( void );




extern POINT        GetSquareXY( int );
extern void         ClearSquare( int square );
extern void         DrawFrame( HDC, RECT &, bool = false );
extern void         ResetNewPos( void );
extern void         Error( char * );
extern void         ColorToPlay( ENUMCOLOR );
extern void         InitDisplay( void );
extern void         ClearDisplay( void );
extern void         ClearInfoWindow( void );
extern void         NewGame( void );
//extern void         InsertPiece( ENUMPIECE p, ENUMCOLOR c, int sq );
extern void         PrintCurLevel( void );
extern void         TakeBackMove( MOVESTRUCT *amove );
extern void         MakeMove( MOVESTRUCT * );
extern short        PieceAttacks( ENUMPIECE, ENUMCOLOR, int, int );
extern void         PrintBestMove( MOVESTRUCT *mainline, int mainevalu );
extern int          StatEvalu( MOVESTRUCT *amove );
extern void         CalcPVTable( void );
extern void         EnterKeyMove( void );
extern void         StartAnalysis( void );
extern void         Warning( char * );
extern void         PrintMove( int, ENUMCOLOR, MOVESTRUCT *, double );
extern void         UpdateBoard( void );
extern void         Message( char * );
extern void         ClearBestLine( void );
extern void         ResetMoves( void );
extern void         InitChessTime( void );
extern void         ClearHint( void );
extern void         ClearPVTable( void );
extern void         AdjustMoves( void );
extern void         FindMove( int maxlevel );
extern void         OpeningLibMsg( void );
extern char         *MoveStr( MOVESTRUCT * );
extern void         EnterMove( MOVESTRUCT *amove );
extern void         FlashMove( MOVESTRUCT *amove );
extern void         StoreMoves( void );
extern void         CalcAttackTab( void );
extern int          SaveGame( char *savefile );
extern int          RestoreGame( char * );
extern bool         Undo( void );
extern bool         Redo( void );
extern void         PrintBoard( void );
extern void         FindHintMove( void );
extern void         ShowHint( void );
extern void         Talk( void );
extern int   GetValidSquare( POINT );
extern void         DrawInvertedBitmap( int );
extern void         DrawNormalBitmap( int );
extern void         InitCommStuff( void );
extern bool         MoveCheck( int, int );
extern void         ProgramMove( void );
extern WORD         MessageToPost;
extern void         QuitProgram( void );
extern void         HideAttacks( void );
void                DoSlideMove( MOVESTRUCT & );
void                GenCastSquare( int, int *, int * );

#endif // __EXTERNS_H
