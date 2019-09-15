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
static int          PieceValue[7]          = { 0, (16 << 4), (9 << 4), (5 << 4), (3 << 4), (3 << 4), (1 << 4) };
const char          pawnrank[8]            = { 0, 0, 0, 2, 4, 8, 30, 0 };
const char          passpawnrank[8]        = { 0, 0, 10, 20, 40, 60, 70, 0 };
const char          pawnfilefactor[8]      = { 0, 0, 2, 5, 6, 2, 0, 0 };

const unsigned char filebittab[8]          = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };

PAWNBITTYPE         pawnbitt[2][MAXPLY + 2];






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
 *
 * pawnbitt[color][0].one contains a byte where each bit represent the presence of a pawn somewhere on that file
 * thus if pawnbitt[color][0].one returns as 0x0100110 we know that we have pawn(s) on the b, c, and g files
 * (remember PawnTable is reversed). There could be more than one pawn on the file.
 * The corresponding bit in dob will tell us if there is more than one pawn on the file.
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

static void CalcPawnBit( ENUMCOLOR color, short depth )
{
    pawnbitt[color][ depth ] = pawnbitt[color][ depth - 1 ];  /*  calculate pawnbitt  */
}

/**
 * \brief Calculate value of the pawn structure in pawnbitt[depth][color]
 *
 * The maximum value is 0. In this case there are no isolated pawns
 * and no double pawns.
 * We penalise based on the amount of double pawns, we penalise based
 * on the amount of isolated pawns we have and give it a double whammy
 * for pawns that are isolated and double.
 */
static int pawnstrval( ENUMCOLOR color, short depth )
{
	PAWNBITTYPE * entry = &pawnbitt[color][depth];
    unsigned char iso;  /*  contains FILEs with isolated pawns  */

    iso = entry->one & ~( ( entry->one << 1 ) | ( entry->one >> 1 ) );

    return ( -( count( entry->dob ) * DOUBLEPAWN
               + count( iso ) * ISOLATEDPAWN
               + count( iso & entry->dob ) * ISOLATEDPAWN * 2 ) );
}

/*
 *  Update pawnbitt and calculates value difference with current position when a pawn is removed from file
 */
static int decpawnstrval( ENUMCOLOR color, unsigned char file, short depth )
{
	PAWNBITTYPE * entry = &pawnbitt[color][depth];

    entry->one &= ~( 1 << file );
    entry->one |= entry->dob;

    entry->dob &= ~( 1 << file );

    return( pawnstrval( color, depth ) - pawnstrval( color, depth - 1 ) );
}

/*
 *  Update pawnbitt and calculates value when a pawn moves from old to new1 file
 */
static int movepawnstrval( ENUMCOLOR color, unsigned char new_file, unsigned char old_file, short depth )
{
	PAWNBITTYPE * entry = &pawnbitt[color][depth];

    entry->dob  |= ( entry->one &  (1 << new_file) );

    entry->one   = ( entry->one & ~(1 << old_file) );
    entry->one  |= ( entry->dob |  (1 << new_file) );

    entry->dob  &= ~(1 << old_file);

    return( pawnstrval( color, depth ) - pawnstrval( color, depth - 1 ) );
}



/*
 *  calculate the value of the piece on the square
 */
static int PiecePosVal( ENUMPIECE piece, ENUMCOLOR color, int square )
{
    return ( (PieceValue[piece] << 4) + PositionValueTable[color][piece][square] );
}

static bool squares_are_neighbours( int square1, int square2 )
{
	int rank1 = square1 >> 4;
	int file1 = square1 & 0x0F;
	int rank2 = square2 >> 4;
	int file2 = square2 & 0x0F;

	return ( abs(rank1 - rank2) == 1 ) && ( abs( file1 - file2 ) == 1 );
}

static int square_distance( int square1, int square2 )
{
	int rank1 = square1 >> 4;
	int file1 = square1 & 0x0F;
	int rank2 = square2 >> 4;
	int file2 = square2 & 0x0F;

	return ( abs(rank1-rank2) + abs(file1-file2) );
}
/**
 *
 *  Calculate attack values of a square (importance of a square)
 * mat_level range 0..78
 */
static char calc_attack_value( ENUMCOLOR color, int square, int mat_level )
{
	const char squarerankvalue[8] = { 0, 0, 0, 0, 3, 6, 12, 12 };
    const char distan[8] = { 3, 2, 1, 0, 0, 1, 2, 3 };

	int rank = (color == white) ? (square >> 4) : 7 - (square >> 4);	/* calculate the rank from the colours perspective */
	int file = square & 0x0F;

	/* mat_level ranges from 0 to 78
	 * squarerankvalue[rank] * ( mat_level + 8 ) ranges 0..1032
	 * squarerankvalue[rank] * ( mat_level + 8 ) >> 5 ranges 0..32
	 * first four ranks are all zero, fifth rank ranges 0..8, sixth rank ranges 1..16 and rank seven and eight ranges 3..32
	 */
	char rank_importance = ( ( squarerankvalue[rank] * ( mat_level + 8 ) ) >> 5 ); /* range 0..32 */

	char centre_importance = max( 0, 8 - 3 * ( distan[rank] + distan[file] ) );
	char king_importance = ( (mat_level > 0) && squares_are_neighbours( square, get_king_square( 1 - color ) ) ) ? ( ( 12 * ( mat_level + 8 ) ) >> 5 ) : 0;

	return rank_importance + centre_importance + king_importance;
}



/**
 * \brief Clears the values of all piece on a all squares
 *
 * This function is called once on the start of a new game
 */
void ClearPVTable( void )
{
    for( ENUMPIECE piece = king; piece <= pawn; ( ( int )piece )++ )
        for( int square = 0; square <= 0x77; square++ ) {
            PositionValueTable[ white ][ piece ][ square ] = 0;
            PositionValueTable[ black ][ piece ][ square ] = 0;
        }
}

/**
 * \brief Calculate the value of a piece on a certain square
 *
 * This function gets called just before we start evaluating moves
 */
void CalcPVTable( void )
{
    unsigned char PawnTable[2][8];
    unsigned char pawnfiletab, bit, oppasstab, behindoppass,
             leftsidetab, rightsidetab, sidetab, leftchaintab,
             rightchaintab, chaintab, leftcovertab, rightcovertab;		/*  Bit tables for static pawn structure evaluation  */
    int           pvcontrol[2][5][0x78];										/*  Value of squares controlled from the square  */
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

	int           materiallevel;	/*  Material level of the game (early middle game = 43 - 32, endgame = 0)  */
    ENUMCOLOR     losingcolor;		/*  the colour which is being mated  */
    bool          mating;           /*  mating evaluation function is used  */
    {
		int material[2] = { 0, 0 };

		/* Calculate material values. Material value at start of game is 39 (<< 4) for each colour ( at default values) */
		for( int square = 0x00; square < 0x78; ++square )
			if( VALIDSQUARE( square ) && Board[square].piece != no_piece && Board[square].piece != king )
				material[ Board[square].color ] += PieceValue[ Board[square].piece ];

		/*  Material level of the game (start game = 78 (<<4), early middle game = 43 - 32 (<<4), endgame = 0) */
		materiallevel = material[white] >> 4 + material[black] >> 4;
		losingcolor = (material[white] < material[black]) ? white : black;

		/*  Set mating if weakest player has less than the equivalence of two bishops and the advantage is at least a rook for a bishop */
		mating = ( material[losingcolor] < 2 * PieceValue[ bishop ] ) &&
				( material[losingcolor] + PieceValue[ rook ] < material[(1 - losingcolor] + PieceValue[ bishop ] );
    }


    char attackvalue[2][0x78];											/*  Importance of an attack of the square  */
	{
		/*  Calculate attack values of each square (importance of each square)  */
		for( int square = 0x00; square < 0x78; ++square ) {
			if( VALIDSQUARE( square ) ) {
				attackvalue[white][square] = calc_attack_value( white, square, materiallevel );
				attackvalue[black][square] = calc_attack_value( black, square, materiallevel );
			}
		}

	}





    /*  Initialise PVControl  */
    for( int square = 0x00; square < 0x78; ++square ) {
        if( VALIDSQUARE( square ) ) {
			pvcontrol[ white ][ rook ][ square ] = 0;
			pvcontrol[ black ][ rook ][ square ] = 0;
			pvcontrol[ white ][ bishop ][ square ] = 0;
			pvcontrol[ black ][ bishop ][ square ] = 0;
			pvcontrol[ white ][ knight ][ square ] = 0;
			pvcontrol[ black ][ knight ][ square ] = 0;
			pvcontrol[ white ][ queen ][ square ] = 0;
			pvcontrol[ black ][ queen ][ square ] = 0;
        }
    }

    /*  Calculate PVControl  */
	/*  Count value of all attacks from the square in the direction.
		The value of attacking a square is found in attackvalue.
		Indirect attacks (e.g. a Rook attacking through another Rook) counts for a Normal attack.
		Attacks through another officer (excluding the queen) or king counts half.
		Attacks through a pawn don't count.

		Basically PVControl tells us how attractive it is to move a certain piece (of a certain colour) to a certain square
		A higher value means it's more attractive.
	*/

	const int dir_table[8]  = { 0x01, -0x01, 0x10, -0x10, 0x11, -0x11, 0x0f, -0x0f };
	const int knight_dir[8] = { 0x0E, -0x0E, 0x12, -0x12, 0x1f, -0x1f, 0x21, -0x21 };

    for( int square = 0x00; square < 0x78; ++square ) {

        if( ! VALIDSQUARE( square ) )
			continue;

		for( color = white; color <= black; ( ( int )color )++ ) {

			/* Calculate bishop and rook values */
			for( dir = 0; dir < 8; ++dir ) {

				cnt = 0;
				direct = 1;

				/* Run through the line. If we run off the board or stomp on a pawn we stop */
				for( int sq = square; VALIDSQUARE( sq ) && ( Board[ sq ].piece != pawn ); sq += dir_table[ dir ] ) {

					// add the attack value of this square to the running total
					cnt += ( direct ? attackvalue[ color ][ sq ] : ( attackvalue[ color ][ sq ] >> 1 ) );

					/* check if there is a (different) piece blocking the way */
					if( ( Board[ sq ].piece != no_piece ) && ( Board[ sq ].piece != queen ) && Board[ sq ].piece != ((dir < 4) ? rook : bishop) )
						direct = 0;
				}

				pvcontrol[ color ][ ((dir < 4) ? rook : bishop) ][ square ] += ( cnt >> 2 );
			}

			/* Calculate the knight value */
			cnt = 0;
			for( dir = 0; dir < 8; dir++ ) {
				sq = square + KnightDir[dir];
				if( VALIDSQUARE( sq ) )
					cnt += attackvalue[color][sq];
			}

			pvcontrol[ color ][ knight ][ square ] = ( cnt >> 1 );

			/* Penalise knight squares that are further from the centre */
			pvcontrol[ color ][ knight ][ square ] -= 3 * ( distan[ square >> 4] + distan[square & 7] );

			/* Calculate queen value from bishop and rook */
			pvcontrol[ color ][ queen ][ square ] = ( pvcontrol[ color ][ rook ][ square ] + pvcontrol[ color ][ bishop ][ square ] ) >> 2;

			/* calculate the king value */
			if( mating ) {
				/* by default we want to move the king. Hence we set it to a high-ish default value */
				pvcontrol[ color ][ king ][ square ] = 128;

					if( color == losingcolor ) {
						/* If we are losing we want to stay away from the edges. Penalise squares that are further away from the centre */
						pvcontrol[ color ][ king ][ square ] -= 16 * distan[ square >> 4  ];
						pvcontrol[ color ][ king ][ square ] -= 12 * distan[ square & 0x0F ];

						/* Extra penalisation for the outer edges of the board */
						if( distan[ square >> 4  ] == 3 )
							pvcontrol[ color ][ king ][ square ] -= 16;

					} else {
						/* Penalise squares that are further away from opponents king */
						pvcontrol[ color ][ king ][ square ] -= 4 * square_distance( square, get_king_square( 1 - color ) );

						/* Penalise squares closer to the edge */
						if( ( distan[ square >> 4 ] >= 2 ) || ( distan[ square & 0x0F ] == 3 ) )
							pvcontrol[ color ][ king ][ square ] -= 16;
					}

			} else {
				if( ! materiallevel )		// materiallevel == 0 means endgame
					/* Penalise squares that are closer to the edge */
					pvcontrol[ color ][ king ][ square ] = -2 * (distan[ square >> 4 ] + distan[ square & 0x0F ]);
				else
					/* Opening and middle game, moving the king has no value */
					pvcontrol[ color ][ king ][ square ] = 0;
			}

			/* calculate the pawn value */
			const char pawnrankfactor[8] = { 0, 0, 0, 2, 4, 8, 30, 0 };
			const char pawnfilefactor[8] = { 0, 0, 2, 5, 6, 2, 0, 0 };

			rank = ( ( color == black ) ? ( 7 - ( square >> 4 ) ) : ( square >> 4 ) );

			if( ( rank == 0 ) || ( rank == 7 ) )		// pawns can never be on rank 0 and get promoted on rank 7
				pvcontrol[ color ][ pawn ][ square ] = 0;
			else {
				pvcontrol[ color ][ pawn ][ square ] = -12;
				pvcontrol[ color ][ pawn ][ square ] += pawnrankfactor[ rank ]; 							/* as pawns progress they become more valuable */
				pvcontrol[ color ][ pawn ][ square ] += pawnfilefactor[ square & 0x07 ] * ( rank + 2 );		/* e and d pawns are much more valuable */
			}
		}
    }

	/*  Calculate the position value table, value by value  */
	for( square = 0x00; square < 0x78; ++square ) {

		if( ! VALIDSQUARE( square ) )
			continue;

		for( color = white; color <= black; ( ( int )color )++ ) {

			for( piece = king; piece <= pawn; ( ( int )piece )++ ) {
				PositionValueTable[ color ][ piece ][ square ] =
							( !mating || ( piece == pawn ) || (piece == king)  ) ? pvcontrol[ color ][ piece ][ square ] : 0;
			}
		}
	}

    /*  Calculate PawnTable (indicates which squares contain pawns )
		for all intents and purposes the pawntable is a 2 times 8x8 matrix of bits. Each bit represents the presence of a pawn
		Each value in the table represents one rank. Within a rank each bit represents a file.
		However for some reason the files are reversed.
		So for instance 0x10011000 does not mean pawns on the a, d and e files but pawns on the d, e and h files.
    */
    for( rank = 0; rank < 8; rank++ ) {
        PawnTable[ white ][ rank ] = 0;
        PawnTable[ black ][ rank ] = 0;
    }


    for( square = 0x00; square < 0x78; ++square ) {
        if( VALIDSQUARE( square ) && ( Board[ square ].piece == pawn ) ) {

			color = Board[ square ].color;
            rank = ( color == black ) ? (square ^ 0x70) >> 4 : (square >> 4);
			file = square & 0x07;

            PawnTable[ color ][rank] |= (1 << file);
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

                if( ( materiallevel == 0 ) || ( oppcolor != ProgramColor ) ) {

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
            value = PiecePosVal( amove->movpiece, Player, amove->old ) - PiecePosVal( pawn, Player, amove->old ) + decpawnstrval( Player, amove->old & 7, Depth + 1 );		      /*  Pawnpromotion  */
    }

    if( amove->content != no_piece ) {										/*  normal moves  */
        value += PiecePosVal( amove->content, Opponent, amove->new1 );

        /*  Penalty for exchanging pieces when behind in material  */
        if( (abs( MainEvalu ) >= 0x100) && ( amove->content != pawn ) &&  ( ( ProgramColor == Opponent ) == ( MainEvalu >= 0 ) ) )
            value -= EXCHANGEVALUE;
    }

    CalcPawnBit( white, Depth + 1 );
    CalcPawnBit( black, Depth + 1 );

    if( ( amove->movpiece == pawn ) && ( ( amove->content != no_piece ) || amove->spe ) )
        value += movepawnstrval( Player, amove->new1 & 7, amove->old & 7, Depth + 1 );

    if( ( amove->content == pawn ) || amove->spe && ( amove->movpiece == pawn ) )
        value -= decpawnstrval( Opponent, amove->new1 & 7, Depth + 1 );

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
