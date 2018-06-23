// OWLCVT 08/22/97 02:12:18
// ObjectWindows - (C) Copyright 1992 by Borland International

#include <owl\owlpch.h>
#pragma hdrstop

#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <stdio.h>
#include "wcdefs.h"
#include "externs.h"

#undef max
#undef min
#define max(a, b)  (((a) > (b)) ? (a) : (b))
#define min(a, b)  (((a) < (b)) ? (a) : (b))

#define VALIDSQUARE( square )  ( !( (square) & 0x88 ) )

extern double WantedTime;

BOOL       ComputerThinking = FALSE;
BOOL       GotValidMove = FALSE;
ENUMCOLOR  Player, Opponent;
short      Depth;
MOVESTRUCT MainLine[ 23 ];
int        MainEvalu;
int        MaxDepth;
int        LegalMoves;
PIECETAB   PieceTab[2][16];
WORD       MessageToPost;
BOOL       NoComputerMove = FALSE;

static MOVESTRUCT movetemp[ 23 - BACK + 2 ];

MOVESTRUCT *MovTab = &movetemp[ -BACK ];

#define TOLERANCE       8
#define IF_EQMOVE(a, b) if( ( a.movpiece == b.movpiece ) && ( a.new1 == b.new1 ) && ( a.old == b.old ) && ( a.content == b.content ) && ( a.spe == b.spe ) )

typedef struct {
    short principvar; /*  Principal variation search  */
    int   value;      /*  Static incremental evaluation  */
    int   evaluation; /*  Evaluation of position */
} INFTYPE;

typedef enum { mane, specialcap, kill, norml } MOVGENTYPE;  /*  move type  */

typedef struct {
    MOVESTRUCT line[ 23 ];        /*  best line at next ply  */
    short capturesearch;  /*  indicates capture search  */
    int maxval;       /*  maximal evaluation returned in search */
    int nextply;          /*  Depth of search at next ply  */
    INFTYPE next;         /* information at Next ply  */
    short zerowindow;     /*  Zero-width alpha-beta-window  */
    MOVGENTYPE movgentype;
} SEARCHTYPE;

typedef struct {
    int alpha, beta;
    int ply;
    INFTYPE *inf;
    MOVESTRUCT *bestline;
    SEARCHTYPE *S;
} PARAMTYPE;

static INFTYPE startinf;     /*  Inf at first ply  */
static int alphawindow;  /*  alpha window value  */
static int repeatevalu;  /*  MainEvalu at ply one  */

static int search(int alpha, int beta, int ply, INFTYPE *inf, MOVESTRUCT *bestline);

/*
 *  Global Variables for this module
 */

MOVESTRUCT killingmove[ 24 ][2];
static short      chcktb[ 26 ];
static short      *checktab = &chcktb[1];
/*  Square of eventual pawn on 7th rank  */
int        passdpawn[ 27 ];
int        *passedpawn = &passdpawn[2];
BOOL       SkipSearch;
static short searchstatedepth;



inline void DisplayMove()
{
	if( !Depth ) {
		sprintf( buf, "%-7d%7s", MaxDepth, MoveStr( &MovTab[ 0 ] ) );
		TInfo->SetDepthText( buf );
	}
}

/*
 *  Initialize killingmove, checktab and passedpawn
 */

static void clearkillmove( void )
{
    const int     rank7[2] = {0x60, 0x10};
    short         dep;
    ENUMCOLOR     col;
    int           sq;
    unsigned char i;

    for( dep = 0; dep <= 23; dep++ )
        for( i = 0; i < 2; i++ )
            killingmove[ dep ][ i ] = ZeroMove;

    checktab[-1] = 0;
    passedpawn[-2] = -1;  /*  No check at first ply  */
    passedpawn[-1] = -1;

    /*  Place eventual pawns on 7th rank in passedpawn  */
    for( col = white; col <= black; ((int)col)++ )
        for( sq = rank7[ col ]; sq <= rank7[ col ] + 7; sq++ )
            if( ( Board[ sq ].piece == pawn ) && ( Board[ sq ].color == col ) )
				passedpawn[ ( col == Player ) ? -2 : -1 ] = sq;
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

        Perform( &MovTab[ Depth ], 1 );
    }

    Depth--;
    if( Opan ) 
		TakeBackMove( &MovTab[ Depth ] );
}


/*
 *  Restore the search surroundings
 */

static void getsearchstate(void)
{
    ENUMCOLOR oldplayer;

    if( Opan) 
		MakeMove( &MovTab[ Depth+1 ] );

    Depth++;
    while( Depth < searchstatedepth ) {
        Perform(&MovTab[Depth], 0);
        oldplayer = Player;
        Player = Opponent;
        Opponent = oldplayer;
        Depth++;
    }
}

inline BOOL UsableMessage(MSG msg)
{
   return( ( msg.hwnd != hWndMain || msg.message != WM_COMMAND ) ? FALSE : TRUE );
}

static void MessageScan()
{
	MSG msg;
	extern HCURSOR hWaitCursor;
	extern HMENU ThinkMenu;
	extern HANDLE hAccel;

	if( !PeekMessage( &msg, hWndMain, 0, 0, PM_REMOVE ) )
		return;

	if( TranslateAccelerator( hWndMain, HACCEL( hAccel ), &msg ) ) {
		::PostMessage( hWndMain, WM_COMMAND, MessageToPost, 0L );
		MessageToPost = 0;
		SkipSearch = FALSE;
		return;
	}

	if( Analysis ) {
		switch( msg.message ) {
		case WM_SETCURSOR :
			DispatchMessage( &msg );
			break;

		case WM_COMMAND :
			switch( msg.wParam ) {
			case CM_STOP :
				SkipSearch = TRUE;
				AutoPlay = FALSE;
				break;
			}
			break;

		default:
			TranslateMessage( &msg );
			DispatchMessage( &msg );
			break;
		}
	} else {
		switch( msg.message ) {
		case WM_LBUTTONDOWN :
			getprogramstate();
			NoComputerMove = TRUE;
			GotValidMove = FALSE;
			DispatchMessage( &msg );
			NoComputerMove = FALSE;
			if( Opan && !MultiMove && GotValidMove ) {
				IF_EQMOVE( KeyMove, MovTab[ Depth + 1 ] ) {
					SkipSearch = FALSE;
					GotValidMove = FALSE;
					EnterKeyMove();
					StartAnalysis();
					PrintBestMove( &MainLine[ 0 ], MainEvalu );
					::SetMenu( hWndMain, ThinkMenu );
					::SetClassWord( hWndMain, GCW_HCURSOR, WORD( hWaitCursor ) );
					::SetCursor( hWaitCursor );
				} else
					SkipSearch = TRUE;
			}
			getsearchstate();
			break;

		default:
			if( UsableMessage( msg ) ) {
				SkipSearch = TRUE;
				if( msg.message != WM_PAINT )
					::PostMessage( msg.hwnd, msg.message, msg.wParam, msg.lParam );
			} else {
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
		break;
		}
	}
}


/*
 *  Update killingmove using bestmove
 */

inline void updatekill(MOVESTRUCT *bestmove)
{
    if( bestmove->movpiece != empty ) {
    /*  Update killingmove unless the move is a capture of last piece moved  */
        if( ( MovTab[Depth - 1].movpiece == empty ) || ( bestmove->new1 != MovTab[ Depth-1 ].new1 ) )
            if( ( killingmove[ Depth ][ 0 ].movpiece == empty ) || ( EqMove( bestmove, &killingmove[ Depth ][ 1 ] ) ) ) {
                killingmove[ Depth ][ 1 ] = killingmove[ Depth ][ 0 ];
                killingmove[ Depth ][ 0 ] = *bestmove;
            } else if( !EqMove(bestmove, &killingmove[ Depth ][ 0 ] ) )
                killingmove[ Depth ][ 1 ] = *bestmove;
    }
}  /*  Updatekill  */



/*
 *  Test if move has been generated before
 */

short generatedbefore( PARAMTYPE *P )
{
    char i;

    if( P->S->movgentype != mane ) {
        IF_EQMOVE( MovTab[ Depth ], P->bestline[ Depth ] ) 
			return 1;

        if( !P->S->capturesearch )
            if( P->S->movgentype != kill )
                for( i = 0; i < 2; i++ )
                    IF_EQMOVE( MovTab[ Depth ], killingmove[ Depth ][ i ] )
                        return 1;
    }
    return 0;
}


/*
 *  Test cut-off.  Cutval cantains the maximal possible evaluation
 */

inline short cut( int cutval, PARAMTYPE *P )
{
    short ct = 0;

    if( cutval <= P->alpha ) {
        ct = 1;
        if( P->S->maxval < cutval ) 
			P->S->maxval = cutval;
    }
    return ct;
}


/*
 *  Perform move, calculate evaluation, test cut-off, etc
 */

static short update( PARAMTYPE *P )
{
    short selection;

    IncNode( &Nodes );
    P->S->nextply = P->ply - 1;      /*  Calculate next ply  */

    if( Level == matesearch ) { /*  Matesearch  */
        Perform( &MovTab[ Depth ], 0 );  /*  Perform Move on the board  */
        
        if( Attacks( Opponent, PieceTab[ Player ][ 0 ].isquare ) ) {		/*  Check if Move is legal  */
			Perform( &MovTab[ Depth ], 1 );	// take back the move
			if( Analysis )
			   DisplayMove();

			return 1;
		}

        if( !Depth ) 
			LegalMoves++;

        checktab[ Depth ]     = 0;
        passedpawn[ Depth ]   = -1;
        P->S->next.value      = 
		P->S->next.evaluation = 0;

        if( P->S->nextply <= 0 ) { /*  Calculate chech and perform evt. cut-off  */
            if( !P->S->nextply )
                checktab[ Depth ] = Attacks( Player, PieceTab[ Opponent ][ 0 ].isquare );

            if( !checktab[ Depth ] )
                if( cut( P->S->next.value, P ) ) {
					Perform( &MovTab[ Depth ], 1 );	// take back the move
					if( Analysis )
					   DisplayMove();

					return 1;
				}
        }

		if( Analysis)
			DisplayMove();

		return 0;					// Accept the move

    }

    /*  Make special limited capturesearch at first iteration  */
    if( MaxDepth <= 1 )
        if( P->S->capturesearch && Depth >= 2 )
            if( !( ( MovTab[ Depth ].content < MovTab[ Depth ].movpiece ) 
				|| ( P->S->movgentype == specialcap ) 
				|| ( MovTab[ Depth ].old == MovTab[ Depth-2 ].new1 ) )
			) {
				if( Analysis )			// cut the move
				   DisplayMove();

				return 1;
			}

    /*  Calculate nxt static incremental evaluation  */
    P->S->next.value = -P->inf->value + StatEvalu( &MovTab[ Depth ] );

    /*  Calculate checktab (only checks with moved piece are calculated) Giving Check does not count as a ply  */
    checktab[ Depth ] = PieceAttacks( MovTab[ Depth ].movpiece, Player, MovTab[ Depth ].new1, PieceTab[ Opponent ][ 0 ].isquare );

    if( checktab[ Depth ] ) 
		P->S->nextply = P->ply;

    /*  Calculate passedpawn.  Moving a pawn to 7th rank does not count as a ply  */
    passedpawn[ Depth ] = passedpawn[ Depth - 2 ];

    if( MovTab[ Depth ].movpiece == pawn )
        if( ( MovTab[ Depth ].new1 < 0x18) || ( MovTab[ Depth ].new1 >= 0x60 ) ) {
            passedpawn[ Depth ] = MovTab[ Depth ].new1;
            P->S->nextply = P->ply;
        }

	/*  Perform selection at last ply and in capture search  */
    selection = ( ( P->S->nextply <= 0 ) && !checktab[ Depth ] && ( Depth > 0 ) );
    if( selection )												/*  check evaluation  */
        if( cut( P->S->next.value + 0, P ) ) {
			if( Analysis )					
			   DisplayMove();

			return 1;						// cut the move
		}

    Perform( &MovTab[ Depth ], 0 );  /*  perform move on the board  */

    /*  check if move is legal  */
    if( Attacks( Opponent, PieceTab[ Player ][ 0 ].isquare ) ) {
		Perform( &MovTab[ Depth ], 1 );				// take back the move
		if( Analysis )
		   DisplayMove();

		return 1;
	}

    if( passedpawn[ Depth ] >= 0 )							/*  check passedpawn  */
        if( Board[ passedpawn[ Depth ] ].piece != pawn || Board[ passedpawn[ Depth ] ].color != Player )
            passedpawn[ Depth ] = -1;

    if( !Depth ) {
        LegalMoves++;
        P->S->next.value += random( 4 );
    }

    P->S->next.evaluation = P->S->next.value;

    if( Analysis)
		DisplayMove();

    return 0;
}


/*
 *  Calculate draw bonus/penalty, and set draw if the game is a draw
 */

static short drawgame(SEARCHTYPE *S)
{
    int drawcount;
    unsigned char searchrepeat;
    unsigned char searchfifty;

    if( Depth == 1 ) {
        searchfifty = FiftyMoveCnt();
        searchrepeat = Repetition( 0 );
        if( searchrepeat >= 3 ) {
            S->next.evaluation = 0;
            return 1;
        }

        drawcount = 0;
        if( searchfifty >= 96 )  /*  48 moves without pawn moves or captures */
            drawcount = 3;
        else {
            if( searchrepeat >= 2 )  /*  2nd repetition  */
                drawcount = 2;
            else if( searchfifty >= 20 )  /*  10 moves without pawn moves or  */
               drawcount = 1;        /*  captures  */
        }
        S->next.value += ( repeatevalu / 4 ) * drawcount;
        S->next.evaluation += ( repeatevalu / 4 ) * drawcount;
    }

    if( Depth >= 3 ) {
        searchrepeat = Repetition( 1 );
        if( searchrepeat >= 2 ) {       /*  Immediate repetition counts as a draw */
            S->next.evaluation = 0;
            return 1;
        }
    }

    return 0;
}


/*
 *  Update bestline and MainEvalu using line and maxval
 */

inline void updatebestline(PARAMTYPE *P)
{
    memcpy( P->bestline, &P->S->line[ 0 ], sizeof( MOVESTRUCT ) * 23 );

    P->bestline[ Depth ] = MovTab[ Depth ];
    if( !Depth) {
        MainEvalu = P->S->maxval;
        if( Level == matesearch )
            P->S->maxval = alphawindow;

        if( Analysis) 
			PrintBestMove( &MainLine[ 0 ], MainEvalu );
    }
}


/*
 *  The inner loop of the search procedure.  MovTab[ Depth ] contains the move.
 */

static short loopbody(PARAMTYPE *P)
{
    ENUMCOLOR oldplayer;
    short lastanalysis;

    if( generatedbefore( P ) ) 
		return 0;

    if( Depth < 23 ) {
        P->S->line[ Depth + 1 ] = ZeroMove;
        if( P->S->movgentype == mane )
            memmove( &P->S->line[ 0 ], P->bestline, sizeof( MOVESTRUCT ) * 23 );
    }

    /*  Principvar indicates principal variation search  */
    /*  Zerowindow indicates zero - width alpha - beta window  */
    P->S->next.principvar = 0;
    P->S->zerowindow = 0;
    if( P->inf->principvar )
        if( P->S->movgentype == mane )
            P->S->next.principvar = ( P->bestline[ Depth+1 ].movpiece != empty );
        else
            P->S->zerowindow = ( P->S->maxval >= P->alpha );

REPEATSEARCH:
    if( update( P ) ) 
		return 0;

    if( Level == matesearch )  /*  stop evt. search  */
        if( ( P->S->nextply <= 0 ) && !checktab[ Depth ] ) 
			goto NOTSEARCH;

    if( drawgame( P->S ) ) 
		goto NOTSEARCH;

    if( Depth >= 23 )
        goto NOTSEARCH;

    /*  Analyse nextply using a recursive call to search  */
    oldplayer = Player;
    Player    = Opponent;
    Opponent  = oldplayer;
    Depth++;

    if( P->S->zerowindow )
        P->S->next.evaluation = -search( -P->alpha - 1, -P->alpha, P->S->nextply, &P->S->next, &P->S->line[ 0 ] );
    else
        P->S->next.evaluation = -search(-P->beta, -P->alpha, P->S->nextply, &P->S->next, &P->S->line[ 0 ] );

    Depth--;
    oldplayer = Opponent;
    Opponent  = Player;
    Player    = oldplayer;

NOTSEARCH:
    Perform( &MovTab[ Depth ], 1 );  /*  take back move  */
    if( SkipSearch )
        return 1;

    lastanalysis = Analysis;  /*  scan messages and test SkipSearch  */
    MessageScan();
    if( !SkipSearch )
        if( Analysis && !SingleStep && ( ( !Depth ) || !lastanalysis ) ) {
            StopTime( &ChessClock );
            if( MainEvalu > alphawindow )
                SkipSearch = ( ChessClock.totaltime >= ( WantedTime * 1.5 ) );
        }

    if( Analysis && ( MaxDepth <= 1 ) )
        SkipSearch = 0;

    P->S->maxval = max( P->S->maxval, P->S->next.evaluation );  /*  Update Maxval  */
    IF_EQMOVE(P->bestline[ Depth ], MovTab[ Depth ] )  /*  Update evt. bestline  */
        updatebestline( P );

    if( P->alpha < P->S->maxval ) {     /*  update alpha and test cutoff */
        updatebestline( P );
        if( P->S->maxval >= P->beta )
            return 1;

        /*  Adjust maxval (tolerance search)  */
        if( P->ply >= 2  && P->inf->principvar && !P->S->zerowindow )
            P->S->maxval = min( P->S->maxval + TOLERANCE, P->beta - 1 );

        P->alpha = P->S->maxval;

        if( P->S->zerowindow && ! SkipSearch ) {
            P->S->zerowindow = 0;			/*  repeat search with full window  */
            goto REPEATSEARCH;
        }
    }

    return SkipSearch;
}


/*
 *  generate  pawn promotions
 */

static short pawnpromotiongen(PARAMTYPE *P)
{
    ENUMPIECE promote;

    MovTab[ Depth ].spe = 1;

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

static short capmovgen(int newsq, PARAMTYPE *P)
{
    int nxtsq, sq;
    int i;

    MovTab[Depth].content  = Board[ newsq ].piece;
    MovTab[Depth].spe      = 0;
    MovTab[Depth].new1     = newsq;
    MovTab[Depth].movpiece = pawn;  /*  pawn captures  */

    nxtsq = MovTab[ Depth ].new1 - ( (Player == white) ? 0x10 : -0x10 );

    for( sq = nxtsq - 1; sq <= nxtsq + 1; sq++) {
        if( ( sq != nxtsq ) && ( VALIDSQUARE( sq ) ) && ( Board[ sq ].piece == pawn && Board[ sq ].color == Player ) ) {
            MovTab[ Depth ].old = sq;

            if( MovTab[ Depth ].new1 < 8 || MovTab[ Depth ].new1 >= 0x70 ) {
                if( pawnpromotiongen( P ) )
                    return 1;
            } else if( loopbody( P ) )
                return 1;
        }
	}

    for( i = OfficerNo[ Player ]; i >= 0; i-- ) { /*  other captures  */
        if( PieceTab[ Player ][ i ].ipiece != empty && PieceTab[ Player ][ i ].ipiece != pawn ) {
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
    char first, last, dir;
    int direction;
    int newsq;

    MovTab[Depth].spe      = 0;
    MovTab[Depth].old      = oldsq;
    MovTab[Depth].movpiece = Board[ oldsq ].piece;
    MovTab[Depth].content  = empty;

    switch( MovTab[ Depth ].movpiece ) {
    case king :
        for( dir = 7; dir >= 0; dir-- ) {
            newsq = MovTab[ Depth ].old + DirTab[ dir ];
            if( VALIDSQUARE( newsq ) && ( Board[ newsq ].piece == empty ) ) {
                MovTab[ Depth ].new1 = newsq;
                if( loopbody( P ) )
                     return 1;
            }
        }
        break;

    case knight :
        for( dir = 7; dir >= 0; dir-- ) {
            newsq = MovTab[ Depth ].old + KnightDir[ dir ];
            if( VALIDSQUARE( newsq ) && ( Board[newsq].piece == empty) ) {
                MovTab[ Depth ].new1 = newsq;
                if( loopbody( P ) )
                    return 1;
            }
        }
        break;

    case queen :
    case rook  :
    case bishop :
        first = ( ( MovTab[ Depth ].movpiece == rook   ) ? 3 : 7 );
        last  = ( ( MovTab[ Depth ].movpiece == bishop ) ? 4 : 0 );

        for( dir = first; dir >= last; dir-- ) {

            newsq = MovTab[ Depth ].old + DirTab[ dir ];

            while( VALIDSQUARE( newsq ) && ( Board[ newsq ].piece == empty ) ) {

                MovTab[ Depth ].new1 = newsq;
                if( loopbody( P ) )
                    return 1;

                newsq = MovTab[ Depth ].new1 + DirTab[ dir ];
            }
        }
        break;

    case pawn :
        /*  One square forward  */
        MovTab[Depth].new1 = MovTab[Depth].old + ( (Player == white) ? 0x10 : -0x10 )
        if( Board[MovTab[Depth].new1].piece == empty)
            if( MovTab[Depth].new1 < 8 || MovTab[Depth].new1 >= 0x70)
            {
                if( pawnpromotiongen(P)) return 1;
            }
            else
            {
                if( loopbody(P))
                    return 1;
                if( MovTab[Depth].old < 0x18 || MovTab[Depth].old
                            >= 0x60)
                {
                    /*  two squares forward  */
                    MovTab[Depth].new1 += (MovTab[Depth].new1
                                - MovTab[Depth].old);
                    if( Board[MovTab[Depth].new1].piece == empty)
                        if( loopbody(P))
                            return 1;
                }
           }
    } /*  switch  */
    return 0;
}


/*
 *  castling moves
 */

static short castlingmovgen(PARAMTYPE *P)
{
    ENUMCASTDIR castdir;

    MovTab[Depth].spe      = 1;
    MovTab[Depth].movpiece = king;
    MovTab[Depth].content  = empty;

    for( castdir = (ENUMCASTDIR)( lng-1 ); castdir <= shrt-1; ( (int)castdir )++ ) {

        MovTab[ Depth ].new1 = CastMove[ Player ][ castdir ].castnew;
        MovTab[ Depth ].old  = CastMove[ Player ][ castdir ].castold;

        if( KillMovGen( &MovTab[ Depth ] ) )
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
    MovTab[ Depth ].content  = empty;
    MovTab[ Depth ].new1     = ( MovTab[ Depth - 1 ].new1 + MovTab[ Depth - 1 ].old ) / 2;	// Square that the pawn hopped over

	// Do we have a pawn that can kill the pawn from the left
	if( VALIDSQUARE( MovTab[ Depth - 1 ].new1 - 1 ) ) {

        MovTab[ Depth ].old = MovTab[ Depth - 1 ].new1 - 1;
        if( KillMovGen( &MovTab[ Depth ] ) )
            if( loopbody( P ) )
                return 1;										// Yes, kill it
	}

	// Do we have a pawn that can kill the pawn from the right
	if( VALIDSQUARE( MovTab[ Depth - 1 ].new1 + 1 ) ) {

        MovTab[ Depth ].old = MovTab[ Depth - 1 ].new1 + 1;
        if( KillMovGen( &MovTab[ Depth ] ) )
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
    if( P->bestline[ Depth ].movpiece != empty ) {
        MovTab[ Depth ] = P->bestline[ Depth ];
        P->S->movgentype = mane;
        if( loopbody( P ) ) 
			return;
    }

    if( ( MovTab[ Depth - 1 ].movpiece != empty ) && ( MovTab[ Depth - 1 ].movpiece != king ) ) {
        P->S->movgentype = specialcap;
        if( capmovgen( MovTab[ Depth - 1 ].new1, P ) ) 
			return;
    }

    P->S->movgentype = kill;
    if( !P->S->capturesearch )
        for( killno = 0; killno <= 1; killno++) {
            MovTab[ Depth ] = killingmove[ Depth ][ killno ];
            if( MovTab[ Depth - 1 ].movpiece != empty )
                if( KillMovGen( &MovTab[ Depth ] ) )
                    if( loopbody( P ) ) 
						return;
        }


    P->S->movgentype = norml;
    for( index = 1; index <= PawnNo[ Opponent ]; index++ )
        if( PieceTab[ Opponent ][ index ].ipiece != empty )
            if( MovTab[ Depth-1 ].movpiece == empty || PieceTab[ Opponent ][ index ].isquare != MovTab[ Depth-1 ].new1 )
                if( capmovgen( PieceTab[ Opponent ][ index ].isquare, P ) )
                    return;

    if( P->S->capturesearch && ( passedpawn[ Depth-2 ] >= 0 ) && ( Board[ passedpawn[ Depth-2 ] ].piece == pawn && Board[ passedpawn[ Depth-2 ] ].color == Player ) ) {
        if( noncapmovgen( passedpawn[ Depth-2 ], P ) ) 
			return;
    }

    if( !P->S->capturesearch ) {               /*  non-captures  */
        if( castlingmovgen( P ) )
            return;      /*  castling  */

        for( index = PawnNo[ Player ]; index >= 0; index-- )
            if( PieceTab[ Player ][ index ].ipiece != empty )
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

    S.capturesearch = ( ( ply <= 0 ) && !checktab[ Depth-1 ] );		/*  Perform capturesearch if ply <= 0 and !check  */

    if( S.capturesearch ) {											/*  initialize maxval  */
        S.maxval = -inf->evaluation;
        if( alpha < S.maxval ) {
            alpha = S.maxval;
            if( S.maxval >= beta ) 
			    return S.maxval;
        }
    } else
        S.maxval = -( LOSEVALUE - Depth * DEPTHFACTOR );

    P.alpha = alpha;
    P.beta = beta;
    P.ply = ply;
    P.inf = inf;
    P.bestline = bestline;
    P.S = &S;
    searchmovgen( &P );   /*  The search loo`  */

    if( !SkipSearch ) {

		if( S.maxval == -( LOSEVALUE - Depth * DEPTHFACTOR ) )   /*  Test stalemate  */
			if( !Attacks( Opponent, PieceTab[ Player ][ 0 ].isquare ) )
				return 0;

		updatekill( &bestline[ Depth ] );
	}

    return S.maxval;
}


/*
 *  Begin the search
 */

static int callsearch( int alpha, int beta )
{
    int maxval;

    startinf.principvar = ( MainLine[0].movpiece != empty );
    LegalMoves = 0;

    maxval = search( alpha, beta, MaxDepth, &startinf, &MainLine[ 0 ] );
    if( !LegalMoves)
        MainEvalu = maxval;

    return maxval;
}


/*
 *  Checks whether the search time is used
 */

inline short timeused( void )
{
    short tu = 0;

    if( Analysis && !SingleStep ) {
        StopTime( &ChessClock );
        tu = ( ChessClock.totaltime >= WantedTime );
    }
    return tu;
}


/*
 *  setup search
 */

void FindMove( int maxlevel )
{
    int    maxval;
    double calcpvtime;

    InitTime( &ChessClock );
    StartTime( &ChessClock );
    InitNode( &Nodes );
    SkipSearch = 0;
    clearkillmove();
    CalcPVTable();
    StopTime( &ChessClock );

    calcpvtime          = ChessClock.totaltime;
    startinf.value      = 
	startinf.evaluation = -RootValue;
    MaxDepth            = 0;
    MainLine[0]         = ZeroMove;
    MainEvalu           = RootValue;
    alphawindow         = MAXINT;
    ComputerThinking    = TRUE;

    do {
        /*  update various variables  */
        if( MaxDepth <= 1 ) 
			repeatevalu = MainEvalu;

        alphawindow = min( alphawindow, MainEvalu - 0x80 );

        if( Level == matesearch ) {
            alphawindow = 0x6000;
            if( MaxDepth > 0) 
				MaxDepth++;
        }

        MaxDepth++;
        maxval = callsearch( alphawindow, 0x7f00 );  /*  perform the search  */

		/*  Repeat the search if the value falls below the alpha-window  */
        if( maxval <= alphawindow && !SkipSearch && Level != matesearch && LegalMoves > 0) {
            MainEvalu  = alphawindow;						
            maxval     = callsearch( -0x7F00, alphawindow - TOLERANCE * 2 );
            LegalMoves = 2;
        }
    } while( !SkipSearch && !timeused() && ( MaxDepth < maxlevel ) && ( LegalMoves > 1 ) && ( abs(MainEvalu) < MATEVALUE - 24 * DEPTHFACTOR ) );

   ComputerThinking = FALSE;
   StopTime( &ChessClock );

   if( Analysis )
      PrintNodes( &Nodes, ( ChessClock.totaltime - calcpvtime ) );
}

