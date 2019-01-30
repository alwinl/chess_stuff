// ObjectWindows - (C) Copyright 1992 by Borland International

#include <math.h>
#include "wcdefs.h"
#include "externs.h"

/*
 *  Globals
 */

static ATTACKTABTYPE attack[240];
ATTACKTABTYPE *AttackTab = &attack[120];
short BitTab[7] = {0, 1, 2, 4, 8, 0x10, 0x20};
int DirTab[8] = { 1, -1, 0x10, -0x10, 0x11, -0x11, 0x0f, -0x0f};
int KnightDir[8] = {0x0E, -0x0E, 0x12, -0x12, 0x1f, -0x1f, 0x21, -0x21};
int PawnDir[2] = {0x10, -0x10};
MOVESTRUCT Next;
int GeneratedMoveCount, BufPnt;
MOVESTRUCT GeneratedMoves[81];
CASTMOVETYPE  CastMove[2][2] = { {{2, 4}, {6, 4}}, {{0x72, 0x74}, {0x76, 0x74}} };


/*
 * Calculate whether acolor attacks the square with a pawn
 * To see if white attacks a square we need to look one rank down
 * To see if black attacks a square we need to look one rank up
 * Next we need to look at either side of that square.
 */
static bool PawnAttacks( ENUMCOLOR acolor, int square )
{
    square += ( ( acolor == black ) ? 0x10 : -0x10 );   /* go to the right rank */

    if( VALIDSQUARE( (square - 1) ) )       /*  left square  */
        if( Board[ (square - 1) ].piece == pawn && Board[ (square - 1) ].color == acolor )
            return true;        /* there is a pawn attacking the square */

    if( VALIDSQUARE( (square + 1) ) )       /*  right square  */
        if( Board[ (square + 1) ].piece == pawn && Board[ (square + 1) ].color == acolor )
            return true;        /* there is a pawn attacking the square */

    return false;        /* no pawn is attacking the square */
}

/*
 * Check whether inpiece is placed on square and has never moved
 * If it has, castling is not allowed
 */
static bool Check( int square, ENUMPIECE inpiece, ENUMCOLOR incolor )
{
    short dep;

    if( Board[square].piece != inpiece || Board[square].color != incolor )
        return false;       /* The piece is not even at its origin location */

    dep = Depth - 1;

    while( MovTab[dep].movpiece != no_piece ) {    /* check all recorder moves */
        if( MovTab[dep].new1 == square )
            return false;                       /* found a move where the piece moved to this square */
        dep--;
    }

    return true;                /* piece has never moved */
}

/*
 *  Is this move not a pawn move or a capture move?
 */
static bool RepeatMove( MOVESTRUCT *amove )
{
    return ( amove->movpiece != no_piece && amove->movpiece != pawn && amove->content == no_piece && !amove->spe );
}

/*
 *  Generates pawnpromotion
 */
static void PawnPromotionGen( int new_square, int old_square, int content )
{
    MOVESTRUCT pawnpromotion_move;

    pawnpromotion_move.new1 = new_square;
    pawnpromotion_move.old = old_square;
    pawnpromotion_move.spe = 1;
    //pawnpromotion_move.movpiece;  /* filled below  */
    pawnpromotion_move.content = content;

    for( ENUMPIECE promote = queen; promote <= knight; ( ( int )promote )++ ) {

        pawnpromotion_move.movpiece = promote;

        GeneratedMoves[++GeneratedMoveCount] = pawnpromotion_move;                     /* Store the move in buffer */
    }
}

/*
 *  Generates captures of the piece on new1 using PieceTab
 */
static void CapMovGen( void )
{
    int nextsq, sq;
    int i;

    Next.spe = 0;
    Next.content = Board[Next.new1].piece;
    Next.movpiece = pawn;

    nextsq = Next.new1 - ( ( Player == white ) ? 0x10 : -0x10 );

    for( sq = nextsq-1; sq <= nextsq+1; sq+=2 )
        if( VALIDSQUARE( sq ) )
            if( Board[sq].piece == pawn && Board[sq].color == Player ) {
                Next.old = sq;
                if( Next.new1 < 8 || Next.new1 >= 0x70 )
                    PawnPromotionGen( Next.new1, Next.old, Next.content );
                else
                    GeneratedMoves[++GeneratedMoveCount] = Next;        /* Store the move in buffer */
            }

    /*  Other captures, starting with the smallest pieces  */
    for( i = OfficerNo[Player]; i >= 0; i-- )
        if( PieceTab[Player][i].ipiece != no_piece && PieceTab[Player][i].ipiece != pawn )
            if( PieceAttacks( PieceTab[Player][i].ipiece, Player, PieceTab[Player][i].isquare, Next.new1 ) ) {

                Next.old = PieceTab[Player][i].isquare;
                Next.movpiece = PieceTab[Player][i].ipiece;

                GeneratedMoves[++GeneratedMoveCount] = Next;    /* Store the move in buffer */
            }
}

/*
 *  generates non captures for the piece on old
 */
static void NonCapMovGen( void )
{
    unsigned int first, last, dir;
    int direction;
    int  newsq;

    Next.spe = 0;
    Next.movpiece = Board[Next.old].piece;
    Next.content = no_piece;

    switch( Next.movpiece ) {
    case king :
        for( dir = 7; dir >= 0; dir-- ) {
            newsq = Next.old + DirTab[dir];
            if( VALIDSQUARE( newsq ) )
                if( Board[newsq].piece == no_piece ) {
                    Next.new1 = newsq;
                    /* Store the move in buffer */
                    GeneratedMoveCount++;
                    GeneratedMoves[GeneratedMoveCount] = Next;
                }
        }
        break;
    case knight :
        for( dir = 7; dir >= 0; dir-- ) {
            newsq = Next.old + KnightDir[dir];
            if( VALIDSQUARE( newsq ) )
                if( Board[newsq].piece == no_piece ) {
                    Next.new1 = newsq;
                    /* Store the move in buffer */
                    GeneratedMoveCount++;
                    GeneratedMoves[GeneratedMoveCount] = Next;
                }
        }
        break;
    case queen :
    case rook  :
    case bishop:
        first = 7;
        last = 0;
        if( Next.movpiece == rook )
            first = 3;
        if( Next.movpiece == bishop )
            last = 4;
        for( dir = first; dir >= last; dir-- ) {
            direction = DirTab[dir];
            newsq = Next.old + direction;
            /*  Generate all non captures in the direction  */
            while( VALIDSQUARE( newsq ) ) {
                if( Board[newsq].piece != no_piece )
                    goto TEN;
                Next.new1 = newsq;
                /* Store the move in buffer */
                GeneratedMoveCount++;
                GeneratedMoves[GeneratedMoveCount] = Next;
                newsq = Next.new1 + direction;
            }
TEN:
            continue;
        }
        break;
    case pawn :
        Next.new1 = Next.old + PawnDir[Player];  /*  one square forward  */
        if( Board[Next.new1].piece == no_piece ) {
            if( Next.new1 < 8 || Next.new1 >= 0x70 )
                PawnPromotionGen( Next.new1, Next.old, Next.content  );
            else {
                /* Store the move in buffer */
                GeneratedMoveCount++;
                GeneratedMoves[GeneratedMoveCount] = Next;
                if( Next.old < 0x18 || Next.old >= 0x60 ) {
                    Next.new1 += ( Next.new1 - Next.old ); /* 2 squares forward */
                    if( Board[Next.new1].piece == no_piece ) {
                        /* Store the move in buffer */
                        GeneratedMoveCount++;
                        GeneratedMoves[GeneratedMoveCount] = Next;
                    }
                }
            }
        }
    }  /* switch */
}

/****************************************************************************/

void CalcAttackTab( void )
{
    for( int square = -0x77; square <= 0x77; square++ ) {
        AttackTab[square].pieceset = 0;
        AttackTab[square].direction = 0;
    }

    for( char dir = 7; dir >=0; dir-- ) {
        for( unsigned char i = 1; i < 8; i++ ) {
            if( dir < 4 )
                AttackTab[DirTab[dir]*i].pieceset = BitTab[queen]+BitTab[rook];
            else
                AttackTab[DirTab[dir]*i].pieceset = BitTab[queen]+BitTab[bishop];
            AttackTab[DirTab[dir]*i].direction = DirTab[dir];
        }
        AttackTab[DirTab[dir]].pieceset += BitTab[king];
        AttackTab[KnightDir[dir]].pieceset = BitTab[knight];
        AttackTab[KnightDir[dir]].direction = KnightDir[dir];
    }
}

/*
 *  calculate whether apiece placed on asquare attacks the square
 */
short PieceAttacks( ENUMPIECE apiece, ENUMCOLOR acolor, int asquare, int square )
{
    int sq;
    int x;

    x = square - asquare;
    if( apiece == pawn )  /*  pawn attacks  */
        return ( abs( x - ( ( acolor == white ) ? 0x10 : -0x10 ) ) == 1 );
    /*  other attacks: can the piece move to the square?  */
    else if( AttackTab[x].pieceset & BitTab[apiece] ) {
        if( apiece == king || apiece == knight )
            return 1;
        else {
            /*  are there any blocking pieces in between?  */
            sq = asquare;
            do {
                sq += AttackTab[x].direction;
            } while( sq != square && Board[sq].piece == no_piece );
            return ( sq == square );
        }
    } else
        return 0;
}

/*
 *  Calculates whether acolor attacks the square
 */
short Attacks( ENUMCOLOR acolor, int square )
{
    int i;

    if( PawnAttacks( acolor, square ) ) /*  pawn attacks  */
        return 1;
    /*  Other attacks:  try all pieces, starting with the smallest  */
    for( i = OfficerNo[acolor]; i >= 0; i-- )
        if( PieceTab[acolor][i].ipiece != no_piece )
            if( PieceAttacks( PieceTab[acolor][i].ipiece, acolor, PieceTab[acolor][i].isquare, square ) )
                return 1;
    return 0;
}

/*
 *  Calculate whether incolor can castle
 */
void CalcCastling( ENUMCOLOR incolor,  ENUMCASTDIR *cast )
{
    int square = 0;

    if( incolor == black )
        square = 0x70;

    *cast = zero;

    if( Check( square + 4, king, incolor ) ) { /*  check king  */

        if( Check( square, rook, incolor ) )
            ( ( int )*cast ) += lng; /*  check a-rook  */

        if( Check( square + 7, rook, incolor ) )
            ( ( int )*cast ) += shrt; /*  check h-rook  */
    }
}

/*
 *  Count the number of moves since last capture or pawn move
 *  The game is a draw when fiftymovecnt = 100
 */
unsigned int FiftyMoveCnt( void )
{
    unsigned int cnt = 0;

    while( RepeatMove( &MovTab[Depth - cnt] ) )
        cnt++;
    return cnt;
}

/*
 *  Calculate how many times the position has occured before
 *  The game is a draw when repetition = 3;
 *  movetab[back..Depth] contains the previous moves
 *  When immediate is set, only immediate repetion is checked
 */
unsigned int Repetition( short immediate )
{
    short lastdep, compdep, tracedep, checkdep, samedepth;
    int tracesq, checksq;
    unsigned int repeatcount;

    repeatcount = 1;
    lastdep = samedepth = Depth + 1;    /*  current postion  */
    compdep = samedepth - 4;            /*  First position to compare  */

    /*  MovTab[lastdep..Depth] contains previous relevant moves  */
    while( RepeatMove( &MovTab[lastdep - 1] ) && ( compdep < lastdep || !immediate ) )
        lastdep--;

    if( compdep < lastdep )
        return repeatcount;

    checkdep = samedepth;
    do {
        checkdep--;
        checksq = MovTab[checkdep].new1;
        for( tracedep = checkdep + 2; tracedep < samedepth; tracedep += 2 )
            if( MovTab[tracedep].old == checksq )
                goto TEN;

        /*  Trace the move backward to see if it has been 'undone' earlier  */
        tracedep = checkdep;
        tracesq = MovTab[tracedep].old;
        do {
            if( tracedep-2 < lastdep )
                return repeatcount;
            tracedep -= 2;
            /*  Check if piece has been moved before  */
            if( tracesq == MovTab[tracedep].new1 )
                tracesq =
                    MovTab[tracedep].old;
        } while( tracesq != checksq || tracedep > compdep + 1 );
        if( tracedep < compdep ) { /*  Adjust evt. compdep  */
            compdep = tracedep;
            if( ( samedepth - compdep ) % 2 == 1 ) {
                if( compdep == lastdep )
                    return repeatcount;
                compdep --;
            }
            checkdep = samedepth;
        }
        /*  All moves between SAMEDEP and compdep have been checked,
            so a repetition is found  */
TEN :
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
 *  Test whether a move is possible
 *
 *  On entry:
 *    Move contains a full description of a move, which
 *    has been legally generated in a different position.
 *    MovTab[Depth - 1] contains last performed move.
 *
 *  On exit:
 *    KillMovGen indicates whether the move is possible
 */
short KillMovGen( MOVESTRUCT *amove )
{
    int castsq;
    ENUMPIECE promote;
    ENUMCASTDIR castdir;
    ENUMCASTDIR cast;
    short killmov;

    killmov = 0;
    if( amove->spe && ( amove->movpiece == king ) ) {
        CalcCastling( Player, &cast );   /*  Castling  */
        if( amove->new1 > amove->old )
            castdir = shrt;
        else
            castdir = lng;

        if( cast & castdir ) { /*  Has king or rook moved before  */
            castsq = ( int )( ( amove->new1 + amove->old ) / 2 );
            /*  Are the squares no_piece ?  */
            if( Board[amove->new1].piece == no_piece )
                if( Board[castsq].piece == no_piece )
                    if( ( amove->new1 > amove->old ) || ( Board[amove->new1-1].piece
                                                        == no_piece ) )
                        /*  Are the squares unattacked  */
                        if( !Attacks( Opponent, amove->old ) )
                            if( !Attacks( Opponent, amove->new1 ) )
                                if( !Attacks( Opponent, castsq ) )
                                    killmov = 1;
        }
    } else {
        if( amove->spe && ( amove->movpiece == pawn ) ) {
            /*  E.p. capture  */
            /*  Was the Opponent's move a 2 square move?  */
            if( MovTab[Depth-1].movpiece == pawn )
                if( abs( MovTab[Depth-1].new1 - MovTab[Depth-1].old ) >= 0x20 )
                    if( ( Board[amove->old].piece == pawn ) && ( Board[amove->old].color
                            == Player ) )
                        killmov = ( amove->new1 == ( ( MovTab[Depth-1].new1
                                                      + MovTab[Depth-1].old ) / 2 ) );
        } else {
            if( amove->spe ) {               /*  Normal test  */
                promote = amove->movpiece;   /*  Pawnpromotion  */
                amove->movpiece = pawn;
            }

            /*  Is the content of Old and New1 squares correct?  */
            if( Board[amove->old].piece == amove->movpiece )
                if( Board[amove->old].color == Player )
                    if( Board[amove->new1].piece == amove->content )
                        if( amove->content == no_piece || Board[amove->new1].color == Opponent ) {
                            if( aove->movpiece == pawn ) { /*  Is the move possible?  */
                                if( abs( amove->new1 - amove->old ) < 0x20 )
                                    killmov = 1;
                                else
                                    killmov = Board[( amove->new1+amove->old ) / 2].piece == no_piece;
                            } else
                                killmov = PieceAttacks( amove->movpiece, Player,
                                                        amove->old, amove->new1 );
                        }
            if( amove->spe )
                amove->movpiece = promote;
        }
    }
    return killmov;
}

/*
 *  The move generator.
 *  InitMovGen generates all possible moves and places them in a buffer.
 *  Movgen will the generate the moves one by one and place them in next.
 *
 *  On entry:
 *    Player contains the color to move.
 *    MovTab[Depth-1] the las performed move.
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
    ENUMCASTDIR castdir;
    int sq;
    int index;

    GeneratedMoveCount = BufPnt = 0;

    /* generate all captures starting with captures of largest pieces */
    for( index = 1; index <= PawnNo[Opponent]; index++ )
        if( PieceTab[Opponent][index].ipiece != no_piece ) {
            Next.new1 = PieceTab[Opponent][index].isquare;
            CapMovGen();
        }

    Next.spe = 1;
    Next.movpiece = king;
    Next.content = no_piece;

    for( castdir = ENUMCASTDIR( lng - 1 ); castdir <= shrt - 1; ( ( int )castdir )++ ) {
        Next.new1 = CastMove[Player][castdir].castnew;
        Next.old = CastMove[Player][castdir].castold;
        if( KillMovGen( &Next ) ) {
            /* Store the move in buffer */
            GeneratedMoveCount++;
            GeneratedMoves[GeneratedMoveCount] = Next;
        }
    }

    /* generate non captures, starting with pawns */
    for( index = PawnNo[Player]; index >= 0; index-- )
        if( PieceTab[Player][index].ipiece != no_piece ) {
            Next.old = PieceTab[Player][index].isquare;
            NonCapMovGen();
        }

    if( MovTab[Depth - 1].movpiece == pawn ) {  /* E.p. captures */
        if( abs( MovTab[Depth - 1].new1 - MovTab[Depth - 1].old ) >= 0x20 ) {
            Next.spe = 1;
            Next.movpiece = pawn;
            Next.content = no_piece;
            Next.new1 = ( MovTab[Depth - 1].new1 + MovTab[Depth - 1].old ) / 2;

            for( sq = MovTab[Depth - 1].new1 - 1; sq <= MovTab[Depth - 1].new1 + 1; sq++ )
                if( sq != MovTab[Depth - 1].new1 )
                    if( VALIDSQUARE( sq ) ) {
                        Next.old = sq;
                        if( KillMovGen( &Next ) ) {
                            /* Store the move in buffer */
                            GeneratedMoveCount++;
                            GeneratedMoves[GeneratedMoveCount] = Next;
                        }
                    }
        }
    }
}

/*
 *  place next move from the buffer in next.  Generate zeromove when there
 *  are no more moves.
 */
void MovGen( void )
{
    if( BufPnt >= GeneratedMoveCount )
        Next = ZeroMove;
    else {
        BufPnt++;
        Next = GeneratedMoves[BufPnt];
    }
}
