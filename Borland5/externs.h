// ObjectWindows - (C) Copyright 1992 by Borland International

#ifndef __EXTERNS_H
#define __EXTERNS_H

#include "wcdefs.h"
#include "info.h"

extern COLORTYPE Opponent;
extern PIECETAB PieceTab[][16];
extern BOARDTYPE Board[0x78];
extern BYTE WhiteSquareColors[];
extern BYTE BlackSquareColors[];
extern char buf[];

extern PTInfoWindow TInfo;
extern HBRUSH hBlackBrush;
extern HBRUSH hWhiteBrush;
extern BOOL ShowBestLine;
extern BOOL Turned;
extern HWND hWndMain;
extern HBITMAP PieceBmpArray[][2];
extern HBITMAP MaskArray[];
extern LEVELTYPE Level;
extern double AverageTime;
extern COLORTYPE RunColor;
extern BYTE MaxLevel;
extern COLORTYPE ComputerColor;
extern int BORDERSIZE;
extern short CHARSIZE;
extern short LINESIZE;
extern BOOL Editing;
extern COLORTYPE Player;
extern COLORTYPE ProgramColor;
extern MAXTYPE MainEvalu;
extern int PVTable[2][7][0x78];
extern int DirTab[];
extern int KnightDir[];
extern int PawnDir[];
extern DEPTHTYPE Depth;
extern short INFOXSIZE, INFOYSIZE;
extern int OfficerNo[];
extern int PawnNo[];
extern MOVETYPE *MovTab;
extern MOVETYPE ZeroMove;
extern int PieceValue[];
extern CLOCKTYPE ChessTime[];
extern BOOL MultiMove, AutoPlay, SingleStep;
extern int MoveNo;
extern CLOCKTYPE ChessClock;
extern BOOL Analysis, Opan;
extern NODEVAL Nodes;
extern CASTMOVETYPE CastMove[2][2];
extern MAXTYPE RootValue;
extern MOVETYPE KeyMove;
extern BOARDIDTYPE Display[];
extern LINETYPE HintLine;
extern MAXTYPE HintEvalu;
extern MOVETYPE Next;
extern MOVETYPE PlayerMove;
extern BOOL ComputerThinking;
extern LINETYPE MainLine;
extern int MaxDepth;
extern HCURSOR hWaitCursor;
extern HMENU ThinkMenu;
extern HMENU MainMenu;
extern BOOL Running;
extern BOOL GotValidMove;
extern BOOL GameOver;
extern char EndGameMessage[];
extern BOOL NoComputerMove;
extern BOOL SoundOn;
extern WORD MessageToPost;


/* board.cpp */
extern void GenCastSquare(SQUARETYPE new1, SQUARETYPE *castsquare, SQUARETYPE *cornersquare);
extern void ClearBoard();
extern void Perform(MOVETYPE *, BOOL);
extern BOOL EqMove(MOVETYPE *a, MOVETYPE *b);
extern void CalcPieceTab();
extern void GenCastSquare(SQUARETYPE, SQUARETYPE*, SQUARETYPE*);

/* colors.cpp */

/* display.cpp */
extern void ClearDisplay();
extern void ClearSquare(SQUARETYPE square);
extern void ClearInfoWindow();
extern void DrawFrame(HDC, RECT&, BOOL=FALSE);
extern POINT GetSquareXY(SQUARETYPE);
extern void Error(char *);
extern void ColorToPlay(COLORTYPE);
extern void InitDisplay();
extern void PrintBestMove(MOVETYPE *mainline, MAXTYPE mainevalu);
extern void PrintCurLevel();
extern void PrintNodes(NODEVAL *, double);
extern void Warning(char *);
extern void PrintMove(int, COLORTYPE, MOVETYPE *, double);
extern void UpdateBoard();
extern void ClearBestLine();
extern void OpeningLibMsg();
extern char *MoveStr(MOVETYPE *);
extern void ClearMessage();
extern void PrintBoard();
extern SQUARETYPE GetValidSquare(POINT, COLORTYPE, BOOL);
extern void DrawInvertedBitmap(SQUARETYPE);
extern void DrawNormalBitmap(SQUARETYPE);
extern void HideAttacks();
extern void Message(char *);

/* drag.cpp */
extern void DragStart(SQUARETYPE, POINT&);
extern void DragEnd(BOOL);
extern void Drag(POINT&);
void SlidePiece(SQUARETYPE, SQUARETYPE);

/* edit.cpp */

/* evalu.cpp */
extern void CalcPVTable();
extern int StatEvalu(MOVETYPE *move);

/* info.cpp */

/* movegen.cpp */
extern short Attacks(COLORTYPE acolor, SQUARETYPE square);
extern short PieceAttacks(PIECETYPE, COLORTYPE, SQUARETYPE, SQUARETYPE);
extern void CalcCastling(COLORTYPE incolor,  CASTDIRTYPE *cast);
extern void CalcAttackTab();
extern FIFTYTYPE FiftyMoveCnt(void);
extern REPEATTYPE Repetition(short immediate);
extern short KillMovGen(MOVETYPE *move);
extern void InitMovGen();
extern void MovGen();

/* pvalue.cpp */

/* saverest.cpp */
extern void SaveGame(char *savefile);
extern void RestoreGame(char *);

/* search.cpp */
extern void FindMove(int maxlevel);

/* small.cpp */
extern void InitNode(NODEVAL*);
extern void TakeBackMove(MOVETYPE *move);
extern void MakeMove(MOVETYPE *);
extern void IncNode(NODEVAL *nodes);
extern void EnterKeyMove();
extern void StartAnalysis();
extern void ResetMoves();
extern void InitChessTime();
extern void ClearHint();
extern void AdjustMoves();
extern void EnterMove(MOVETYPE *move);
extern void FlashMove(MOVETYPE *move);
extern void StoreMoves();
extern void ShowHint();
extern BOOL MoveCheck(SQUARETYPE, SQUARETYPE);
extern void DoSlideMove(MOVETYPE&);

/* talk.cpp */
extern void ResetNewPos();
extern void NewGame();
extern void InsertPiece(PIECETYPE p, COLORTYPE c, SQUARETYPE sq);
extern void ClearPVTable();
extern BOOL Undo();
extern BOOL Redo();
extern void FindHintMove();
extern void Talk();
extern void ProgramMove();
extern void QuitProgram();

/* timelib.cpp */
extern void InitTime(CLOCKTYPE *clock);
extern void StartTime(CLOCKTYPE *);
extern void StopTime(CLOCKTYPE *);
extern void DisplayTime();

/* wchess.cpp */

#endif // __EXTERNS_H
