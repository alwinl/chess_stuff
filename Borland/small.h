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

#ifndef SMALL_H_INCLUDED
#define SMALL_H_INCLUDED

void TakeBackMove( MOVESTRUCT *amove );
void MakeMove( MOVESTRUCT * );
void EnterKeyMove( void );
void StartAnalysis( void );
void ResetMoves( void );
void InitChessTime( void );
void AdjustMoves( void );
void EnterMove( MOVESTRUCT *amove );
void FlashMove( MOVESTRUCT *amove );
void StoreMoves( void );
bool MoveCheck( int, int );
void DoSlideMove( MOVESTRUCT & );

#endif // SMALL_H_INCLUDED
