// ObjectWindows - (C) Copyright 1992 by Borland International

#include <owl\owlpch.h>
#pragma hdrstop

#include <fstream.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <dos.h>
#include <time.h>
#include <ctype.h>

#include "wchess.h"
#include "wcdefs.h"
#include "externs.h"

/*
 *  Global declarations
 */
char        *INIFile = "wchess.ini";
bool        InLibrary;  // True if program is in the opening library
bool        MultiMove,  // True if multimove mode
            AutoPlay,               // True if AutoPlay mode
            SingleStep;             // Single Step mode, used for debugging purposes
ENUMLEVEL   Level;
double      AverageTime = 5.0;
ENUMPIECE   Pieces[8] = { rook, knight, bishop, queen, king, bishop, knight, rook };
char        MaxLevel;
bool        Turned;
bool        UseLib;
unsigned char * Openings;
int         MoveNo;
MOVESTRUCT    PlayerMove;
bool        Logging;
double     Nodes;      // Number of analysed nodes
extern int  LegalMoves;
int     HintEvalu;  /* Evaluation for hintline */

const UNPLAYMARK = 0x3f;


int OpCount, LibNo;

static short LibDepth;
static bool Found;
static short dep;
static MOVESTRUCT[MAXPLY]    HintLine;   /* suggested hint line */

ofstream *OutputFile;

enum CONTROLVAR { readmove, checkmove, gamemove } ;

static CONTROLVAR Control;

void ResetGame()
{
    ClearBoard();

    for( int i = 0; i < 8; i++ ) {
        InsertPiece( Pieces[ i ], white, i );
        InsertPiece( pawn, white, i + 0x10 );
        InsertPiece( Pieces[ i ], black, i + 0x70 );
        InsertPiece( pawn, black, i + 0x60 );
    }

    CalcPieceTab();

    Player = white;
    Opponent = black;
    Running = false;

    ClearDisplay();
    InitDisplay();
    TInfo_SetTurnText( Player );
}

void NewGame()
{
    SingleStep = false;
    InLibrary  = false;
    GameOver   = false;

    ResetGame();
    TInfo_PrintCurLevel();
    ResetMoves();

    UseLib = ( ( !*Openings ) ? 0 : 200 );
    MovTab[-1].content = king;

    InitChessTime();

    ProgramColor = white;
    MoveNo = 0;

    ClearHint();
    ClearPVTable();

    PlayerMove = ZeroMove;

    if( Logging && !AutoPlay )
        *OutputFile << "\n No  Player Program      Hint      Value Level       Nodes    Time\n";

    Nodes = 0;

    setTotalTime( &ChessClock, 0.0 );
    Control = AutoPlay ? gamemove : readmove;
}

void ResetNewPos( void )
{
    ResetMoves();
    CalcPieceTab();

    UseLib  = false;
    Running = false;

    ClearHint();
}

void ResetOpening()
{
    ifstream fin( "opening.lib", ios::in | ios::binary );

    if( !fin ) {
        ::MessageBox( NULL,"Cannot find Openings Library", "Error", MB_ICONHAND | MB_OK );
        Openings = new unsigned char;
        *Openings = 0;
        return;
    }

    Openings = new unsigned char[ 32000 ];
    fin.read( Openings, 32000 );
    fin.close();

    *Openings = 0xFF;
}

void readINIFile()
{
    long color;
    char tmp_buf[80];

    Level = ( ENUMLEVEL )GetPrivateProfileInt( "WCHESS", "Level", ( int )easygame, INIFile );

    if( GetPrivateProfileString( "WCHESS", "AverageTime", "5.0", tmp_buf, 80, INIFile ) )
        sscanf( tmp_buf, "%lf", &AverageTime );

    if( GetPrivateProfileString( "WCHESS", "WhiteSquare", "", tmp_buf, 80, INIFile ) ) {
        sscanf( tmp_buf, "%ld", &color );
        WhiteSquareColors[0] = color & 0xffL;
        WhiteSquareColors[1] = (( color & 0xff00L ) >> 8 );
        WhiteSquareColors[2] = (( color & 0xff0000L ) >> 16 );
    }

    if( GetPrivateProfileString( "WCHESS", "BlackSquare", "", tmp_buf, 80, INIFile ) ) {
        sscanf( tmp_buf, "%ld", &color );
        BlackSquareColors[0] = color & 0xff;
        BlackSquareColors[1] = (( color & 0xff00L ) >> 8 );
        BlackSquareColors[2] = (( color & 0xff0000L ) >> 16 );
    }

    MaxLevel = (char) GetPrivateProfileInt( "WCHESS", "MaxLevel", MAXPLY, INIFile );
    SoundOn  = (bool) GetPrivateProfileInt( "WCHESS", "SoundOn", 1, INIFile );

}

void writeINIFile()
{
    long color;
    char tmp_buf[80];

    sprintf( tmp_buf, "%d", Level );
    WritePrivateProfileString( "WCHESS", "Level", tmp_buf, INIFile );

    sprintf( tmp_buf, "%lf", AverageTime );
    WritePrivateProfileString( "WCHESS", "AverageTime", tmp_buf, INIFile );

    color = RGB( WhiteSquareColors[0], WhiteSquareColors[1], WhiteSquareColors[2] );
    sprintf( tmp_buf, "%ld", color );
    WritePrivateProfileString( "WCHESS", "WhiteSquare", tmp_buf, INIFile );

    color = RGB( BlackSquareColors[0], BlackSquareColors[1], BlackSquareColors[2] );
    sprintf( tmp_buf, "%ld", color );
    WritePrivateProfileString( "WCHESS", "BlackSquare", tmp_buf, INIFile );

    sprintf( tmp_buf, "%d", (int)MaxLevel );
    WritePrivateProfileString( "WCHESS", "MaxLevel", tmp_buf, INIFile );

    sprintf( tmp_buf, "%d", (int)SoundOn );
    WritePrivateProfileString( "WCHESS", "SoundOn", tmp_buf, INIFile );

}

static void StartUp()
{
    long color;

    randomize();

    readINIFile();

    if( !SoundOn )  // defaults to checked at startup
        CheckMenuItem( MainMenu, IDM_SOUND, MF_UNCHECKED );

    MultiMove = false;
    AutoPlay  = false;
    Turned    = false;

    ResetOpening();

    OutputFile = new ofstream( "Chess.log" );
    if( !OutputFile ) {
        ::MessageBox( NULL, "Unable to open log file.\nLogging disabled", "OWL Chess", MB_OK | MB_ICONEXCLAMATION );
        Logging = false;
    } else {
        *OutputFile << endl;
        *OutputFile << "            OWL CHESS by Borland International\n";
        *OutputFile << "            ==================================\n" << endl;
    }
}

/*
 *  Sets libno to the previous move in the block
 */

void PreviousLibNo( void )
{
    int n;

    n = 0;
    do {
        LibNo--;
        if( Openings[LibNo] >= 128 )
            n++;
        if( Openings[LibNo] & 64 )
            n--;
    } while( n );
}

/*
 *  Set libno to the first move in the block
 */
void FirstLibNo()
{
    while( !( Openings[ LibNo - 1 ] & 64 ) )
        PreviousLibNo();
}

/*
 *  set libno to the next move in the block.  Unplayable
 *  moves are skipped if skip is set
 */
void NextLibNo( short skip )
{
    int n;

    if( Openings[ LibNo ] >= 128 )
        FirstLibNo();
    else {
        n = 0;
        do {
            if( Openings[LibNo] & 64 )
                n++;
            if( Openings[LibNo] >= 128 )
                n--;
            LibNo++;
        } while( n );

        if( skip && ( Openings[ LibNo ] == UNPLAYMARK ) )
            FirstLibNo();
    }
}


/*
 *  find the node corresponding to the correct block
 */
static void FindNode( void )
{
    LibNo++;
    if( Depth > LibDepth ) {
        Found = true;
        return;
    }

    OpCount = -1;
    InitMovGen();

    MOVESTRUCT test_move;
    do {
        OpCount++;
        test_move = MovGen();
    } while( test_move.movpiece != no_piece && !EqMove( &test_move, &MovTab[ Depth ] ) );

    if( test_move.movpiece != no_piece ) {
        while( ( ( Openings[ LibNo ] & 63 ) != OpCount ) && ( Openings[ LibNo ] < 128 ) )
            NextLibNo( 0 );

        if( ( Openings[ LibNo ] & 127 ) == 64 + OpCount ) {
            MakeMove( &MovTab[ Depth ] );
            FindNode();
            TakeBackMove( &MovTab[ Depth-1 ] );
        }
    }
}


/*
 *  Set LibNo to the block corresponding to the position
 */
static void CalcLibNo( void )
{
    LibNo = 0;

    if( MoveNo < UseLib ) {
        LibDepth = Depth;

        while( MovTab[ Depth ].movpiece != no_piece )
            TakeBackMove( &MovTab[ Depth ] );

        Found = false;

        if( MovTab[ Depth ].content == king ) {
            Depth++;
            FindNode();
            Depth--;
        }

        while( Depth < LibDepth )
            MakeMove( &MovTab[ Depth + 1 ] );

        if( Found )
            UseLib = 200;
        else {
            UseLib = MoveNo;
            LibNo = 0;
        }
    }
}


/*
 *  find an opening move from the library
 */
static void FindOpeningMove( void )
{
    const unsigned char weight[7] = {7, 10, 12, 13, 14, 15, 16};
    unsigned char cnt, r, p, countp;

    r = random( 16 ); /*  calculate weighted random number in 0..16  */
    p = 0;
    while( r >= weight[ p ] )
        p++;

    for( countp = 1; countp <= p; countp++ )  /* find corresponding node */
        NextLibNo( 1 );

    OpCount = Openings[ LibNo ] & 63;  /*  generate the move  */

    InitMovGen();
    for( cnt = 0; cnt <= OpCount; cnt++ )
        MainLine[0] = MovGen();

    MainLine[1] = ZeroMove;
    MainEvalu   = 0;
    MaxDepth    = 0;
    LegalMoves  = 0;

    Nodes = 0;
}

void OutputNode( double nodes )
{
    char buf[20];

    if( !Logging )
        return;

    sprintf( buf, "%12.1f", nodes );

    *OutputFile << buf;
}

void ReturnAnalysis()
{
    int  myx;
    char str[8];
    char buf[40];

    MovTab[0] = MainLine[0];   /*  copy the MainLine to HintLine  */
    for( myx = 1; myx <= MAXPLY; myx++ )
        HintLine[ myx - 1 ] = MainLine[ myx ];

    dep = MAXPLY;
    HintEvalu = MainEvalu;

    if( MovTab[0].movpiece == no_piece ) {
        HintLine[0] = ZeroMove;   /*  No possible move  */
        if( AutoPlay ) {
            NewGame();
            PrintBoard();
            StartMove();
        }
        return;
    }

//   FlashMove(&MovTab[Depth+1]);  /*  flash and perform the move  */
    DoSlideMove( MovTab[Depth+1] );
    EnterMove( &MovTab[Depth+1] );
    if( SoundOn )
        MessageBeep( 0 );
    StoreMoves();
    if( Logging && !AutoPlay ) {
        sprintf( buf, "%3d. ",( MoveNo+1 ) / 2 );
        *OutputFile << buf;
        strcpy( str, MoveStr( &MovTab[0] ) );
        if( ( PlayerMove.movpiece == no_piece ) && ( Opponent == white ) )
            sprintf( buf, "%8.8s ", str );
        else
            sprintf( buf, "%s%8.8s",MoveStr( &PlayerMove ), str );
        *OutputFile << buf;
        sprintf( buf, "    (%s)%9.2f%3d:%2d", MoveStr( &MainLine[1] ),
                 MainEvalu / 256.0, MaxDepth, LegalMoves );
        *OutputFile << buf;
        OutputNode( Nodes );
        sprintf( buf, "%8.1lf\n", getTotalTime( &ChessClock ) );
        *OutputFile << buf;
    }
    PlayerMove = ZeroMove;
    TInfo_SetTurnText( Player );
    if( AutoPlay ) {
        if( ( MoveNo >= 120 ) || ( FiftyMoveCnt() >= 100 ) ||
                ( Repetition( 0 ) >= 3 ) || ( MainEvalu <= -0x880 ) ) {
            NewGame();
            PrintBoard();
        }
        StartMove();
        return;
    }
    if( Level != easygame && !GameOver )
        ThinkAwhile();
}

/*
 *  Perform analysis in the opponents time of reflection.
 *  The program assumes that the opponent will perform the
 *  Hint move, and starts analysing on it for a counter move
 */
static void ThinkAwhile()
{
    set_arrow_cursor();

    main_window->set_main_menu();

    if( ( HintLine[0].movpiece == no_piece ) || MultiMove )
        return;

    Analysis = false;
    Opan     = true;

    AdjustMoves();                     /*  Setup surroundings as if the  */

    MovTab[Depth+1] = HintLine[0];     /*  Opponent had performed  */

    MakeMove( &MovTab[ Depth + 1 ] );  /*  The hint move  */
    StoreMoves();
    AdjustMoves();

    Depth = 0;          /*  analyse until something is entered from by */

    FindMove( MaxLevel ); /*  the user */

    Depth = -1;
    Opan = false;

    if( Analysis ) {       /*  If the Opponent did make the  */
        ReturnAnalysis();  /*  Hint move then go and perform the counter move  */
        return;
    }

    TakeBackMove( &MovTab[ Depth ] );  /*  restore the surroundings  */

    if( GotValidMove ) {
        AdjustMoves();
        EnterKeyMove();
        StoreMoves();

        main_window->set_think_menu();

        set_wait_cursor();
    }
    return;
}

void StartMove()
{
    MSG msg;

    set_wait_cursor();

    StartAnalysis();
    AdjustMoves();
    CalcLibNo();            /*  Try to find a move in the opening library  */

    Depth = 0;

    if( LibNo > 0 ) {
        OpeningLibMsg();
        InLibrary = true;
        FindOpeningMove();

        if( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) ) {
            if( msg.message == WM_COMMAND && msg.wParam == CM_STOP )
                return;
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
    } else {
        if( InLibrary ) {
            InLibrary = false;
            TInfo_SetMessageText( "" );
        }
        FindMove( MaxLevel );
    }

    Depth = -1;
    ReturnAnalysis();
}

void ProgramMove()
{
    do {
        GotValidMove = false;
        TInfo_SetTurnText( Player );
        StartMove();
    } while( GotValidMove );
}

void Talk()
{
    StartUp();
    NewGame();
}

bool Redo()
{
    EnterMove( &MovTab[Depth+1] );
    ClearHint();
    TInfo_ClearBestLine();
    TInfo_SetTurnText( Player );

    return Depth < -1;
}

bool Undo()
{
    RemoveMove( &MovTab[ Depth ] );

    ClearHint();
    TInfo_ClearBestLine();
    TInfo_SetTurnText( Player );

    if( MovTab[Depth].movpiece == no_piece )
        return false;  // Can't undo anymore

    return true;  // Can still undo
}

void QuitProgram()
{
    long color;

    if( Logging )
        OutputFile->close();

    writeINIFile();
}

void FindHintMove()
{
    /*  If hintline is no_piece the get the move from the opening library or perform a 1 - ply search  */
    if( HintLine[ 0 ].movpiece != no_piece )
        return;

    AdjustMoves();
    CalcLibNo();
    Depth       = 0;

    if( LibNo > 0 )
        FindOpeningMove();
    else {
        Analysis = 1;
        Opan = false;
        FindMove( 1 );
    }

    Depth       = -1;
    HintLine[0] = MainLine[0];
    HintLine[1] = ZeroMove;
    HintEvalu   = -MainEvalu;

    return;
}

void ShowHint()
{
    short dep = 0;

    buf[0] = '\0';
    TInfo_SetMessageText( "" );

    while( HintLine[dep].movpiece != no_piece ) {

        strcat( buf, MoveStr( &HintLine[dep] ) );
        strcat( buf, " " );
        TInfo_SetMessageText( buf );

        MakeMove( &HintLine[dep] );
        UpdateBoard();

        Wait( 6 );
        dep++;
    }

    while( dep > 0 ) {
        dep--;
        TakeBackMove( &HintLine[dep] );
    }

    UpdateBoard();
}

