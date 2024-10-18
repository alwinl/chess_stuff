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

#ifndef SEARCH_H_INCLUDED
#define SEARCH_H_INCLUDED

struct PIECETAB {
    int isquare;
    ENUMPIECE ipiece;
};

extern PIECETAB      PieceTab[][16];
extern ENUMCOLOR     Opponent;
extern ENUMCOLOR     Player;
extern short         Depth;
extern int           MainEvalu;
extern MOVESTRUCT *  MovTab;

void FindMove( int maxlevel );


void clear_piecetab( ENUMCOLOR color );
void set_piecetab_piece( ENUMCOLOR color, short index, ENUMPIECE piece );
void set_piecetab_square( ENUMCOLOR color, short index, int square );
int get_king_square( ENUMCOLOR color );


#endif // SEARCH_H_INCLUDED
