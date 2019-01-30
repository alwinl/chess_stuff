// OWLCVT 08/22/97 02:11:38
// ObjectWindows - (C) Copyright 1992 by Borland International

#include <owl\owlpch.h>
#pragma hdrstop

#include <math.h>
#include "wcdefs.h"
#include "externs.h"

#undef max
#undef min
#define max(a, b)  (((a) > (b)) ? (a) : (b))
#define min(a, b)  (((a) < (b)) ? (a) : (b))
#define VALIDSQUARE( square )  ( !( (square) & 0x88 ) )

#define TOLERANCE           8   /*  Tolerance width  */
#define EXCHANGEVALUE      32	/*  Value for exchanging pieces when ahead (not pawns)  */
#define ISOLATEDPAWN       20	/*  Isolated pawn.  Double isolated pawn is 3 * 20  */
#define DOUBLEPAWN          8   /*  Double pawn  */
#define SIDEPAWN            6   /*  Having a pawn on the side  */
#define CHAINPAWN           3   /*  Being covered by a pawn  */
#define COVERPAWN           3   /*  covering a pawn  */
#define NOTMOVEPAWN         2   /*  Penalty for moving pawn  */
#define BISHOPBLOCKVALUE   20   /*  Penalty for bishop blocking d2/e2 pawn  */
#define ROOKBEHINDPASSPAWN 16   /*  Bonus for Rook behind passed pawn  */

struct PAWNBITTYPE {
    unsigned char one;
    unsigned char dob;
};

/*
 *  external variables
 */

int                 RootValue;
BOARDTYPE           Board[ 0x78 ];
int                 PositionValueTable[2][7][0x78];

/*
 *  Global variables for this module
 */

static int          PieceValue[7]          = { 0, 0x1000, 0x900, 0x4c0, 0x300, 0x300, 0x100 };
const char          pawnrank[8]            = { 0, 0, 0, 2, 4, 8, 30, 0 };
const char          passpawnrank[8]        = { 0, 0, 10, 20, 40, 60, 70, 0 };
const char          pawnfilefactor[8]      = { 0, 0, 2, 5, 6, 2, 0, 0 };

const unsigned char filebittab[8]          = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
const unsigned char squarerankvalue[8]     = { 0, 0, 0, 0, 1, 2, 4, 4 };

PAWNBITTYPE         pawnbitt[2][MAXPLY + 2];

int                 totalmaterial;
int                 pawntotalmaterial;
int                 material;
int                 materiallevel;	/*  Material level of the game (early middlegame = 43 - 32, endgame = 0)  */





/*
 *  count the number of set bits in b
 */
static int count( unsigned char b )
{
    char cnt = 0;

    while( b ) {
        if( b % 2 == 1 )
            cnt++;
        b >>= 1;
    }
    return cnt;
}


/*
 * Initialize pawnbitt
 */
static void InitPawnbitt( ENUMCOLOR color )
{
    pawnbitt[color][0].one = 0;
    pawnbitt[color][0].dob = 0;

    for( int sq_rank = 1; sq_rank < 7; sq_rank++ ) {
        pawnbitt[color][0].dob |= ( pawnbitt[color][0].one & PawnTable[color][sq_rank] );
        pawnbitt[color][0].one |= PawnTable[color][sq_rank];
    }
}

/*
 *  Calculate value of the pawn structure in pawnbitt[depth][color]
 */
static int pawnstrval( ENUMCOLOR color, short depth )
{
    unsigned char iso;  /*  contains FILEs with isolated pawns  */

    iso = pawnbitt[color][depth].one & ~( ( pawnbitt[color][depth].one << 1 ) | ( pawnbitt[color][depth].one >> 1 ) );

    return ( -( count( pawnbitt[color][depth].dob ) * DOUBLEPAWN
               + count( iso ) * ISOLATEDPAWN
               + count( iso & pawnbitt[color][depth].dob ) * ISOLATEDPAWN * 2 )
    );
}

/*
 *  Update pawnbitt and calculates value when a pawn is removed from line
 */
static int decpawnstrval( ENUMCOLOR color, unsigned char line )
{
    pawnbitt[color][ Depth+1 ].one &= ~filebittab[ line ];
    pawnbitt[color][ Depth+1 ].one |= pawnbitt[color][ Depth+1 ].dob;

    pawnbitt[color][ Depth+1 ].dob &= ~filebittab[ line ];

    return( pawnstrval( color, Depth+1 ) - pawnstrval( color, Depth ) );
}

/*
 *  Update pawnbitt and calculates value when a pawn moves from old to new1 file
 */
static int movepawnstrval( ENUMCOLOR color, unsigned char newLine, unsigned char oldLine )
{
    pawnbitt[color][Depth + 1].dob  |= ( pawnbitt[color][ Depth + 1].one &  filebittab[ newLine ] );

    pawnbitt[color][Depth + 1].one   = ( pawnbitt[color][ Depth + 1].one & ~filebittab[ oldLine ] );
    pawnbitt[color][Depth + 1].one  |= ( pawnbitt[color][ Depth + 1].dob |  filebittab[ newLine ] );

    pawnbitt[color][Depth + 1].dob  &= ~filebittab[ oldLine ];

    return( pawnstrval( color, Depth+1 ) - pawnstrval( color, Depth ) );
}

static void CalcPawnBit( ENUMCOLOR color)
{
    pawnbitt[color][ Depth + 1 ] = pawnbitt[color][ Depth ];  /*  calculate pawnbitt  */
}





/*
 *  calculate the value of the piece on the square
 */
inline int PiecePosVal( ENUMPIECE piece, ENUMCOLOR color, int square )
{
    return ( PieceValue[piece] + PositionValueTable[color][piece][square] );
}

/*
 *  calculates piece-value table for the static evaluation function
 */
void ClearPVTable( void )
{
    for( ENUMPIECE piece = king; piece <= pawn; ( ( int )piece )++ )
        for( int square = 0; square <= 0x77; square++ ) {
            PositionValueTable[ white ][ piece ][ square ] = 0;
            PositionValueTable[ black ][ piece ][ square ] = 0;
        }
}

/*
 *  calculates piece-value table for the static evaluation function
 */
void CalcPVTable( void )
{
    unsigned char PawnTable[2][8];
    unsigned char pawnfiletab, bit, oppasstab, behindoppass,
             leftsidetab, rightsidetab, sidetab, leftchaintab,
             rightchaintab, chaintab, leftcovertab, rightcovertab;		/*  Bit tables for static pawn structure evaluation  */
    /*  Importance of an attack of the square  */
    char          attackvalue[2][0x78];											/*  Importance of an attack of the square  */
    int           pvcontrol[2][5][0x78];										/*  Value of squares controlled from the square  */
    ENUMCOLOR     losingcolor;												/*  the color which is being mated  */
    int           posval;													/*  The positional value of piece  */
    int           attval;													/*  The attack value of the square  */
    unsigned char line;														/*  The file of the piece  */
    unsigned char rank;														/*  The rank of the piece  */
    char          dist, kingdist;											/*  Distance to center, to opponents king */
    ENUMCASTDIR   cast;														/*  Possible castlings  */
    short         direct;													/*  Indicates direct attack  */
    int           cnt;														/*  Counter for attack values  */
    int           strval;													/*  Pawnstructure value  */
    ENUMCOLOR     color, oppcolor;											/*  Color and opponents color  */
    ENUMPIECE     piece;												/*  Piece counter  */
    int           square;													/*  Square counter  */
    char          dir;														/*  Direction counter  */
    int           sq;														/*  Square counter  */
    const char    distan[8] = { 3, 2, 1, 0, 0, 1, 2, 3 };	                /*  The value of a pawn is the sum of Rank and file values. The file value is equal to PawnFileFactor * (Rank Number + 2) */
    bool          mating;                                                   /*  mating evaluation function is used  */

    /*  Calculate material values  */
    material = pawntotalmaterial = totalmaterial = mating = 0;

    for( int square = 0; square < 0x78; square++ ) {
        if( VALIDSQUARE( square ) && Board[square].piece != no_piece && Board[square].piece != king ) {

            if( Board[square].piece == pawn )
                pawntotalmaterial += PieceValue[ pawn ];

            if( Board[square].color == white )
                material += PieceValue[ Board[square].piece ];
            else
                material -= PieceValue[ Board[square].piece ];

            totalmaterial += PieceValue[ Board[square].piece ];
        }
    }

    materiallevel = max( 0, totalmaterial - 0x2000 ) / 0x100;

    losingcolor = ( ( material < 0 ) ? white : black );

    /*  Set mating if weakest player has less than the equivalence of two bishops and the advantage is at least a rook for a bishop  */
    mating      = ( ( totalmaterial - abs( material ) ) / 2 <= 2 * PieceValue[ bishop ] ) && ( abs( material ) >= PieceValue[ rook ] - PieceValue[ bishop ] );

    /*  Calculate attack values of each square (importance of each square)  */
    for( rank = 0; rank < 8; rank++ ) {
        for( line = 0; line < 8; line++ ) {

            square = ( rank << 4 ) + line;

            attval = max( 0, 8 - 3 * ( distan[ rank ] + distan[ line ] ) ); /*  centre importance */

            attackvalue[ white ][ square        ] =
            attackvalue[ black ][ square ^ 0x70 ] = ( ( squarerankvalue[ rank ] * 3 * ( materiallevel + 8 ) ) >> 5 ) + attval;	/*  Rank importance  */

        }
    }

    /*  Add importance of the 8 squares around the opponent's King  */
    for( color = white; color <= black; ( ( int )color )++ ) {

        oppcolor = ( ENUMCOLOR )( 1 - color );
        CalcCastling( oppcolor, &cast );

        if( cast != shrt && materiallevel > 0 ) {
            for( dir = 0; dir < 8; dir++ ) {
                square = PieceTab[ oppcolor ][ 0 ].isquare + DirTab[dir];

                if( VALIDSQUARE( square ) )
                    attackvalue[ color ][ square ] += ( ( 12 * ( materiallevel + 8 ) ) >> 5 );
            }
        }
    }

    /*  Initialise PVControl  */
    for( square = 0x77; square >=0; square-- ) {
        if( VALIDSQUARE( square ) ) {
            for( piececount = rook; piececount <= bishop; ( ( int )piececount )++ ) {
                pvcontrol[ white ][ piececount ][ square ] = 0;
                pvcontrol[ black ][ piececount ][ square ] = 0;
            }
        }
    }

    /*  Calculate PVControl  */
    for( square = 0x77; square >=0; square-- ) {
        if( VALIDSQUARE( square ) ) {
            for( color = white; color <= black; ( ( int )color )++ ) {
                for( dir = 7; dir >= 0; dir-- ) {

                    piece = ( dir < 4 ) ? rook : bishop;	 // rook attacks vertical and horizontal, bishop attacks diagonal, queen from all directions

                    /*  Count value of all attacks from the square in the Direction.
                        The Value of attacking a Square is Found in ATTACKVAL.
                        Indirect Attacks (e.g. a Rook attacking through
                        another Rook) counts for a Normal attack,
                        Attacks through another Piece counts half  */

                    cnt = 0;
                    sq = square;
                    direct = 1;
                    do {
                        sq += DirTab[ dir ];
                        if( !VALIDSQUARE( sq ) )			// end of the board reached?
                            break;

                        cnt += ( direct ? attackvalue[ color ][ sq ] : ( attackvalue[ color ][ sq ] >> 1 ) );

                        if( ( Board[ sq ].piece != no_piece ) && ( ( Board[ sq ].piece != piece ) && ( Board[ sq ].piece != queen ) ) )
                            direct = 0;

                    } while( Board[ sq ].piece != pawn );

                    pvcontrol[ color ][ piece ][ square ] += ( cnt >> 2 );
                }
            }
        }

        /*  Calculate the position value table, value by value  */
        for( square = 0x77; square >= 0; square-- ) {
            if( VALIDSQUARE( square ) ) {

                for( color = white; color <= black; ( ( int )color )++ ) {

                    oppcolor = ( ENUMCOLOR )( 1 - color );
                    line     = square & 7;
                    rank     = ( ( color == black ) ? ( 7 - ( square >> 4 ) ) : ( square >> 4 ) );
                    dist     = distan[ rank ] + distan[ line ];
                    kingdist = abs( ( square >> 4 ) - ( PieceTab[ oppcolor ][ 0 ].isquare >> 4 ) ) + ( ( square - PieceTab[ oppcolor ][ 0 ].isquare ) & 7 );

                    for( piece = king; piece <= pawn; ( ( int )piece )++ ) {

                        posval = 0;												/*  Calculate POSITIONAL Value for the piece on the Square  */

                        if( mating && ( piece != pawn ) ) {
                            if( piece == king )
                                if( color == losingcolor ) {					/*  Mating evaluation  */
                                    posval = 128 - 16 * distan[ rank ] - 12 * distan[line];
                                    if( distan[ rank ] == 3 )
                                        posval -= 16;
                                } else {
                                    posval = 128 - 4 * kingdist;
                                    if( ( distan[ rank ] >= 2 ) || ( distan[ line ] == 3 ) )
                                        posval -= 16;
                                }
                        } else {
                            switch( piece ) {								/*  Normal evaluation function  */
                            case king :
                                if( materiallevel <= 0 )
                                    posval = -2 * dist;
                                break;

                            case queen :
                                posval = ( pvcontrol[ color ][ rook ][ square ] + pvcontrol[ color ][ bishop ][ square ] ) >> 2;
                                break;

                            case rook :
                                posval = pvcontrol[ color ][ rook ][ square ];
                                break;

                            case bishop :
                                posval = pvcontrol[ color ][ bishop ][ square ];
                                break;

                            case knight :
                                cnt = 0;
                                for( dir = 0; dir < 8; dir++ ) {
                                    sq = square + KnightDir[dir];
                                    if( VALIDSQUARE( sq ) )
                                        cnt += attackvalue[color][sq];
                                }
                                posval = ( cnt >> 1 ) - dist * 3;
                                break;

                            case pawn :
                                if( ( rank != 0 ) && ( rank != 7 ) )
                                    posval = pawnrank[ rank ] + pawnfilefactor[ line ] * ( rank + 2 ) - 12;
                                break;
                            }
                        }

                        PositionValueTable[ color ][ piece ][ square ] = posval;
                    }
                }
            }
        }
    }

    /*  Calculate PawnTable (indicates which squares contain pawns )  */
    for( rank = 0; rank < 8; rank++ ) {
        PawnTable[ white ][ rank ] = 0;
        PawnTable[ black ][ rank ] = 0;
    }

    for( square = 0x77; square >= 0; square-- ) {
        if( VALIDSQUARE( square ) && ( Board[ square ].piece == pawn ) ) {
            if( Board[ square ].color == black ) {
                rank = (square ^ 0x70) >> 4;
            else
                rank = square >> 4;

            PawnTable[ Board[square].color ][rank] |= filebittab[ square & 7 ];
        }
    }

    InitPawnbitt( white );
    InitPawnbitt( black );

    /*  Calculate pawnstructurevalue  */
    RootValue = pawnstrval( Player, 0 ) - pawnstrval( Opponent, 0 );

    /*  Calculate static value for pawn structure  */
    for( color = white; color <= black; ( ( int )color )++ ) {

        oppcolor     = ( ENUMCOLOR )( 1 - color );
        pawnfiletab  = 0;
        leftsidetab  = 0;
        rightsidetab = 0;
        behindoppass = 0;
        oppasstab    = 0xff;

        for( rank = 1; rank < 7; rank++ ) {

            oppasstab    &= ( ~( pawnfiletab | leftsidetab | rightsidetab ) );	/*  Squares where opponents pawns are passed pawns  */
            behindoppass |= ( oppasstab & PawnTable[ oppcolor ][ 7 - rank ] );	/*  Squares behind the opponents passed pawns  */

            leftchaintab  = leftsidetab;										/*  squares which are covered by a pawn  */
            rightchaintab = rightsidetab;

            pawnfiletab   = PawnTable[ color ][ rank ];							/*  squares w/ pawns  */

            leftsidetab   = ( pawnfiletab << 1 ) & 0xff;							/*  squares w/ a pawn beside them  */
            rightsidetab  = ( pawnfiletab >> 1 ) & 0xff;

            sidetab       = leftsidetab | rightsidetab;
            chaintab      = leftchaintab | rightchaintab;

            leftcovertab  = ( PawnTable[ color ][ rank + 1 ] << 1 ) & 0xff;
            rightcovertab = ( PawnTable[ color ][ rank + 1 ] >> 1 ) & 0xff;

            sq            = ( ( color == black ) ? ( rank << 4 ) ^ 0x70 : ( rank << 4 ) );

            while( bit ) {

                strval = 0;

                if( bit & sidetab )
                    strval = SIDEPAWN;
                else if( bit & chaintab )
                    strval = CHAINPAWN;

                if( bit & leftcovertab )
                    strval += COVERPAWN;

                if( bit & rightcovertab )
                    strval += COVERPAWN;

                if( bit & pawnfiletab )
                    strval += NOTMOVEPAWN;

                PositionValueTable[ color ][ pawn ][ sq ] += strval;

                if( ( materiallevel <= 0 ) || ( oppcolor != ProgramColor ) ) {

                    if( bit & oppasstab )
                        PositionValueTable[ oppcolor ][ pawn ][ sq ] += passpawnrank[ 7 - rank ];

                    if( bit & behindoppass ) {

                        PositionValueTable[ black ][ rook ][ sq ] += ROOKBEHINDPASSPAWN;
                        PositionValueTable[ white ][ rook ][ sq ] += ROOKBEHINDPASSPAWN;

                        if( rank == 6 ) {
                            PositionValueTable[ black ][ rook ][ sq ^ 0x10 ] += ROOKBEHINDPASSPAWN;
                            PositionValueTable[ white ][ rook ][ sq ^ 0x10 ] += ROOKBEHINDPASSPAWN;
                        }
                    }
                }
                sq++;
                bit = ( bit << 1 ) & 0xff;
            }
        }
    }

    /*  Calculate penalty for blocking centre pawns with a bishop  */
    for( sq = 3; sq < 5; sq ++ ) {
        if( ( Board[ sq + 0x10 ].piece == pawn ) && ( Board[ sq + 0x10 ].color == white ) )
            PositionValueTable[ white ][ bishop ][ sq + 0x20 ] -= BISHOPBLOCKVALUE;

        if( ( Board[ sq + 0x60 ].piece == pawn ) && ( Board[ sq + 0x60 ].color == black ) )
            PositionValueTable[ black ][ bishop ][ sq + 0x50 ] -= BISHOPBLOCKVALUE;
    }

    for( square = 0x77; square >= 0; square-- ) {                                       /*  Calculate RootValue  */
        if( VALIDSQUARE( square ) && ( Board[ square ].piece != no_piece ) )
            if( Board[ square ].color == Player )
                RootValue += PiecePosVal( Board[ square ].piece, Player, square );
            else
                RootValue -= PiecePosVal( Board[ square ].piece, Opponent, square );
    }
}

/****************************************************************************/


/****************************************************************************/

/*
 *  Calculate STATIC evaluation of the move
 */

int StatEvalu( MOVESTRUCT *amove )
{
    const char          castvalue[2]           = { 4, 32 };  /*  Value of castling  */
    const int ShortCastleValue = 32;
    const int LongCastleValue = 4;

    int value = 0;

    if( amove->spe ) {
        if( amove->movpiece == king ) {

            int castsquare;
            int cornersquare;

            GenCastSquare( amove->new1, &castsquare, &cornersquare );

            value = PiecePosVal( rook, Player, castsquare ) - PiecePosVal( rook, Player, cornersquare );
            value += ( amove->new1 > amove->old ) ? ShortCastleValue : LongCastleValue;

        } else if( amove->movpiece == pawn ) {
            value = PiecePosVal( pawn, Opponent, amove->new1 - ( ( Player == white ) ? 0x10 : -0x10 ) ); /*  E.p. capture  */
        } else
            value = PiecePosVal( amove->movpiece, Player, amove->old ) - PiecePosVal( pawn, Player, amove->old ) + decpawnstrval( Player, amove->old & 7 );		      /*  Pawnpromotion  */
    }

    if( amove->content != no_piece ) {										/*  normal moves  */
        value += PiecePosVal( amove->content, Opponent, amove->new1 );

        /*  Penalty for exchanging pieces when behind in material  */
        if( (abs( MainEvalu ) >= 0x100) && ( amove->content != pawn ) &&  ( ( ProgramColor == Opponent ) == ( MainEvalu >= 0 ) ) )
            value -= EXCHANGEVALUE;
    }

    CalcPawnBit( white );
    CalcPawnBit( black );

    if( ( amove->movpiece == pawn ) && ( ( amove->content != no_piece ) || amove->spe ) )
        value += movepawnstrval( Player, amove->new1 & 7, amove->old & 7 );

    if( ( amove->content == pawn ) || amove->spe && ( amove->movpiece == pawn ) )
        value -= decpawnstrval( Opponent, amove->new1 & 7 );

    /*  Calculate value of move  */
    value += PiecePosVal( amove->movpiece, Player, amove->new1 )- PiecePosVal( amove->movpiece, Player, amove->old );

    return( value );
}

void SetPieceValues( int NewValues[7] )
{
    memcpy( PieceValue, NewValues, 7 * sizeof( int ) );

}

void GetPieceValues( int OutValues[7] )
{
    memcpy( OutValues, PieceValue, 7 * sizeof( int ) );

}
