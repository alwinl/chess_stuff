// ObjectWindows - (C) Copyright 1992 by Borland International

#include <math.h>
#include "wcdefs.h"
#include "externs.h"

/*
 *  Globals
 */


int DirTab[8] = { 0x01, -0x01, 0x10, -0x10, 0x11, -0x11, 0x0f, -0x0f };
int KnightDir[8] = {0x0E, -0x0E, 0x12, -0x12, 0x1f, -0x1f, 0x21, -0x21};
int PawnDir[2] = {0x10, -0x10};



int GeneratedMoveCount, BufPnt;
static MOVESTRUCT GeneratedMoves[81];

class GeneratedMoves
{
public:
	GeneratedMoves() : generated(0), retrieved(0) { };

	void reset() {
		generated = 0;
		retrieved = 0;
	};

	void add_move( MOVESTRUCT new_move )
	{
		if( generated < MAXMOVES ) {
			GeneratedMoves[generated] = new_move;
			++generated;
		}
	};

	MOVESTRUCT get_move()
	{
		if( retrieved > generated )
			return ZeroMove;

		return GeneratedMoves[retrieved++];
	};

private:
	const unsigned int MAXMOVES = 81;

	unsigned int generated;
	unsigned int retrieved;

	MOVESTRUCT GeneratedMoves[MAXMOVES];
};

GeneratedMoves gen_move;

static void clear_generated_moves()
{
	gen_move.reset();
}

static void add_generated_move( MOVESTRUCT new_move )
{
	gen_move.add_move( new_move );
}

static MOVESTRUCT get_generated_move( )
{
	return gen_move.get_move();
}


/****************************************************************************/

/** \brief The attack table is a difference table.
 *
 * Put the centre of the table on the starting square and the table will tell you
 * which squares a piece on the starting square can move to.
 */
static int * AttackTable( void )
{
	static int attack2[0xF0];
	static bool AttackTab_calculated = false;

	if( AttackTab_calculated )
		return &attack2[0x78];

	memset( attack2, 0, sizeof(int) * 0xF0 );

    for( int dir=0; dir<8; ++dir ) {

        for( int i = 1; i < 8; i++ ) {

			int index = DirTab[dir] * i + 0x78;

			attack2[index] |= (1 << queen);								/* queen moves both diagonally and straight */
            attack2[index] |= ( dir < 4 ) ? (1 << rook) : (1 << bishop);	/* 0..3 are straight directions, 4..7 are diagonals */

            attack2[index] |= (DirTab[dir] << 8);
        }

        attack2[DirTab[dir]+0x78]    |= (DirTab[dir] << 8) | (1 << king);
        attack2[KnightDir[dir]+0x78] |= (KnightDir[dir] << 8) | (1 << knight);
    }

    AttackTab_calculated = true;

	return &attack2[0x78];
}

/* For pawns we need to look a rank ahead or back for various functions.
 * Going up a rank depends on the colourCASTMOVETYPE
 */
static int pawn_rank_up( ENUMCOLOR acolor, int asquare )
{
	return ( acolor == white ) ? asquare + 0x10 : asquare - 0x10;
}

static int pawn_rank_down( ENUMCOLOR acolour, int asquare )
{
	return ( acolour == white ) ? asquare - 0x10 : asquare + 0x10;
}

/* Is there a pawn of this colour on this square? */
static bool pawn_on_square( ENUMCOLOR acolour, int asquare )
{
	return ( VALIDSQUARE( asquare) && Board[asquare].piece == pawn && Board[asquare].color == acolour );
}

/** \brief Calculate whether apiece placed on asquare attacks the square
 *
 */
bool PieceAttacks( ENUMPIECE apiece, ENUMCOLOR acolour, int asquare, int target_square )
{
    if( apiece == pawn )  /*  pawn attacks  */
		/* go up a rank (from the pawns perspective) and look on either side */
        return ( (pawn_rank_up( acolour, asquare ) - 1 == target_square) || (pawn_rank_up(acolour, asquare) + 1 == target_square) );

	int * AttackTab = AttackTable();

	int index  = target_square - asquare;

    /*  other attacks: can the piece move to the square?  */
    if( ! (AttackTab[index] & (1 << apiece)) )
		return false;	/* nope, it cannot */

	if( apiece == king || apiece == knight )
		return true;

	/*  are there any blocking pieces in between?  */
	for( int sq = asquare; VALIDSQUARE(sq) && (sq != target_square); sq += (AttackTab[index] >> 8) )
		if( Board[sq].piece != no_piece )
			return false;	/* we have a blocking piece */

	return true;
}

/** \brief Calculates whether acolour attacks the square
 *
 */
bool Attacks( ENUMCOLOR acolour, int target_square )
{
    int i;

	/* Check if there is a pawn in a position to attack
	 * We need to look one rank down (from the pawns perspective) and either side of the file.
	 */
    if( ( pawn_on_square( acolour, pawn_rank_down( acolour, target_square ) - 0x01 )
		|| pawn_on_square( acolour, pawn_rank_down( acolour, target_square ) + 0x01 ) ) )
        return true;

    /*  Other attacks:  try all pieces, starting with the smallest  */
    for( int i = OfficerNo[acolour]; i >= 0; i-- ) {
		PIECETAB piecetab_entry = PieceTab[acolour][i];

        if( piecetab_entry.ipiece == no_piece )
			continue;

		if( PieceAttacks( piecetab_entry.ipiece, acolour, piecetab_entry.isquare, target_square ) )
			return true;
    }

    return false;
}

/****************************************************************************/

/** \brief Check whether apiece is placed on square and has never moved
 *
 * If it has, castling is not allowed
 */
static bool is_piece_at_origin( int square, ENUMPIECE apiece, ENUMCOLOR acolour )
{
    if( Board[square].piece != apiece || Board[square].color != acolour )
        return false;       /* The piece is not even at its origin location */

    short dep = Depth - 1;

    while( MovTab[dep].movpiece != no_piece ) {    /* check all recorded moves */
        if( MovTab[dep].new1 == square )
            return false;                       /* found a move where the piece moved to this square */
        dep--;
    }

    return true;                /* piece has never moved */
}

/** \brief Generates captures of the piece on new1 using the piece table
 *
 * \param acolour ENUMCOLOR
 * \return void
 *
 */
static void CapMovGen( ENUMCOLOR acolour )
{
    int oldsq, sq;
    int i;
    MOVESTRUCT cap_move;
    ENUMCOLOR opponent_colour = (acolour == white) ? black : white;

    /* generate all captures starting with captures of largest pieces */
    for( int index = 1; index <= PawnNo[opponent_colour]; index++ ) {

		PIECETAB opponent_piece = PieceTab[opponent_colour][index];

        if( opponent_piece.ipiece == no_piece )
			continue;

		/* start building the move */
		cap_move.new1 = opponent_piece.isquare;
		//cap_move.old
		cap_move.spe = 0;
		cap_move.movpiece = pawn;
		cap_move.content = Board[opponent_piece.isquare].piece;

		oldsq = cap_move.new1 - ( ( acolour == white ) ? 0x10 : -0x10 );	/* calculate rank where the pawn is coming from */

		for( cap_move.old = oldsq-1; cap_move.old <= oldsq+1; cap_move.old+=2 ) {	/* pawns attack sideways */

			/* if there is one of our pawns on the square that can capture the piece on this square */
			if( VALIDSQUARE( cap_move.old ) && Board[cap_move.old].piece == pawn && Board[cap_move.old].color == acolour ) {
				/* pawn not only captures the piece but promotes at the same time */
				if( cap_move.new1 < 8 || cap_move.new1 >= 0x70 ) {
					cap_move.spe = 1;
					for( cap_move.movpiece = queen; cap_move.movpiece <= knight; ( ( int )cap_move.movpiece )++ )
						add_generated_move( cap_move );                     /* Store the move in buffer */

				} else {
					cap_move.spe = 0;
					cap_move.movpiece = pawn;
					add_generated_move( cap_move );        /* Store the move in buffer */
				}
			}
		}

		/*  Other captures, starting with the smallest pieces  */
		for( int i = OfficerNo[acolour]; i >= 0; i-- ) {

			PIECETAB piecetab_entry = PieceTab[acolour][i];

			if( (piecetab_entry.ipiece == no_piece) || ( piecetab_entry.ipiece == pawn ) )
				continue

			if( ! PieceAttacks( piecetab_entry.ipiece, acolour, piecetab_entry.isquare, cap_move.new1 ) )
				continue;

			cap_move.old = piecetab_entry.isquare;
			cap_move.movpiece = piecetab_entry.ipiece;

			add_generated_move( cap_move );    /* Store the move in buffer */
		}
    }
}

/** \brief Generate castling moves
 *
 * There are two possible castling moves, a short and a long castle.
 * build each move and test whether the move can be executed at this point in the game.
 * if it is a valid possible move, add it to the list of generated moves
 *
 * \param acolour ENUMCOLOR The colour to generate the castling moves for.
 */
static void CastMovGen( ENUMCOLOR acolour )
{
	MOVESTRUCT cast_move;
	ENUMCOLOR opponent_colour = (acolour == white) ? black : white;

    // cast_move.new1;  filled in below
    cast_move.old = (acolour == white) ? 0x04 : 0x74; /* the king square */

    /* if the king has moved, no castling is allowed */
	if( ! is_piece_at_origin( cast_move.old, king, acolour ) )
		return;

    cast_move.spe = 1;
    cast_move.movpiece = king;
    cast_move.content = no_piece;

	/* Build a short castle move */
	cast_move.new1 = cast_move.old + 0x02;

	if( is_piece_at_origin( cast_move.old + 0x03, rook, acolour ) && 	/* check that rook has not moved */
		(Board[cast_move.old + 0x01].piece == no_piece) && 				/* check all affected squares are empty */
		(Board[cast_move.old + 0x02].piece == no_piece) &&
		! Attacks( opponent_colour, cast_move.old ) &&							/* check none of the affected (king) squares are under attack */
		! Attacks( opponent_colour, cast_move.old + 0x01 ) &&
		! Attacks( opponent_colour, cast_move.old + 0x02 )
	)
		add_generated_move( cast_move );								/* all checks pass, add the move */

	/* Build a long castle move */
	cast_move.new1 = cast_move.old - 0x02;

	if( is_piece_at_origin( cast_move.old - 0x04, rook, acolour ) && 	/* check that rook has not moved */
		(Board[cast_move.old - 0x01].piece == no_piece) && 				/* check all affected squares are empty */
		(Board[cast_move.old - 0x02].piece == no_piece) &&
		(Board[cast_move.old - 0x03].piece == no_piece) &&
		! Attacks( opponent_colour, cast_move.old ) &&							/* check none of the affected (king) squares are under attack */
		! Attacks( opponent_colour, cast_move.old - 0x01 ) &&
		! Attacks( opponent_colour, cast_move.old - 0x02 )
	)
		add_generated_move( cast_move );								/* all checks pass, add the move */
}

/** \brief generates non captures for acolours' army
 *
 * \param acolour ENUMCOLOR
 * \return void
 *
 */
static void NonCapMovGen( ENUMCOLOR acolour )
{
    MOVESTRUCT new_move;

    for( index = PawnNo[acolour]; index >= 0; index-- ) {

		PIECETAB player_piece = PieceTab[acolour][index];

        if( player_piece.ipiece == no_piece )
			continue;

		// new1;	/* no default, is set below */
		new_move.old = player_piece.isquare;
		new_move.spe = 0;
		new_move.movpiece = Board[player_piece.isquare].piece;
		new_move.content = no_piece;

		switch( Board[player_piece.isquare].piece ) {
		case king :
			for( int dir = 7; dir >= 0; dir-- ) {
				new_move.new1 = new_move.old + DirTab[dir];
				if( VALIDSQUARE( new_move.new1 ) && ( Board[new_move.new1].piece == no_piece ) )
					add_generated_move( new_move );
			}
			break;

		case knight :
			for( int dir = 7; dir >= 0; dir-- ) {
				new_move.new1 = new_move.old + KnightDir[dir];
				if( VALIDSQUARE( new_move.new1 ) && ( Board[new_move.new1].piece == no_piece ) )
					add_generated_move( new_move );
			}
			break;

		case queen :
			for( int dir = 7; dir >= 0; dir-- ) {
				/* Generate all non captures in the direction as long as we do not run of the board or bump into another piece */
				for( new_move.new1 = new_move.old + DirTab[dir]; VALIDSQUARE( new_move.new1 ) && (Board[new_move.new1].piece == no_piece); new_move.new1 += DirTab[dir] )
					add_generated_move( new_move );
			}
			break;

		case rook  :
			for( int dir = 3; dir >= 0; dir-- ) {
				/* Generate all non captures in the direction as long as we do not run of the board or bump into another piece */
				for( new_move.new1 = new_move.old + DirTab[dir]; VALIDSQUARE( new_move.new1 ) && (Board[new_move.new1].piece == no_piece); new_move.new1 += DirTab[dir] )
					add_generated_move( new_move );
			}
			break;

		case bishop:
			for( int dir = 7; dir >= 4; dir-- ) {
				/* Generate all non captures in the direction as long as we do not run of the board or bump into another piece */
				for( new_move.new1 = new_move.old + DirTab[dir]; VALIDSQUARE( new_move.new1 ) && (Board[new_move.new1].piece == no_piece); new_move.new1 += DirTab[dir] )
					add_generated_move( new_move );
			}
			break;

		case pawn :
			new_move.new1 = pawn_rank_up( acolour, new_move.old );  /*  one square forward  */

			if( Board[new_move.new1].piece != no_piece )	/* pawn is blocked */
				break;

			if( new_move.new1 < 8 || new_move.new1 >= 0x70 ) {	/* if we end up on the last rank, generate pawn promotion moves */
				new_move.spe = 1;
				for( new_move.movpiece = queen; new_move.movpiece <= knight; ( ( int )new_move.movpiece )++ )
					add_generated_move( new_move );                     /* Store the move in buffer */
				break;
			}

			add_generated_move( new_move );			/* normal pawn move */

			if( (new_move.old >= 0x18 && new_move.old < 0x60 ))	/* pawn is not on a starting rank */
				break;

			new_move.new1 = pawn_rank_up( acolour, new_move.new1 );	/* Pawn on starting rank can move two steps forward */

			if( Board[new_move.new1].piece != no_piece )	/* pawn is blocked */
				break;

			add_generated_move( new_move );
			break;
		}  /* switch */
    }
}

/** \brief Generate en passant moves
 *
 * \param acolour ENUMCOLOR
 * \param last_move MOVESTRUCT
 * \return void
 *
 */
static void EPMovGen( ENUMCOLOR acolour, MOVESTRUCT last_move )
{
	/* if the opponents last move was not a pawn move or the opponents last (pawn) move was not a 2 square move */
    if( (last_move.movpiece != pawn) || (abs( last_move.new1 - last_move.old ) != 0x20) )
		return;									/* no en passant move possible */

	MOVESTRUCT ep_move;

	ep_move.new1 = ( last_move.new1 + last_move.old ) / 2;	/* the ep square is right in the middle of the 2 square move */
	// ep_move.old;	/* set below */
	ep_move.spe = 1;
	ep_move.movpiece = pawn;
	ep_move.content = no_piece;

	/* now look on either side of the file */
	ep_move.old = last_move.new1 - 0x01;		/* file on the left */

	/* The square we are attacking from has to be valid and contain one of our pawns */
	if( VALIDSQUARE( ep_move.old ) && ( Board[ep_move.old].piece == pawn ) && ( Board[ep_move.old].color == acolour ) )
		add_generated_move( ep_move );

	ep_move.old = last_move.new1 + 0x01;		/* file on the right */

	if( VALIDSQUARE( ep_move.old ) && ( Board[ep_move.old].piece == pawn ) && ( Board[ep_move.old].color == acolour ) )
		add_generated_move( ep_move );
}

/** \brief The move generator.
 *
 *  InitMovGen generates all possible moves and places them in a buffer.
 *  Movgen will the generate the moves one by one and place them in next.
 *
 *  On entry:
 *    Player contains the colour to move.
 *    MovTab[Depth-1] the last performed move.
 *
 *  On exit:
 *    GeneratedMoves contains the generated moves.
 *
 *    The moves are generated in the order :
 *      Captures
 *      Castlings
 *      Non captures
 *      E.p. captures
 */
void InitMovGen( void )
{
    clear_generated_moves();

	CapMovGen( Player );			/* generate all possible capture moves */
	CastMovGen( Player );	/* generate castling moves */
	NonCapMovGen( Player );	/* generate non capture moves */
	EPMovGen( Player, MovTab[Depth - 1] );		/* generate en passant moves */
}

/** \brief Return the next move
 *
 *  place next move from the buffer in next.  Generate zeromove when there
 *  are no more moves.
 */
MOVESTRUCT MovGen( void )
{
	return get_generated_move();
}
