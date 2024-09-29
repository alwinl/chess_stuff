// ObjectWindows - (C) Copyright 1992 by Borland International

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <stdio.h>
#include <static.h>
#include <filedial.h>
#include <inputdia.h>
#include "wcdefs.h"
#include "wchess.h"
#include "externs.h"



bool       ComputerThinking = false;
bool       GotValidMove = false;
ENUMCOLOR  Player, Opponent;
short      Depth;
MOVESTRUCT MainLine[ 23 ];
int        MainEvalu;
int        MaxDepth;
int        LegalMoves;
PIECETAB   PieceTab[2][16];
WORD       MessageToPost;
bool       NoComputerMove = false;
static MOVESTRUCT movetemp[MAXPLY - BACK + 2];
MOVESTRUCT *MovTab = &movetemp[-BACK];




struct INFTYPE {
    short   principvar; /* Principal variation search */
    int value;      /* Static incremental evaluation */
    int evaluation; /* Evaluation of position */
};

enum ENUMMOVGEN { main_variation, specialcap, kill, norml } ;  /* move type */

struct SEARCHTYPE {
    MOVESTRUCT[MAXPLY]    line;           /* best line at next ply */
    short       capturesearch;  /* indicates capture search */
    int     maxval;         /* maximal evaluation returned in search */
    int         nextply;        /* Depth of search at next ply */
    INFTYPE     next;           /* information at next ply */
    short       zerowindow;     /* Zero-width alpha-beta-window */
    ENUMMOVGEN  movgentype;
};

struct PARAMTYPE {
    int     alpha, beta;
    int         ply;
    INFTYPE     *inf;
    MOVESTRUCT    *bestline;
    SEARCHTYPE  *S;
};




#define TOLERANCE   8

static INFTYPE    startinf;       /* Inf at first ply */
static int        alphawindow;    /* alpha window value */
static int        repeatevalu;    /* MainEvalu at ply one */
static MOVESTRUCT killingmove[MAXPLY + 1][2];       /* second index is a kill number. We have a stack of two moves per depth */
static short      checktable[MAXPLY + 3];
static int        passdpawn[MAXPLY + 4];         /* Square of eventual pawn on 7th rank */
static bool       SkipSearch;
static short      searchstatedepth;



/*
 *  Initialize killingmove, checktab and passedpawn
 */
static void clearkillmove( void )
{
    const int WHITERANK7 = 0x60;
    const int BLACKRANK7 = 0x10;

    for( short dep = 0; dep <= MAXPLY; dep++ ) {
        killingmove[dep][0] = ZeroMove;
        killingmove[dep][1] = ZeroMove;
    }

    checktable[0] = 0;
    passdpawn[0] = -1;  /*  No check at first ply  */
    passdpawn[1] = -1;

    /*  Place eventual pawns on 7th rank in passedpawn  */
    for( int sq = WHITERANK7; sq <= WHITERANK7 + 7; sq++ )
        if( ( Board[sq].piece == pawn ) && ( Board[sq].color == white ) )
            passdpawn[ (white == Player) ? 0 : 1 ] = sq;

    for( int sq = BLACKRANK7; sq <= BLACKRANK7 + 7; sq++ )
        if( ( Board[sq].piece == pawn ) && ( Board[sq].color == black ) )
            passdpawn[ (black == Player) ? 0 : 1 ] = sq;
}

/*
 *  Backup the search and setup talk surroundings
 */
static void getprogramstate( void )
{
    ENUMCOLOR oldplayer;

    searchstatedepth = Depth;
    while( Depth > 0 ) {
        Depth--;
        oldplayer = Opponent;
        Opponent = Player;
        Player = oldplayer;

        ResetMove( &MovTab[ Depth ] );
    }

    Depth--;
    if( Opan )
        TakeBackMove( &MovTab[Depth] );
}

/*
 *  Restore the search surroundings
 */
static void getsearchstate( void )
{
    ENUMCOLOR oldplayer;

    if( Opan )
        MakeMove( &MovTab[Depth+1] );

    Depth++;

    while( Depth < searchstatedepth ) {
        SetMove( &MovTab[Depth] );
        oldplayer = Player;
        Player = Opponent;
        Opponent = oldplayer;
        Depth++;
    }
}

/*
 *  Update killingmove using bestmove
 */
static void updatekill( MOVESTRUCT *bestmove )
{
	/*  Update killingmove unless the move is a capture of last piece moved  */
	if( ( MovTab[Depth - 1].movpiece != no_piece ) && ( bestmove->new1 == MovTab[Depth-1].new1 ) )
		return;

	if( ( killingmove[Depth][0].movpiece == no_piece ) || ( EqMove( bestmove, &killingmove[Depth][1] ) ) ) {
		killingmove[Depth][1] = killingmove[Depth][0];
		killingmove[Depth][0] = *bestmove;
		return;
	}

	if( !EqMove( bestmove, &killingmove[Depth][0] ) )
		killingmove[Depth][1] = *bestmove;

	return;
}

/*
 *  Test if move has been generated before
 */
static bool generatedbefore( PARAMTYPE *P )
{
    if( P->S->movgentype == main_variation )
		return false;

	if( EqMove( &MovTab[Depth], &P->bestline[Depth] ) )
		return true;

	if( (P->S->capturesearch) || (P->S->movgentype == kill) )
		return false;

	if( (EqMove( &MovTab[Depth], &killingmove[Depth][0] )) || (EqMove( &MovTab[Depth], &killingmove[Depth][1] )) )
		return true;

    return false;
}

/*
 *  Perform move, calculate evaluation, test cut-off, etc
 */
static bool update( PARAMTYPE *P )
{
    short selection;

    ++Nodes;
    P->S->nextply = P->ply - 1;      /*  Calculate next ply  */

    if( Level == matesearch ) { /*  Matesearch  */
        SetMove( &MovTab[ Depth ] );  /*  Perform Move on the board  */

        if( Attacks( Opponent, get_king_square(Player) ) ) {		/*  Check if Move is legal  */
            ResetMove( &MovTab[ Depth ] );	// take back the move
			if( Analysis || !Depth )
				TInfo_PrintDepthMessage( MaxDepth, &MovTab[0] );
            return true;
        }

        if( !Depth )
            LegalMoves++;

        checktable[ Depth + 1]     = 0;
        passdpawn[ Depth + 2 ]   = -1;
        P->S->next.value      = P->S->next.evaluation = 0;

        if( P->S->nextply <= 0 ) { /*  Calculate check and perform evt. cut-off  */
            if( !P->S->nextply )
                checktable[ Depth + 1 ] = Attacks( Player, get_king_square(Opponent) );

            if( !checktable[ Depth + 1 ] ) {
				// test cut_off
                if( P->S->next.value <= P->alpha ) ) {

                	if( (P->S->maxval < P->S->next.value) )
						P->S->maxval = P->S->next.value;

                    ResetMove( &MovTab[ Depth ] );	// take back the move
					if( Analysis || !Depth )
						TInfo_PrintDepthMessage( MaxDepth, &MovTab[0] );

                    return true;
                }
            }
        }

		if( Analysis || !Depth )
			TInfo_PrintDepthMessage( MaxDepth, &MovTab[0] );

        return false;					// Accept the move
    }

    /* Make special limited capturesearch at first iteration */
    if( MaxDepth <= 1 ) {
        if( P->S->capturesearch && Depth >= 2 ) {
            if
            (
                !( ( MovTab[Depth].content < MovTab[Depth].movpiece )
                   || ( P->S->movgentype == specialcap )
                   || ( MovTab[Depth].old == MovTab[Depth - 2].new1 )
                 )
            ) {
				if( Analysis || !Depth )
					TInfo_PrintDepthMessage( MaxDepth, &MovTab[0] );

                return true;
            }
        }
    }

    /* Calculate nxt static incremental evaluation */
    P->S->next.value = -P->inf->value + StatEvalu( &MovTab[Depth] );

    /*
     * Calculate checktab (only checks with moved piece are calculated) Giving Check
     * does not count as a ply
     */
    checktable[Depth + 1] = PieceAttacks( MovTab[Depth].movpiece, Player, MovTab[Depth].new1, get_king_square( Opponent ) );

    if( checktable[Depth + 1] )
        P->S->nextply = P->ply;

    /* Calculate passedpawn. Moving a pawn to 7th rank does not count as a ply */
    passdpawn[Depth+2] = passdpawn[Depth];

    if( MovTab[Depth].movpiece == pawn ) {
        if( ( MovTab[Depth].new1 < 0x18 ) || ( MovTab[Depth].new1 >= 0x60 ) ) {
            passdpawn[Depth+2] = MovTab[Depth].new1;
            P->S->nextply = P->ply;
        }
    }

    /* Perform selection at last ply and in capture search */
    selection = ( ( P->S->nextply <= 0 ) && !checktable[Depth + 1] && ( Depth > 0 ) );
    if( selection ) {                     /* check evaluation */
		/* check cut off */
        if( P->S->next.value <= P->alpha ) {

			if( P->S->maxval < P->S->next.value )
				P->S->maxval = cutval;

			if( Analysis || !Depth )
				TInfo_PrintDepthMessage( MaxDepth, &MovTab[0] );

            return true;                // cut the move
        }
    }

    SetMove( &MovTab[Depth] );       /* perform move on the board */

    /* check if move is legal */
    if( Attacks( Opponent, get_king_square(Player) ) ) {
        ResetMove( &MovTab[Depth] );   // take back the move
		if( Analysis || !Depth )
			TInfo_PrintDepthMessage( MaxDepth, &MovTab[0] );
        return true;
    }

    if( passdpawn[Depth+2] >= 0 ) {      /* check passedpawn */
        if( Board[passdpawn[Depth+2]].piece != pawn || Board[passdpawn[Depth+2]].color != Player )
            passdpawn[Depth+2] = -1;
    }

    if( !Depth ) {
        LegalMoves++;
        P->S->next.value += random( 4 );
    }

    P->S->next.evaluation = P->S->next.value;

    if( Analysis || !Depth )
		TInfo_PrintDepthMessage( MaxDepth, &MovTab[0] );

    return false;
}

/**
 * return true if the move is either a pawn move or a capture move
 */
static bool isPawnOrCapture( MOVESTRUCT *amove )
{
	return ( (amove->movpiece == no_piece) || (amove->movpiece == pawn) || (amove->content != no_piece) || (amove->spe) )
}

/**
 *  Count the number of moves since last capture or pawn move
 *  The game is a draw when fiftymovecnt = 100
 */
unsigned int FiftyMoveCnt( void )
{
    unsigned int cnt = 0;

    while( !isPawnOrCapture( &MovTab[Depth - cnt] ) )
        cnt++;

    return cnt;
}


/*
 *  Calculate how many times the position has occurred before
 *  The game is a draw when repetition = 3;
 *  movetab[back..Depth] contains the previous moves
 *  When immediate is set, only immediate repetition is checked
 */
unsigned int Repetition( short immediate )
{
    short lastdep, compdep, tracedep, checkdep, samedepth;
    int tracesq, checksq;
    unsigned int repeatcount;

    repeatcount = 1;
    lastdep = samedepth = Depth + 1;    /*  current position  */
    compdep = samedepth - 4;            /*  First position to compare  */

    /*  MovTab[lastdep..Depth] contains previous relevant moves  */
    while( !isPawnOrCapture( &MovTab[lastdep - 1] ) && ( (compdep < lastdep) || !immediate ) )
        lastdep--;

    if( compdep < lastdep )
        return repeatcount;

    checkdep = samedepth;

    do {
        checkdep--;
        checksq = MovTab[checkdep].new1;

        for( tracedep = checkdep + 2; (tracedep < samedepth) && ( MovTab[tracedep].old != checksq ); tracedep += 2 )
			;

		if( MovTab[tracedep].old != checksq ) {

			/*  Trace the move backward to see if it has been 'undone' earlier  */
			tracedep = checkdep;
			tracesq = MovTab[tracedep].old;
			do {
				if( tracedep-2 < lastdep )
					return repeatcount;

				tracedep -= 2;

				if( tracesq == MovTab[tracedep].new1 )			/*  Check if piece has been moved before  */
					tracesq = MovTab[tracedep].old;

			} while( (tracesq != checksq) || (tracedep > compdep + 1) );

			if( tracedep < compdep ) { /*  Adjust evt. compdep  */
				compdep = tracedep;

				if( ( samedepth - compdep ) % 2 == 1 ) {
					if( compdep == lastdep )
						return repeatcount;

					compdep --;
				}
				checkdep = samedepth;
			}
		}

        /*  All moves between SAMEDEP and compdep have been checked, so a repetition is found  */
        if( checkdep <= compdep ) {

            repeatcount++;
            if( compdep - 2 < lastdep )
                return repeatcount;

            checkdep = samedepth = compdep;
            compdep -= 2;
        }

    } while( 1 );
}

/*
 *  Calculate draw bonus/penalty, and set draw if the game is a draw
 */
static bool drawgame( SEARCHTYPE *S, short depth, int adjustment )
{
	switch( depth ) {
	case 0:
	case 2:
		break;

	case 1:
		{
			unsigned int searchfifty = FiftyMoveCnt();
			unsigned int searchrepeat = Repetition( 0 );

			if( searchrepeat >= 3 ) {
				S->next.evaluation = 0;
				return true;
			}

			if( searchfifty >= 96 ) {  /*  48 moves without pawn moves or captures */
				S->next.value += ( adjustment / 4 ) * 3;
				S->next.evaluation += ( adjustment / 4 ) * 3;
				break;
			}

			if( searchrepeat >= 2 ) {  /*  2nd repetition  */
				S->next.value += ( adjustment / 4 ) * 2;
				S->next.evaluation += ( adjustment / 4 ) * 2;
				break;
			}

			if( searchfifty >= 20 ) {  /*  10 moves without pawn moves or captures */
				S->next.value += ( adjustment / 4 );
				S->next.evaluation += ( adjustment / 4 );
				break;
			}
		}
		break;

	default:
		if( Repetition( 1 ) < 3 )
			break;

		/*  Immediate repetition counts as a draw */
		S->next.evaluation = 0;
		return true;
	}

	return false;
}

/*
 *  Update bestline and MainEvalu using line and maxval
 */
static void updatebestline( PARAMTYPE *P )
{
    memcpy( P->bestline, &P->S->line[ 0 ], sizeof( MOVESTRUCT ) * 23 );

    P->bestline[ Depth ] = MovTab[ Depth ];
    if( Depth )
		return;

	MainEvalu = P->S->maxval;

	if( Level == matesearch )
		P->S->maxval = alphawindow;

	if( Analysis )
		TInfo_PrintBestMove( &MainLine[ 0 ], MainEvalu );
}

/*
 *  The inner loop of the search procedure.  MovTab[Depth] contains the move.
 */
static bool loopbody( PARAMTYPE *P )
{
    ENUMCOLOR oldplayer;
    short lastanalysis;

    if( generatedbefore( P ) )
        return false;

    if( Depth < MAXPLY ) {
        P->S->line[Depth + 1] = ZeroMove;
        if( P->S->movgentype == main_variation )
            memmove( &P->S->line[0], P->bestline, sizeof( MOVESTRUCT ) * MAXPLY );
    }

    /*  Principvar indicates principal variation search  */
    /*  Zerowindow indicates zero - width alpha - beta window  */
    P->S->next.principvar = 0;
    P->S->zerowindow = 0;

    if( P->inf->principvar ) {
        if( P->S->movgentype == main_variation )
            P->S->next.principvar = ( P->bestline[ Depth+1 ].movpiece != no_piece );
        else
            P->S->zerowindow = ( P->S->maxval >= P->alpha );
    }

	while( 1 ) {
		if( update( P ) )
			return false;

		bool skip_next = false;

		if( (Level == matesearch) && (P->S->nextply <= 0) && !checktable[Depth + 1] )  /*  stop evt. search  */
			skip_next = true;

		if( drawgame( P->S, Depth, repeatevalu ) || (Depth >= 23) )
			skip_next = true;

		if( ! skip_next ) {

			/*  Analyse nextply using a recursive call to search  */
			oldplayer = Player;
			Player    = Opponent;
			Opponent  = oldplayer;
			Depth++;

			if( P->S->zerowindow )
				P->S->next.evaluation = -search( -P->alpha-1, -P->alpha, P->S->nextply, &P->S->next, &P->S->line[0] );
			else
				P->S->next.evaluation = -search( -P->beta,    -P->alpha, P->S->nextply, &P->S->next, &P->S->line[0] );

			Depth--;
			oldplayer = Opponent;
			Opponent  = Player;
			Player    = oldplayer;
		}

		ResetMove( &MovTab[ Depth ] );  /*  take back move  */
		if( SkipSearch )
			return true;

		lastanalysis = Analysis;  /*  scan messages and test SkipSearch  */

		MessageScan();

		if( !SkipSearch )
			if( Analysis && !SingleStep && ( ( !Depth ) || !lastanalysis ) ) {
				StopTime( &ChessClock );
				if( MainEvalu > alphawindow )
					SkipSearch = ( getTotalTime( &ChessClock ) >= ( WantedTime * 1.5 ) );
			}

		if( Analysis && ( MaxDepth <= 1 ) )
			SkipSearch = 0;

		P->S->maxval = max( P->S->maxval, P->S->next.evaluation );  /*  Update Maxval  */
		if( EqMove( &P->bestline[ Depth ], &MovTab[ Depth ] ) /*  Update evt. bestline  */
			updatebestline( P );

		if( P->alpha >= P->S->maxval )     /*  update alpha and test cutoff */
			return SkipSearch;

		updatebestline( P );

		if( P->S->maxval >= P->beta )
			return true;

		/*  Adjust maxval (tolerance search)  */
		if( P->ply >= 2  && P->inf->principvar && !P->S->zerowindow )
			P->S->maxval = min( P->S->maxval + TOLERANCE, P->beta - 1 );

		P->alpha = P->S->maxval;

		if( ! (P->S->zerowindow && ! SkipSearch ) ) {
			return SkipSearch;

		P->S->zerowindow = 0;			/*  repeat search with full window  */
	}

    return SkipSearch;
}

/*
 *  generate  pawn promotions
 */
static short pawnpromotiongen( PARAMTYPE *P )
{
    ENUMPIECE promote;

    MovTab[Depth].spe = 1;

    for( promote = queen; promote <= knight; ( ( int ) promote )++ ) {
        MovTab[ Depth ].movpiece = promote;
        if( loopbody( P ) )
            return 1;
    }

    MovTab[ Depth ].spe = 0;
    return 0;
}

/*
 *  Generate captures of the piece on Newsq
 */
static short capmovgen( int newsq, PARAMTYPE *P )
{
    int nxtsq, sq;
    int i;

    MovTab[Depth].content  = Board[ newsq ].piece;
    MovTab[Depth].spe      = 0;
    MovTab[Depth].new1     = newsq;
    MovTab[Depth].movpiece = pawn;  /*  pawn captures  */

    nxtsq = MovTab[ Depth ].new1 - ( ( Player == white ) ? 0x10 : -0x10 );

    for( sq = nxtsq - 1; sq <= nxtsq + 1; sq++ ) {
        if( ( sq != nxtsq ) && ( VALIDSQUARE( sq ) ) && ( Board[ sq ].piece == pawn && Board[ sq ].color == Player ) ) {
            MovTab[ Depth ].old = sq;

            if( MovTab[ Depth ].new1 < 8 || MovTab[ Depth ].new1 >= 0x70 ) {
                if( pawnpromotiongen( P ) )
                    return 1;
            } else if( loopbody( P ) )
                return 1;
        }
    }

    for( i = get_officer_count(Player); i >= 0; i-- ) { /*  other captures  */
        if( PieceTab[ Player ][ i ].ipiece != pawn ) {
            if( PieceAttacks( PieceTab[ Player ][ i ].ipiece, Player, PieceTab[ Player ][ i ].isquare, newsq ) ) {

                MovTab[ Depth ].old      = PieceTab[ Player ][ i ].isquare;
                MovTab[ Depth ].movpiece = PieceTab[ Player ][ i ].ipiece;

                if( loopbody( P ) )
                    return 1;
            }
        }
    }
    return 0;
}

/*
 *  Generates non captures for the piece on oldsq
 */
static short noncapmovgen( int oldsq, PARAMTYPE *P )
{
    unsigned int first, last, dir;
    int direction;
    int newsq;

    MovTab[Depth].spe      = 0;
    MovTab[Depth].old      = oldsq;
    MovTab[Depth].movpiece = Board[ oldsq ].piece;
    MovTab[Depth].content  = no_piece;

    switch( MovTab[ Depth ].movpiece ) {
    case king :
        for( dir = 7; dir >= 0; dir-- ) {
            newsq = MovTab[ Depth ].old + DirTab[ dir ];
            if( VALIDSQUARE( newsq ) && ( Board[ newsq ].piece == no_piece ) ) {
                MovTab[ Depth ].new1 = newsq;
                if( loopbody( P ) )
                    return 1;
            }
        }
        break;

    case knight :
        for( dir = 7; dir >= 0; dir-- ) {
            newsq = MovTab[ Depth ].old + KnightDir[ dir ];
            if( VALIDSQUARE( newsq ) && ( Board[newsq].piece == no_piece ) ) {
                MovTab[ Depth ].new1 = newsq;
                if( loopbody( P ) )
                    return 1;
            }
        }
        break;

    case queen :
    case rook  :
    case bishop :
        first = ( ( MovTab[ Depth ].movpiece == rook ) ? 3 : 7 );
        last  = ( ( MovTab[ Depth ].movpiece == bishop ) ? 4 : 0 );

        for( dir = first; dir >= last; dir-- ) {

            newsq = MovTab[ Depth ].old + DirTab[ dir ];

            while( VALIDSQUARE( newsq ) && ( Board[ newsq ].piece == no_piece ) ) {

                MovTab[ Depth ].new1 = newsq;
                if( loopbody( P ) )
                    return 1;

                newsq = MovTab[ Depth ].new1 + DirTab[ dir ];
            }
        }
        break;

    case pawn :
        /*  One square forward  */
        MovTab[Depth].new1 = MovTab[Depth].old + ( ( Player == white ) ? 0x10 : -0x10 )
                             if( Board[MovTab[Depth].new1].piece == no_piece )
            if( MovTab[Depth].new1 < 8 || MovTab[Depth].new1 >= 0x70 ) {
                if( pawnpromotiongen( P ) )
                    return 1;
            } else {
                if( loopbody( P ) )
                    return 1;
                if( MovTab[Depth].old < 0x18 || MovTab[Depth].old
                        >= 0x60 ) {
                    /*  two squares forward  */
                    MovTab[Depth].new1 += ( MovTab[Depth].new1
                                            - MovTab[Depth].old );
                    if( Board[MovTab[Depth].new1].piece == no_piece )
                        if( loopbody( P ) )
                            return 1;
                }
            }
    } /*  switch  */
    return 0;
}

static bool is_castling_move( MOVESTRUCT *amove )
{
	return amove->spe && ( amove->movpiece == king );
}

static bool is_en_pasant_move( MOVESTRUCT *amove )
{
	return amove->spe && ( amove->movpiece == pawn );
}

/** \brief Test whether a move is possible
 *
 *	Move contains a full description of a move, which
 * 	has been legally generated in a different position.
 *	MovTab[Depth - 1] contains last performed move.
 *
 * \param amove MOVESTRUCT* The move to be examined
 * \return bool True if move is possible
 *
 */
static bool IsValidMove( MOVESTRUCT *amove, MOVESTRUCT *last_move )
{
    if( is_castling_move(amove) ) {

        ENUMCASTDIR castdir = ( amove->new1 > amove->old ) ? shrt : lng;	/* which way are we castling */
		int cast_square = ( Player == black ) ? 0x74 : 0x04;				/* get the king position, e-file */

		if( ! is_piece_at_origin( cast_square, king, Player ) ) /*  check king  */
			return false;		/* king has moved */

		/* now find the rook position, long castle is a-file and short castle is h-file */
		cast_square += ( castdir == lng ) ? -4 : 3;

		if( !is_piece_at_origin( cast_square, rook, Player ) )	 /*  check rook  */
			return false;	/* rook has moved */

		cast_square = ( int )( ( amove->new1 + amove->old ) / 2 );

		/*  make sure there are no pieces on the squares we want to move too  */
		if( Board[amove->new1].piece != no_piece || Board[cast_square].piece != no_piece )
			return false;

		/* On a long castle we have an extra square that needs to be empty */
		if( ( castdir == lng ) && ( Board[amove->new1-1].piece != no_piece ) )
			return false;

		/* No castle allowed if any of the squares are being attacked */
		if( Attacks( Opponent, amove->old ) || Attacks( Opponent, amove->new1 ) || Attacks( Opponent, cast_square ) )
			return false;

		return true;	/* move is allowed */
    }

	if( is_en_pasant_move(amove) ) {

		/* Can't do ep capture if the opponents last move was not a pawn move */
		if( last_move->movpiece != pawn )
			return false;

		/*  Last opponents move has to be a 2 square move  */
		if( abs( last_move->new1 - last_move->old ) < 0x20 )
			return false;

		/* If the square we are attacking from does not even have one of our pawns, this move makes no sense */
		if( ( Board[amove->old].piece != pawn ) || ( Board[amove->old].color != Player ) )
			return false;

		/* The ep square is in the middle of the opponents last moves pawn path */
		if( amove->new1 != ( ( last_move->new1 + last_move->old ) / 2 ) )
			return false;

		return true;	/* move is allowed */
	}

	/* On pawn promotion we change the moving piece. So to test for attacks we need signal that it was a pawn*/
	ENUMPIECE test_piece = ( amove->spe ) ? pawn : amove->movpiece;

	/*  Test if the moves starting position actually has the piece on the square  */
	if( (Board[amove->old].piece != test_piece) || ( Board[amove->old].color != Player ) )
		return false;

	if( Board[amove->new1].piece != amove->content )
		return false;

	/* if the square we are moving to is occupied by one of our pieces, we cannot move there */
	if( amove->content != no_piece && Board[amove->new1].color == Player )
		return false;

	if( amove->movpiece != pawn ) { /*  Is the move possible?  */
		return PieceAttacks( test_piece, Player, amove->old, amove->new1 );

	/* if the pawn wants to move 2 squares, the middle square better be empty */
	if( ( abs( amove->new1 - amove->old ) == 0x20 ) && Board[( amove->new1 + amove->old ) / 2].piece != no_piece )
		return false;

    return true;	/* move is allowed */
}

/*
 *  castling moves
 */
struct CASTMOVETYPE {
    int castnew;
    int castold;
};

static CASTMOVETYPE  CastMove[2][2] = { {{0x02, 0x04}, {0x06, 0x04}}, {{0x72, 0x74}, {0x76, 0x74}} };

static short castlingmovgen( PARAMTYPE *P )
{
    ENUMCASTDIR castdir;

    MovTab[Depth].spe      = 1;
    MovTab[Depth].movpiece = king;
    MovTab[Depth].content  = no_piece;

    for( castdir = ( ENUMCASTDIR )( lng-1 ); castdir <= shrt-1; ( ( int )castdir )++ ) {

        MovTab[ Depth ].new1 = CastMove[ Player ][ castdir ].castnew;
        MovTab[ Depth ].old  = CastMove[ Player ][ castdir ].castold;

        if( IsValidMove( &MovTab[ Depth ], &MoveTab[ Depth - 1 ] ) )
            if( loopbody( P ) )
                return 1;
    }
    return 0;
}

/*
 *  e.p. captures
 */
static short epcapmovgen( PARAMTYPE *P )
{
    int sq;

    // if the last move of the opponent was not a pawn move or the pawn did not move two squares
    if( ( MovTab[ Depth - 1 ].movpiece != pawn ) || ( abs( MovTab[ Depth - 1 ].new1 - MovTab[ Depth - 1 ].old ) < 0x20 ) )
        return 0;												// Don't even try

    MovTab[ Depth ].spe      = 1;
    MovTab[ Depth ].movpiece = pawn;
    MovTab[ Depth ].content  = no_piece;
    MovTab[ Depth ].new1     = ( MovTab[ Depth - 1 ].new1 + MovTab[ Depth - 1 ].old ) / 2;	// Square that the pawn hopped over

    // Do we have a pawn that can kill the pawn from the left
    if( VALIDSQUARE( MovTab[ Depth - 1 ].new1 - 1 ) ) {

        MovTab[ Depth ].old = MovTab[ Depth - 1 ].new1 - 1;
        if( IsValidMove( &MovTab[ Depth ], &MoveTab[ Depth - 1 ] ) )
            if( loopbody( P ) )
                return 1;										// Yes, kill it
    }

    // Do we have a pawn that can kill the pawn from the right
    if( VALIDSQUARE( MovTab[ Depth - 1 ].new1 + 1 ) ) {

        MovTab[ Depth ].old = MovTab[ Depth - 1 ].new1 + 1;
        if( IsValidMove( &MovTab[ Depth ], &MoveTab[ Depth - 1 ] ) )
            if( loopbody( P ) )
                return 1;										// Yes, kill it
    }

    return 0;													// Cannot do it
}

/*
 *  Generate the next move to be analysed.
 *   Controls the order of the movegeneration.
 *      The moves are generated in the order:
 *      Main variation
 *      Captures of last moved piece
 *      Killing moves
 *      Other captures
 *      Pawnpromotions
 *      Castling
 *      Normal moves
 *      E.p. captures
 */
static void searchmovgen( PARAMTYPE *P )
{
    int index;
    char killno;

    /*  generate move from main variation  */
    if( P->bestline[ Depth ].movpiece != no_piece ) {
        MovTab[ Depth ] = P->bestline[ Depth ];
        P->S->movgentype = main_variation;
        if( loopbody( P ) )
            return;
    }

    if( ( MovTab[ Depth - 1 ].movpiece != no_piece ) && ( MovTab[ Depth - 1 ].movpiece != king ) ) {
        P->S->movgentype = specialcap;
        if( capmovgen( MovTab[ Depth - 1 ].new1, P ) )
            return;
    }

    P->S->movgentype = kill;
    if( !P->S->capturesearch )
        for( killno = 0; killno <= 1; killno++ ) {
            MovTab[ Depth ] = killingmove[ Depth ][ killno ];
            if( MovTab[ Depth - 1 ].movpiece != no_piece )
                if( IsValidMove( &MovTab[ Depth ], &MoveTab[ Depth - 1 ] ) )
                    if( loopbody( P ) )
                        return;
        }


    P->S->movgentype = norml;
    for( index = 1; index <= get_total_piece_count(Opponent); index++ )
        if( PieceTab[ Opponent ][ index ].ipiece != no_piece )
            if( MovTab[ Depth-1 ].movpiece == no_piece || PieceTab[ Opponent ][ index ].isquare != MovTab[ Depth-1 ].new1 )
                if( capmovgen( PieceTab[ Opponent ][ index ].isquare, P ) )
                    return;

    if( P->S->capturesearch && ( passdpawn[ Depth ] >= 0 ) && ( Board[ passdpawn[ Depth ] ].piece == pawn && Board[ passdpawn[ Depth ] ].color == Player ) ) {
        if( noncapmovgen( (passdpawn[ Depth ], P ) )
            return;
    }

    if( !P->S->capturesearch ) {               /*  non-captures  */
        if( castlingmovgen( P ) )
            return;      /*  castling  */

        for( index = get_total_piece_count(Player); index >= 0; index-- )
            if( PieceTab[ Player ][ index ].ipiece != no_piece )
                if( noncapmovgen( PieceTab[ Player ][ index ].isquare, P ) )
                    return;
    }
    if( epcapmovgen( P ) )
        return;  /*  e.p. captures  */
}

/*
 *  Perform the search
 *  On entry :
 *    Player is next to move
 *    MovTab[Depth-1] contains last move
 *    alpha, beta contains the alpha - beta window
 *    ply contains the Depth of the search
 *    inf contains various information
 *
 *  On exit :
 *    Bestline contains the principal variation
 *    search contains the evaluation for Player
 */
static int search( int alpha, int beta, int ply, INFTYPE *inf, MOVESTRUCT *bestline )
{
    SEARCHTYPE S;
    PARAMTYPE  P;

    S.capturesearch = ( ( ply <= 0 ) && !checktable[ Depth ] );		/*  Perform capturesearch if ply <= 0 and !check  */

	S.maxval = S.capturesearch ? -inf->evaluation : -( LOSEVALUE - Depth * DEPTHFACTOR );

    if( S.capturesearch && (alpha < S.maxval) ) {											/*  initialize maxval  */

		alpha = S.maxval;

		if( S.maxval >= beta )
			return S.maxval;
    }

    P.alpha = alpha;
    P.beta = beta;
    P.ply = ply;
    P.inf = inf;
    P.bestline = bestline;
    P.S = &S;

    searchmovgen( &P );   /*  The search loop  */

    if( SkipSearch )
		return S.maxval;

	/*  Test stalemate  */
	if( (S.maxval == -( LOSEVALUE - Depth * DEPTHFACTOR ) ) && ( !Attacks( Opponent, get_king_square(Player) ) ) )
		return 0;

	if( bestline[ Depth ].movpiece != no_piece )
		updatekill( &bestline[ Depth ] );

    return S.maxval;
}

/*
 *  Begin the search
 */
static int callsearch( int alpha, int beta )
{
    int maxval;

    startinf.principvar = ( MainLine[0].movpiece != no_piece );
    LegalMoves = 0;

    maxval = search( alpha, beta, MaxDepth, &startinf, &MainLine[ 0 ] );
    if( !LegalMoves )
        MainEvalu = maxval;

    return maxval;
}

/*
 *  Checks whether the search time is used
 */
static bool timeused( void )
{
    if( !Analysis || SingleStep )
        return false;       /* We have all the time in the world */

    StopTime( &ChessClock );
    return ( getTotalTime( &ChessClock ) >= WantedTime );
}

/*
 *  setup search
 */
void FindMove( int maxlevel )
{
    int maxval;
    double calcpvtime;

    InitTime( &ChessClock );
    StartTime( &ChessClock );

    Nodes = 0;
    SkipSearch = false;
    clearkillmove();
    CalcPVTable();
    int RootValue = CalcRootvalue();

    StopTime( &ChessClock );

    calcpvtime          = getTotalTime( &ChessClock );
    startinf.value      = -RootValue;
    startinf.evaluation = -RootValue;
    MaxDepth            = 0;
    MainLine[0]         = ZeroMove;
    MainEvalu           = RootValue;
    alphawindow         = MAXINT;
    ComputerThinking    = true;

    do {
        /*  update various variables  */
        if( MaxDepth <= 1 )
            repeatevalu = MainEvalu;

        alphawindow = min( alphawindow, MainEvalu - 0x80 );

        if( Level == matesearch ) {
            alphawindow = 0x6000;
            if( MaxDepth > 0 )
                MaxDepth++;
        }

        MaxDepth++;
        maxval = callsearch( alphawindow, 0x7f00 );  /*  perform the search  */

        /*  Repeat the search if the value falls below the alpha-window  */
        if( maxval <= alphawindow && !SkipSearch && Level != matesearch && LegalMoves > 0 ) {
            MainEvalu  = alphawindow;
            maxval     = callsearch( -0x7F00, alphawindow - TOLERANCE * 2 );
            LegalMoves = 2;
        }

    } while( !SkipSearch && !timeused() && ( MaxDepth < maxlevel ) && ( LegalMoves > 1 ) && ( abs( MainEvalu ) < MATEVALUE - 24 * DEPTHFACTOR ) );

    ComputerThinking = false;
    StopTime( &ChessClock );

    if( Analysis )
        TInfo_PrintNodes( Nodes, ( getTotalTime( &ChessClock ) - calcpvtime ) );
}




void clear_piecetab( ENUMCOLOR color )
{
    for( int index = 0; index < 16; index++ ) {
		PieceTab[color][index].ipiece = no_piece;
		PieceTab[color][index].isquare = 0;
    }
}

void set_piecetab_piece( ENUMCOLOR color, short index, ENUMPIECE piece )
{
    PieceTab[color][index].ipiece = piece;
}

void set_piecetab_square( ENUMCOLOR color, short index, int square )
{
    PieceTab[color][index].isquare = square;
}

int get_king_square( ENUMCOLOR color )
{
	return PieceTab[color][king].isquare;
}



