// ObjectWindows - (C) Copyright 1992 by Borland International

#ifndef __WCDEFS_H
#define __WCDEFS_H

/*
 *  Constants
 */

const BACK           =  -104;
const MAXPLY         =  23;
const LOSEVALUE      = 0x7D00;
const MATEVALUE      = 0x7C80;
const DEPTHFACTOR    = 0x80;
const COMMANDLENGTH  = 10;
const MAXLEN         = 64;
const SQUARE_SIZE    = 36;
const MAXBDSIZE      = 8;
const MYFRAMESIZE    = 3;

#define     MAXINT      32767

/*
 *  enums
 */

enum ENUMPIECE   { empty, king, queen, rook, bishop, knight, pawn };
enum ENUMCOLOR   { white, black };
enum ENUMCASTDIR { zero, lng, shrt };
enum LEVELTYPE   { normal, fullgametime, easygame, infinite, plysearch, matesearch, matching };

/*
 *  typedefs
 */


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

struct BOARDIDTYPE {
    ENUMPIECE piece;
    ENUMCOLOR color;
};

struct  BOARDTYPE {
	ENUMPIECE     piece;
	ENUMCOLOR     color;
	short         index;
	unsigned char attacked;
};

struct CLOCKTYPE {
	int hour;
	int minute;
	int second;
	int sec100;
	int totalhours;
	int totmin;
	int totalsecs;
	int tots100;
	int number;
	double totaltime;
};

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

typedef struct {
    int       x;
    char      s[ 6 ];
} PICTURETYPE[3];

struct PIECETAB {
   int       isquare;
   ENUMPIECE ipiece;
};

struct ATTACKTABTYPE {
    short pieceset;			/*  A set of king..pawn.  gives the pieces, which can move to the square  */
    int   direction;					/*  The direction from the piece to the square  */
};

struct CASTMOVETYPE {
    int   castnew;
    int   castold;
};
    


#endif  // __WCDEFS_H
