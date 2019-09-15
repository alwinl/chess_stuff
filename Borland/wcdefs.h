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

#ifndef __WCDEFS_H
#define __WCDEFS_H

/*
 *  Constants
 */
const int BACK           = -104;
const int MAXPLY         = 23;
const int LOSEVALUE      = 0x7D00;
const int MATEVALUE      = 0x7C80;
const int DEPTHFACTOR    = 0x80;
const int COMMANDLENGTH  = 10;
const int MAXLEN         = 64;
const unsigned int SQUARE_SIZE    = 36;
const unsigned int MAXBDSIZE      = 8;
const unsigned int MYFRAMESIZE    = 3;
const int MAXINT         = 32767;

/*
 *  enums
 */
enum ENUMPIECE { no_piece, king, queen, rook, bishop, knight, pawn };
enum ENUMCOLOR { white, black };
enum ENUMCASTDIR { zero, lng, shrt };
enum ENUMLEVEL { normal, fullgametime, easygame, infinite, plysearch, matesearch, matching };

/*
 *  structs
 */
struct MOVESTRUCT {
    int       new1;      /* old square  */
    int       old;       /* new1 square  */
    short     spe;       /* Indicates special move: case movepiece of king: castling pawn: e.p. capture else : pawnpromotion  */
    ENUMPIECE movpiece;  /* moving piece  */
    ENUMPIECE content;   /* evt. captured piece  */
};

struct BOARDTYPE {
    ENUMPIECE     piece;
    ENUMCOLOR     color;
    short         index;
    unsigned char attacked;
};

/*
struct NODEVAL {
    unsigned long nodebase;
    unsigned long nodeoffset;
};

struct TEXTCOLORTYPE {
    unsigned char background;
    unsigned char color;
};

struct SCREENPOSTYPE {
    unsigned char x;
    unsigned char y;
    unsigned char background;
    unsigned char color;
};
*/

struct PIECETAB {
    int isquare;
    ENUMPIECE ipiece;
};

struct ATTACKTABTYPE {
    short  pieceset;               /* A set of king..pawn. gives the pieces, which can move to the square */
    int    direction;              /* The direction from the piece to the square */
};

struct CASTMOVETYPE {
    int castnew;
    int castold;
};

#endif // __WCDEFS_H
