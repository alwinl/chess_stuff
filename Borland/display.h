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


#ifndef DISPLAY_H_INCLUDED
#define DISPLAY_H_INCLUDED

HBITMAP GetBitmapHandle( ENUMPIECE piece, ENUMCOLOR pcolor );
void ClearInfoWindow();
void ColorToPlay( ENUMCOLOR color );
void Message( char *str );
void Error( char *str );
void Warning( char *str );
char *MoveStr( MOVESTRUCT *amove );
void PrintMove( int moveno, ENUMCOLOR programcolor, MOVESTRUCT *amove, double time );
void DrawFrame( HDC hDC, RECT& frame_area, bool DrawBackground );
void PrintCurLevel();
POINT GetSquareXY( int square );
void ClearSquare( int square );
void ClearDisplay();
void InitDisplay();
void HideAttacks( void );
void ShowAttacks();
void UpdateBoard();
void PrintBoard();
int GetValidSquare( POINT p );
void DrawNormalBitmap( int square );
void DrawInvertedBitmap( int square );
void OpeningLibMsg();
void PrintNodes( double nodes, double time );
void PrintBestMove( MOVESTRUCT *mainline, int mainvalue );
void ClearBestLine();


#endif // DISPLAY_H_INCLUDED
