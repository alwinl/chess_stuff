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

#ifndef BOARD_H_INCLUDED
#define BOARD_H_INCLUDED

struct BOARDTYPE {
    ENUMPIECE     piece;
    ENUMCOLOR     color;
    short         index;
    unsigned char attacked;
};

extern BOARDTYPE     Board[0x78];


void ClearBoard( void );
void CalcPieceTab( void );
void GenCastSquare( int newSquare, int *castsquare, int *cornersquare );

void InsertPiece( ENUMPIECE p, ENUMCOLOR c, int sq );

void SetMove( MOVESTRUCT *amove );
void ResetMove( MOVESTRUCT *amove );
bool EqMove( MOVESTRUCT *a, MOVESTRUCT *b );

int get_total_piece_count( ENUMCOLOR color );
int get_officer_count( ENUMCOLOR color );

#endif // BOARD_H_INCLUDED
