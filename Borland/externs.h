/*
 * Copyright 2017 Alwin Leerling <alwin@jambo>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */

#ifndef __EXTERNS_H
#define __EXTERNS_H

#include "wcdefs.h"

extern ENUMCOLOR     Opponent;
extern PIECETAB      PieceTab[][16];
extern BOARDTYPE     Board[0x78];
extern char          WhiteSquareColors[];
extern char          BlackSquareColors[];
extern char          buf[];

/*
extern TChessWindow * main_window;
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

extern WORD         MessageToPost;


extern ENUMCOLOR     RunColor;
extern ENUMCOLOR     ComputerColor;
extern ENUMCOLOR     Player;
extern ENUMCOLOR     ProgramColor;
extern ENUMLEVEL     Level;
extern MOVESTRUCT *  MovTab;
extern MOVESTRUCT    ZeroMove;
extern MOVESTRUCT    KeyMove;
extern MOVESTRUCT    PlayerMove;
extern MOVESTRUCT    MainLine[MAXPLY];
extern double        Nodes;
extern double        AverageTime;
extern double		 WantedTime;
extern char          MaxLevel;
extern int           MainEvalu;
extern int           DirTab[];
extern int           KnightDir[];
extern short         Depth;
extern int           MoveNo;
extern int           RootValue;
extern int           HintEvalu;
extern int           MaxDepth;
extern char          EndGameMessage[];



/* timelib.cpp */

#endif // __EXTERNS_H
