/*$T WCDEFS.H GC 1.123 09/25/01 14:31:50 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __WCDEFS_H
#define __WCDEFS_H

/* Constants */
const   BACK = -104;
const   MAXPLY = 23;
const   LOSEVALUE = 0x7D00;
const   MATEVALUE = 0x7C80;
const   DEPTHFACTOR = 0x80;
const   COMMANDLENGTH = 10;
const   MAXLEN = 64;
const   SQUARE_SIZE = 36;
const   MAXBDSIZE = 8;
const   MYFRAMESIZE = 3;

#define MAXINT  32767

/* typedefs */
enum PIECETYPE { empty, king, queen, rook, bishop, knight, pawn };
enum COLORTYPE { white, black };
enum CASTDIRTYPE { zero, lng, shrt };
enum LEVELTYPE { normal, fullgametime, easygame, infinite, plysearch, matesearch, matching };

typedef int MAXTYPE;                    /* Evaluation type */
typedef int INDEXTYPE;
typedef int SQUARETYPE;
typedef int EDGESQUARETYPE;
typedef short DEPTHTYPE;
typedef unsigned char *LIBTYPE;         /* The opening library */
typedef char DIRTYPE;
typedef short SETOFPIECE;
typedef CASTDIRTYPE CASTTYPE;
typedef unsigned char FIFTYTYPE;
typedef unsigned char REPEATTYPE;
typedef char STR80[81];
typedef char COMMANDSTRING[COMMANDLENGTH];
typedef char STR6[6];
typedef char MAXSTRING[MAXLEN];

typedef struct {
    int     x;
    STR6    s;
} PICTURETYPE[3];

struct MOVETYPE {
    int         new1;                   /* new1 square */
    int         old;                    /* old square */
    short       spe;                    /* Indicates special move: case movepiece of king: castling pawn: e.p. capture
                                         * else : pawnpromotion */
    PIECETYPE   movpiece;               /* moving piece and evt. captured piece */
    PIECETYPE   content;                /* moving piece and evt. captured piece */
};

struct BOARDTYPE {
    PIECETYPE       piece;
    COLORTYPE       color;
    short           index;
    unsigned char   attacked;
};

typedef MOVETYPE    LINETYPE[MAXPLY];   /* Principal variation type */

struct CLOCKTYPE {
    int     hour;
    int     minute;
    int     second;
    int     sec100;
    int     totalhours;
    int     totmin;
    int     totalsecs;
    int     tots100;
    int     number;
    double  totaltime;
};

struct NODEVAL {
    unsigned long   nodebase;
    unsigned long   nodeoffset;
};

struct TEXTCOLORTYPE {
    unsigned char   background;
    unsigned char   color;
};

struct SCREENPOSTYPE {
    unsigned char   x;
    unsigned char   y;
    unsigned char   background;
    unsigned char   color;
};

struct BOARDIDTYPE {
    PIECETYPE   piece;
    COLORTYPE   color;
};

struct PIECETAB {
    SQUARETYPE  isquare;
    PIECETYPE   ipiece;
};

struct ATTACKTABTYPE {
    SETOFPIECE  pieceset;               /* A set of king..pawn. gives the pieces, which can move to the square */
    int         direction;              /* The direction from the piece to the square */
};

struct CASTMOVETYPE {
    SQUARETYPE  castnew;
    SQUARETYPE  castold;
};

#endif // __WCDEFS_H
