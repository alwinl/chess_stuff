// ObjectWindows - (C) Copyright 1992 by Borland International

#ifndef __EXTERNS_H
#define __EXTERNS_H

#ifndef __WCDEFS_H
#include "wcdefs.h"
#endif

#include "tinfwndw.h"

extern ENUMCOLOR     Opponent;
extern PIECETAB      PieceTab[][16];
extern BOARDTYPE     Board[0x78];
extern BYTE          WhiteSquareColors[];
extern BYTE          BlackSquareColors[];
extern char          buf[];

extern TInfoWindow * TInfo;
extern HBRUSH        hBlackBrush;
extern HBRUSH        hWhiteBrush;
extern BOOL          ShowBestLine;
extern BOOL          Turned;
extern HWND          hWndMain;
extern HBITMAP       PieceBmpArray[][2];
extern HBITMAP       MaskArray[];
extern LEVELTYPE     Level;
extern double        AverageTime;
extern ENUMCOLOR     RunColor;
extern BYTE          MaxLevel;
extern ENUMCOLOR     ComputerColor;
extern int           BORDERSIZE;
extern short         CHARSIZE;
extern short         LINESIZE;
extern BOOL          Editing;
extern ENUMCOLOR     Player;
extern ENUMCOLOR     ProgramColor;
extern int           MainEvalu;
extern int           DirTab[];
extern int           KnightDir[];
extern short         Depth;
extern short         INFOXSIZE;
extern short         INFOYSIZE;
extern int           OfficerNo[];
extern int           PawnNo[];
extern MOVESTRUCT *    MovTab;
extern MOVESTRUCT      ZeroMove;
extern int           PieceValue[];
extern CLOCKTYPE     ChessTime[];
extern BOOL          MultiMove, AutoPlay, SingleStep;
extern int           MoveNo;
extern CLOCKTYPE     ChessClock;
extern BOOL          Analysis, Opan;
extern NODEVAL       Nodes;             
extern CASTMOVETYPE  CastMove[2][2];
extern int           RootValue;
extern MOVESTRUCT      KeyMove;
extern BOARDIDTYPE   Display[];
extern MOVESTRUCT      HintLine[MAXPLY];
extern int           HintEvalu;
extern MOVESTRUCT      Next;
extern MOVESTRUCT      PlayerMove;
extern BOOL          ComputerThinking;
extern MOVESTRUCT      MainLine[MAXPLY];
extern int           MaxDepth;
extern HCURSOR       hWaitCursor;
extern HMENU         ThinkMenu;
extern HMENU         MainMenu;
extern BOOL          Running;
extern BOOL          GotValidMove;
extern BOOL          GameOver;
extern char          EndGameMessage[];
extern BOOL          NoComputerMove;
extern BOOL          SoundOn;


extern void DragStart(int, POINT&);
extern void DragEnd(BOOL);
extern void Drag(POINT&);
extern short Attacks(ENUMCOLOR acolor, int square);
extern POINT GetSquareXY(int);
extern void ClearSquare(int square);
extern void DrawFrame(HDC, RECT&, BOOL=FALSE);
extern void GenCastSquare(int new1, int *castsquare,
                    int *cornersquare);
extern void CalcCastling(ENUMCOLOR incolor,  ENUMCASTDIR *cast);
extern void ClearBoard();
extern void ResetNewPos();
extern void Error(char *);
extern void ColorToPlay(ENUMCOLOR);
extern void InitDisplay();
extern void ClearDisplay();
extern void ClearInfoWindow();
extern void NewGame();
extern void InsertPiece(ENUMPIECE p, ENUMCOLOR c, int sq);
extern void PrintCurLevel();
extern void InitNode(NODEVAL*);
extern void InitTime(CLOCKTYPE *clock);
extern void StartTime(CLOCKTYPE *);
extern void StopTime(CLOCKTYPE *);
extern void PrintNodes(NODEVAL *, double);
extern void Perform(MOVESTRUCT *, BOOL);
extern void TakeBackMove(MOVESTRUCT *move);
extern void MakeMove(MOVESTRUCT *);
extern BOOL EqMove(MOVESTRUCT *a, MOVESTRUCT *b);
extern void IncNode(NODEVAL *nodes);
extern short PieceAttacks(ENUMPIECE, ENUMCOLOR, int, int);
extern unsigned char FiftyMoveCnt(void);
extern unsigned char Repetition(short immediate);
extern void PrintBestMove(MOVESTRUCT *mainline, int mainevalu);
extern void DisplayMove(BOOL *singlestep, short maxdepth, int evaluation, BOOL);
extern int StatEvalu(MOVESTRUCT *move);
extern short KillMovGen(MOVESTRUCT *move);
extern void CalcPVTable();
extern void EnterKeyMove();
extern void StartAnalysis();
extern void Warning(char *);
extern void PrintMove(int, ENUMCOLOR, MOVESTRUCT *, double);
extern void UpdateBoard();
extern void InitMovGen();
extern void MovGen();
extern void Message(char *);
extern void CalcPieceTab();
extern void ClearBestLine();
extern void ResetMoves();
extern void InitChessTime();
extern void ClearHint();
extern void ClearPVTable();
extern void AdjustMoves();
extern void FindMove(int maxlevel);
extern void OpeningLibMsg();
extern char *MoveStr(MOVESTRUCT *);
extern void ClearMessage();
extern void EnterMove(MOVESTRUCT *move);
extern void FlashMove(MOVESTRUCT *move);
extern void StoreMoves();
extern void CalcAttackTab();
extern void SaveGame(char *savefile);
extern void RestoreGame(char *);
extern BOOL Undo();
extern BOOL Redo();
extern void PrintBoard();
extern void FindHintMove();
extern void ShowHint();
extern void Talk();
extern int GetValidSquare(POINT, ENUMCOLOR, BOOL);
extern void DrawInvertedBitmap(int);
extern void DrawNormalBitmap(int);
extern void InitCommStuff();
extern BOOL MoveCheck(int, int);
extern void ProgramMove();
extern WORD MessageToPost;
extern void QuitProgram();
extern void HideAttacks();
extern void DisplayTime();
void DoSlideMove(MOVESTRUCT&);
void SlidePiece(int, int);
void GenCastSquare(int, int*, int*);

#endif // __EXTERNS_H
