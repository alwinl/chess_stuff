// OWLCVT 08/22/97 02:11:38
// ObjectWindows - (C) Copyright 1992 by Borland International

#include "evalu.h"
#include <string.h>
#include <cmath>
#include "search.h"
#include "board.h"
// #include "small.h"

#undef max
#undef min
#define max(a, b)  (((a) > (b)) ? (a) : (b))
#define min(a, b)  (((a) < (b)) ? (a) : (b))

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
 *  Global variables for this module
 */
const char          pawnrank[8]            = { 0, 0, 0, 2, 4, 8, 30, 0 };
const char          pawnfilefactor[8]      = { 0, 0, 2, 5, 6, 2, 0, 0 };
const unsigned char filebittab[8]          = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };

static int          PieceValue[7]          = { 0, 0x1000, 0x900, 0x4c0, 0x300, 0x300, 0x100 };
//static int          PieceValue[7]          = { 0, (16 << 4), (9 << 4), (5 << 4), (3 << 4), (3 << 4), (1 << 4) };

static int PositionValueTable[2][7][0x78];
static PAWNBITTYPE pawnbitt[2][MAXPLY + 2];




void SetPieceValues( int NewValues[7] )
{
    memcpy( PieceValue, NewValues, 7 * sizeof( int ) );
}

void GetPieceValues( int OutValues[7] )
{
    memcpy( OutValues, PieceValue, 7 * sizeof( int ) );
}


/*
 *  count the number of set bits in b
 */
static int count_bits( unsigned char b )
{
    char cnt = 0;

    while( b ) {
        if( b % 2 == 1 )
            cnt++;
        b >>= 1;
    }
    return cnt;
}


/*  Calculate PawnTable (indicates which squares contain pawns )
	for all intents and purposes the pawntable is a 2 times 8x8 matrix of bits. Each bit represents the presence of a pawn
	Each value in the table represents one rank. Within a rank each bit represents a file.
	However for some reason the files are reversed.
	So for instance 0x10011000 does not mean pawns on the a, d and e files but pawns on the d, e and h files.
*/
void BuildPawnTable( unsigned char table[], ENUMCOLOR color, BOARDTYPE board[] )
{
	memset( table, 0, 8 );

    for( unsigned int square = 0x00; square < 0x78; ++square ) {
        if( VALIDSQUARE( square ) && ( board[ square ].piece == pawn ) && (Board[ square ].color == color) ) {

            unsigned char rank = (square >> 4);
			unsigned char file = (square & 0x07);

            if( color == black )
                rank ^= 0x07;

            table[rank] |= (1 << file);
        }
    }
}

/*
 * Initialize pawnbitt (at depth 0)
 *
 * pawnbitt.one contains a byte where each bit represent the presence of a pawn somewhere on that file
 * thus if pawnbitt.one returns as 0x0100110 we know that we have pawn(s) on the b, c, and g files
 * (remember PawnTable is reversed). There could be more than one pawn on the file.
 * The corresponding bit in dob will tell us if there is more than one pawn on the file.
 */
static PAWNBITTYPE InitPawnbitt( unsigned char table[] )
{
    PAWNBITTYPE pawn_bitt;

    pawn_bitt.one = 0;
    pawn_bitt.dob = 0;

    for( int rank = 1; rank < 7; rank++ ) {
        pawn_bitt.dob |= ( pawn_bitt.one & table[rank] );
        pawn_bitt.one |= table[rank];
    }

    return pawn_bitt;
}

static void remove_pawn( PAWNBITTYPE * entry, unsigned char file )
{
    entry->one &= ~( 1 << file );	// remove from file
    entry->one |= entry->dob;		// if there is a double pawn, this will turn the bit back on

    entry->dob &= ~( 1 << file );	// remove (potential) double pawn flag
}

static void add_pawn( PAWNBITTYPE * entry, unsigned char file )
{
    entry->dob  |= ( entry->one &  (1 << file) ); // if there already is a pawn on the file, this will now signal a double pawn
    entry->one  |= (1 << file);
}

/**
 * \brief Calculate value of the pawn structure
 *
 * The maximum value is 0. In this case there are no isolated pawns
 * and no double pawns.
 * We penalise based on the amount of double pawns, we penalise based
 * on the amount of isolated pawns we have and give it a double whammy
 * for pawns that are isolated and double.
 */
static int pawnstrval( PAWNBITTYPE * entry )
{
    unsigned char isolated_pawns = entry->one & ~( (entry->one << 1) | (entry->one >> 1) );

    return (
        - count_bits( entry->dob ) * DOUBLEPAWN
        - count_bits( isolated_pawns ) * ISOLATEDPAWN
        - count_bits( isolated_pawns & entry->dob ) * ISOLATEDPAWN * 2
    );
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
    const char distance_from_center[8] = { 3, 2, 1, 0, 0, 1, 2, 3 };

	int rank = square >> 4;
	int file = square & 0x0F;
    ENUMCOLOR opponent = (color == white) ? black : white;

    if( color == black )
        rank ^= 0x07;


	/* mat_level ranges from 0 to 78
	 * squarerankvalue[rank] * ( mat_level + 8 ) ranges 0..1032
	 * squarerankvalue[rank] * ( mat_level + 8 ) >> 5 ranges 0..32
	 * first four ranks are all zero, fifth rank ranges 0..8, sixth rank ranges 1..16 and rank seven and eight ranges 3..32
	 */
	char rank_importance = ( ( squarerankvalue[rank] * ( mat_level + 8 ) ) >> 5 ); /* range 0..32 */

	char centre_importance = max( 0, 8 - 3 * ( distance_from_center[rank] + distance_from_center[file] ) );
	char king_importance = ( (mat_level > 0) && squares_are_neighbours( square, get_king_square( opponent ) ) ) ? ( ( 12 * ( mat_level + 8 ) ) >> 5 ) : 0;

	return rank_importance + centre_importance + king_importance;
}



/**
 * \brief Clears the values of all piece on a all squares
 *
 * This function is called once on the start of a new game
 */
void ClearPVTable( void )
{
    memset( PositionValueTable, 0, sizeof(int) * 2 * 7 * 0x78 );
}

/**
 * \brief Calculate the value of a piece on a certain square
 *
 * This function gets called just before we start evaluating moves.
 * It builds the positional value tables (piece square tables) and the current boards pawn structure evaluation
 */
void CalcPVTable( void )
{
    unsigned char PawnTable[2][8];
    unsigned char pawnfiletab, bit, oppasstab, behindoppass,
             leftsidetab, rightsidetab, sidetab, leftchaintab,
             rightchaintab, chaintab, leftcovertab, rightcovertab;		/*  Bit tables for static pawn structure evaluation  */
    //int           posval;													/*  The positional value of piece  */
    //int           attval;													/*  The attack value of the square  */
    //unsigned char file;														/*  The file of the piece  */
    //unsigned char rank;														/*  The rank of the piece  */
    //char          dist;											/*  Distance to center, to opponents king */
    //char          kingdist;											/*  Distance to center, to opponents king */
    //ENUMCASTDIR   cast;														/*  Possible castlings  */
    short         direct;													/*  Indicates direct attack  */
    int           cnt;														/*  Counter for attack values  */
    int           strval;													/*  Pawnstructure value  */
    ENUMCOLOR     color, oppcolor;											/*  Color and opponents color  */
    ENUMPIECE     piece;												/*  Piece counter  */
    int           square;													/*  Square counter  */
    //char          dir;														/*  Direction counter  */
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
		materiallevel = (material[white] >> 4) + (material[black] >> 4);
		losingcolor = (material[white] < material[black]) ? white : black;

		/*  Set mating if weakest player has less than the equivalence of two bishops and the advantage is at least a rook for a bishop */
		mating = ( material[losingcolor] < 2 * PieceValue[ bishop ] ) &&
				( material[losingcolor] + PieceValue[ rook ] < material[(losingcolor == white) ? black : white] + PieceValue[ bishop ] );
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
    int pvcontrol[2][7][0x78];										/*  Value of squares controlled from the square  */

    memset( pvcontrol, 0, sizeof(int) * 2 * 7 * 0x78 );

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

		for( color = white; color <= black; color = ENUMCOLOR(color + 1) ) {

            if( ! mating ) {                // if we are mating we don't need to calculate these values, they will be zero

                /* Calculate bishop and rook values */
                for( int dir = 0; dir < 8; ++dir ) {

                    cnt = 0;
                    direct = 1;
                    ENUMPIECE piece_type = ((dir < 4) ? rook : bishop);

                    /* Run through the line. If we run off the board or stomp on a pawn we stop */
                    for( int sq = square; VALIDSQUARE( sq ) && ( Board[ sq ].piece != pawn ); sq += dir_table[ dir ] ) {

                        // add the attack value of this square to the running total
                        cnt += ( direct ? attackvalue[ color ][ sq ] : ( attackvalue[ color ][ sq ] >> 1 ) );

                        /* check if there is a (different) piece blocking the way */
                        if( ( Board[ sq ].piece != no_piece ) && ( Board[ sq ].piece != queen ) && Board[ sq ].piece != piece_type )
                            direct = 0;
                    }

                    PositionValueTable[ color ][piece_type][ square ] += ( cnt >> 2 );
                }

                /* Calculate the knight value. If we are mating the value is 0 */
                cnt = 0;
                for( int dir = 0; dir < 8; dir++ ) {
                    sq = square + knight_dir[dir];
                    if( VALIDSQUARE( sq ) )
                        cnt += attackvalue[color][sq];
                }

                PositionValueTable[ color ][ knight ][ square ] = ( cnt >> 1 );
                PositionValueTable[ color ][ knight ][ square ] -= 3 * ( distan[ square >> 4] + distan[square & 7] );    /* Penalise knight squares that are further from the centre */

                /* Calculate queen value from bishop and rook */
                PositionValueTable[ color ][ queen ][ square ] = ( PositionValueTable[ color ][ rook ][ square ] + PositionValueTable[ color ][ bishop ][ square ] ) >> 2;

			    /* Calculate the king value */
				if( ! materiallevel )		// materiallevel == 0 => endgame, !=0 => opening or midgame, moving the king has no value
					PositionValueTable[ color ][ king ][ square ] = -2 * (distan[ square >> 4 ] + distan[ square & 0x0F ]);  /* Penalise squares that are closer to the edge */

            } else {            // mating

                /* Bishop, rook, knight and queen values are all zero */

                /* Calculate the king value */

				/* by default we want to move the king. Hence we set it to a high-ish default value */
				PositionValueTable[ color ][ king ][ square ] = 128;

                if( color == losingcolor ) {
                    /* If we are losing we want to stay away from the edges. Penalise squares that are further away from the centre */
                    PositionValueTable[ color ][ king ][ square ] -= 16 * distan[ square >> 4  ];        // rank
                    PositionValueTable[ color ][ king ][ square ] -= 12 * distan[ square & 0x0F ];       // file

                    /* Extra penalisation for the outer edges of the board */
                    if( distan[ square >> 4  ] == 3 )
                        PositionValueTable[ color ][ king ][ square ] -= 16;

                } else {
                    /* Penalise squares that are further away from opponents king */
                    PositionValueTable[ color ][ king ][ square ] -= 4 * square_distance( square, get_king_square( (color == white) ? black : white ) );

                    /* Penalise squares closer to the edge */
                    if( ( distan[ square >> 4 ] >= 2 ) || ( distan[ square & 0x0F ] == 3 ) )
                        PositionValueTable[ color ][ king ][ square ] -= 16;
                }
			}

			/* Calculate the pawn value, gets calculated irrespectively of mating condition */
			const char pawnrankfactor[8] = { 0, 0, 0, 2, 4, 8, 30, 0 };
			const char pawnfilefactor[8] = { 0, 0, 2, 5, 6, 2, 0, 0 };

            unsigned char rank = (square >> 4);
            if( color == black )
                rank ^= 0x07;

			if( ( rank != 0 ) && ( rank != 7 ) ) {		// pawns can never be on rank 0 and get promoted on rank 7
				PositionValueTable[ color ][ pawn ][ square ] = -12;
				PositionValueTable[ color ][ pawn ][ square ] += pawnrankfactor[ rank ]; 							/* as pawns progress they become more valuable */
				PositionValueTable[ color ][ pawn ][ square ] += pawnfilefactor[ square & 0x07 ] * ( rank + 2 );		/* e and d pawns are much more valuable */
			}
		}
    }

    /*  Calculate penalty for blocking centre pawns with a bishop  */
    for( square = 0x13; square < 0x15; ++square ) {
        if( ( Board[square].piece == pawn ) && ( Board[square].color == white ) ) // if the pawn is still at the starting position
            PositionValueTable[ white ][ bishop ][ square + 0x10 ] -= BISHOPBLOCKVALUE;             // we don't want the bishop blocking its progress
    }

    for( square = 0x63; square < 0x65; ++square ) {
        if( ( Board[square].piece == pawn ) && ( Board[square].color == black ) )
            PositionValueTable[ black ][ bishop ][ square - 0x10 ] -= BISHOPBLOCKVALUE;
    }

    /*  Calculate pawnstructurevalue  */

    static const char passpawnrank[8] = { 0, 0, 10, 20, 40, 60, 70, 0 };

	BuildPawnTable( PawnTable[white], white, Board );
	BuildPawnTable( PawnTable[black], black, Board );

    pawnbitt[white][0] = InitPawnbitt( PawnTable[white] );
    pawnbitt[black][0] = InitPawnbitt( PawnTable[black] );

    /*  Calculate static value for pawn structure  */
    for( ENUMCOLOR color = white; color <= black; color = ENUMCOLOR(color+1) ) {

        oppcolor     = (color == white) ? black : white;
        pawnfiletab  = 0;
        leftsidetab  = 0;
        rightsidetab = 0;
        behindoppass = 0;
        oppasstab    = 0xff;

        for( unsigned char rank = 1; rank < 7; rank++ ) {

            oppasstab    &= ( ~( pawnfiletab | leftsidetab | rightsidetab ) );	/*  Squares where opponents pawns are passed pawns  */
            behindoppass |= ( oppasstab & PawnTable[ oppcolor ][ 7 - rank ] );	/*  Squares behind the opponents passed pawns  */

            leftchaintab  = leftsidetab;										/*  squares which are covered by a pawn  */
            rightchaintab = rightsidetab;

            pawnfiletab   = ( PawnTable[ color ][ rank ] );							/*  squares w/ pawns  */

            leftsidetab   = ( PawnTable[ color ][ rank ] << 1 ) & 0xff;							/*  squares w/ a pawn beside them  */
            rightsidetab  = ( PawnTable[ color ][ rank ] >> 1 ) & 0xff;

            leftcovertab  = ( PawnTable[ color ][ rank + 1 ] << 1 ) & 0xff;
            rightcovertab = ( PawnTable[ color ][ rank + 1 ] >> 1 ) & 0xff;

            sq            = ( ( color == black ) ? ( rank << 4 ) ^ 0x70 : ( rank << 4 ) );      // first file

            for( bit = 0x01; bit != 0x00; bit = (bit << 1) & 0xFF, ++sq ) {     // increases both the bit mask and the file

                strval = 0;

                if( bit & (leftsidetab | rightsidetab) )
                    strval = SIDEPAWN;
                else
                    if( bit & (leftchaintab | rightchaintab) )
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

            }
        }
    }

}

/*
 * @brief Returns the current boards positional value (root value)
 */
int CalcRootvalue()
{
    int RootValue = 0;

    /*  Calculate RootValue  */
    for( unsigned int square = 0x77; square >= 0; square-- ) {
        if( VALIDSQUARE( square ) && ( Board[ square ].piece != no_piece ) ) {
            if( Board[ square ].color == Player )
                RootValue += PiecePosVal( Board[ square ].piece, Player, square );
            else
                RootValue -= PiecePosVal( Board[ square ].piece, Opponent, square );
        }
    }

    RootValue += pawnstrval( &pawnbitt[Player][0] ) - pawnstrval( &pawnbitt[Opponent][0] );

    return RootValue;
}

/*
 *  Calculate STATIC evaluation of the move
 */
int StatEvalu( MOVESTRUCT *amove )
{
    const int ShortCastleValue = 32;
    const int LongCastleValue = 4;

    bool is_ep_capture = amove->spe && (amove->movpiece == pawn);
    bool is_castling = amove->spe && (amove->movpiece == king);
    bool is_promotion = amove->spe && !is_ep_capture && !is_castling;
    bool is_capture = (amove->content != no_piece);

    /* base value of move */
    int value = PiecePosVal( amove->movpiece, Player, amove->new1 ) - PiecePosVal( amove->movpiece, Player, amove->old );

	if( is_castling ) {

		int castsquare;
		int cornersquare;

		GenCastSquare( amove->new1, &castsquare, &cornersquare );

		value += PiecePosVal( rook, Player, castsquare ) - PiecePosVal( rook, Player, cornersquare );
		value += ( amove->new1 > amove->old ) ? ShortCastleValue : LongCastleValue;
	}

	if( is_ep_capture )
		value += PiecePosVal( pawn, Opponent, amove->new1 - ( ( Player == white ) ? 0x10 : -0x10 ) );

	if( is_promotion )
		value += PiecePosVal( amove->movpiece, Player, amove->old ) - PiecePosVal( pawn, Player, amove->old );

    if( is_capture ) {
        value += PiecePosVal( amove->content, Opponent, amove->new1 );

        /*  Penalty for exchanging pieces when behind in material  */
        if( (abs( MainEvalu ) >= 0x100) && ( amove->content != pawn ) &&  ( ( ProgramColor == Opponent ) == ( MainEvalu >= 0 ) ) )
            value -= EXCHANGEVALUE;
    }

    // Pawn things

    /*  Initialise pawnbitt using the board without this move */
    pawnbitt[white][ Depth + 1 ] = pawnbitt[white][ Depth ];
    pawnbitt[black][ Depth + 1 ] = pawnbitt[black][ Depth ];

    if( ( is_capture && (amove->movpiece == pawn) ) || is_ep_capture ) { // if we move our pawn to another file (ie we use a pawn to capture)

        remove_pawn( &pawnbitt[Player][Depth + 1], amove->old & 7 );
        add_pawn( &pawnbitt[Player][Depth + 1], amove->new1 & 7 );

        value += ( pawnstrval( &pawnbitt[Player][Depth + 1] ) - pawnstrval( &pawnbitt[Player][Depth] ) );
    }

    if( ( amove->content == pawn ) || is_ep_capture ) {     // if an opponents pawn is captured
        remove_pawn( &pawnbitt[Opponent][Depth + 1], amove->new1 & 7 );

        value -=( pawnstrval( &pawnbitt[Opponent][Depth + 1] ) - pawnstrval( &pawnbitt[Opponent][Depth] ) );
    }

    if( is_promotion ) {                                    // Our pawn is now gone
        remove_pawn( &pawnbitt[Player][Depth + 1], amove->old & 7 );

        value +=( pawnstrval( &pawnbitt[Player][Depth + 1] ) - pawnstrval( &pawnbitt[Player][Depth] ) );
    }

    return( value );
}
