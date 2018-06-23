// ObjectWindows - (C) Copyright 1992 by Borland International

#ifndef __WCDEFS_H
#define __WCDEFS_H

/*
 *  Constants
 */
const BACK           = -104;
const MAXPLY         = 23;
const LOSEVALUE      = 0x7D00;
const MATEVALUE      = 0x7C80;
const DEPTHFACTOR    = 0x80;
const COMMANDLENGTH  = 10;
const MAXLEN         = 64;
const SQUARE_SIZE    = 36;
const MAXBDSIZE      = 8;
const MYFRAMESIZE    = 3;
const MAXINT         = 32767;

/*
 *  typedefs
 */
typedef int MAXTYPE;  /*  Evaluation type  */
typedef int INDEXTYPE;
typedef int SQUARETYPE;
typedef int EDGESQUARETYPE;
typedef short DEPTHTYPE;
typedef unsigned char *LIBTYPE;      /*  The opening library  */
typedef char DIRTYPE;
typedef short SETOFPIECE;
typedef unsigned char FIFTYTYPE;
typedef unsigned char REPEATTYPE;
typedef char STR80[81];
typedef char COMMANDSTRING[COMMANDLENGTH];
typedef char STR6[6];
typedef char MAXSTRING[MAXLEN];

typedef MOVETYPE LINETYPE[MAXPLY];  /*  Principal variation type  */

enum PIECETYPE { no_piece, king, queen, rook, bishop, knight, pawn };
enum COLORTYPE { white, black };
enum CASTTYPE { zero, lng, shrt };
enum LEVELTYPE { normal, fullgametime, easygame, infinite, plysearch, matesearch, matching };



struct MOVETYPE
{
    int new1, old;  /*  new1 and old square  */
    short spe;           /*  Indicates special move: if movepiece is king: castling pawn: e.p. capture else : pawnpromotion  */
    PIECETYPE movpiece, content;  /* moving piece and evt. captured piece  */
};

struct BOARDTYPE
{
    PIECETYPE piece;
    COLORTYPE color;
    short index;
    unsigned char attacked;
};

struct CLOCKTYPE
{
    int hour, minute, second, sec100;
    int totalhours, totmin, totalsecs, tots100;
    int number;
    double totaltime;
};

struct NODEVAL
{
    unsigned long nodebase;
    unsigned long nodeoffset;
};

struct TEXTCOLORTYPE
{
    unsigned char background;
    unsigned char color;
};

struct SCREENPOSTYPE
{
    unsigned char x, y;
    unsigned char background, color;
};

struct BOARDIDTYPE
{
    PIECETYPE piece;
    COLORTYPE color;
};

struct PIECETAB
{
    SQUARETYPE isquare;
    PIECETYPE ipiece;
};


struct ATTACKTABTYPE
{
    SETOFPIECE pieceset;    /*  A set of king..pawn.  gives the pieces, which can move to the square */
    int direction;          /*  The direction from the piece to the square  */
};

struct CASTMOVETYPE
{
    SQUARETYPE castnew;
    SQUARETYPE castold;
};



#endif  // __WCDEFS_H
