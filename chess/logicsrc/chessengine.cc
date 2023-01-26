/*
 * Copyright 2017 Alwin Leerling <alwin@jambo>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */

#include "chessengine.h"
#include "chessgame.h"
#include "timeinputter.h"
#include "piecevalues.h"
#include "gameloader.h"

#include "fentranslator.h"

#include <chrono>
#include <thread>
#include <regex>

#include <iostream>

using namespace std;


ChessEngine::ChessEngine()
{
    is_arranging = false;

    colour_chooser = nullptr;
    time_inputter = nullptr;
    piece_values_object = nullptr;
    game_loader = nullptr;
}

ChessEngine::~ChessEngine()
{
}


void ChessEngine::new_game( )
{
    game_loader->new_file();

    game.initialise();


    info.turn = "white";
    info.black = "";
    info.time = "";
    info.level = "";
    info.value = "";
    info.nodes = "";
    info.n_sec = "";
    info.depth = "";
    info.bestline = "";

    multi_player = false;

    current_board.standard_opening_board();
}




bool ChessEngine::enter_move( STSquare start_square, STSquare end_square )
{
    static int test = 0;

    if( (++test) % 2 ) {

        current_board.move_piece( start_square, end_square );
        return true;

    } else {
        return false;
    }
    //return true;
}

STSquare ChessEngine::hint()
{
    STSquare square = make_square( 3, 3 );


    return square;
}

void ChessEngine::cancel_move( )
{
}

void ChessEngine::arranging_start()
{
    arrange_board.clear_all();
    is_arranging = true;
}

void ChessEngine::arranging_clear()
{
    arrange_board.clear_all();
}

void ChessEngine::arrange_add_piece( STSquare square, char piece )
{
    STPiece new_piece;

    new_piece.code = piece;
    new_piece.is_white = ( string("KQRBNP").find( piece ) != std::string::npos );
    new_piece.is_dragging = false;

    arrange_board.add_piece( square, new_piece );
}

void ChessEngine::arrange_remove_piece(STSquare square )
{
    arrange_board.remove_piece( square );
}

void ChessEngine::arrange_turn( eTurns new_turn )
{
    arrange_board.set_white_move( new_turn == TURNWHITE );
}

bool ChessEngine::arranging_end( bool canceled )
{
    if( canceled ) {
        is_arranging = false;
        return true;
    }

    if( !arrange_board.is_valid() )
        return false;

    current_board = arrange_board;

    is_arranging = false;
    return true;
}







std::map<STSquare,STPiece> ChessEngine::get_piece_positions( )
{
    return is_arranging ? arrange_board.get_pieces() : current_board.get_pieces();
}


bool ChessEngine::open_file( )
{
    return game_loader->load_file( game );
}

bool ChessEngine::save_file( )
{
    return game_loader->save_file( game );
}

bool ChessEngine::save_file_as( )
{
    return game_loader->save_file_as( game );
}

void ChessEngine::change_piece_values( )
{
    piece_values_object->choose_piece_values( );
}


bool ChessEngine::can_quit( )
{
    return false;
}


void ChessEngine::undo()
{

}

void ChessEngine::redo()
{

}

void ChessEngine::stop_thinking()
{

}

void ChessEngine::change_level( eLevels new_level )
{
    if( multi_player ) {
        info.level = "Two Player";
        return;
    }

    switch( new_level ) {
    case EASY:
        info.level = "Easy";
        break;

    case TIMED:
        if( !time_inputter->time_per_move( 120 ) )
            return;

        level_time = time_inputter->get_time();

        // model->set_level_info(      "%1.0f sec / move", level_time
        break;

    case TOTALTIME:
        if( !time_inputter->total_game_time( 60 ) )
            return;

        level_time = time_inputter->get_time();
        // model->set_level_info(      "%2.2f min / game", AverageTime )
        break;

    case INFINITE:
        info.level = "Infinite";
        break;

    case PLAYSEARCH:
        //sprintf( info_string, "Ply-Depth = %d", MaxLevel );
        break;

    case MATESEARCH:
        info.level = "MateSearch";
        break;

    case MATCHING:
        info.level = "Match users time";
        break;

    default:
        break;

    }

    if( new_level)
        level = new_level;

    // new level is (eLevels)level


    /*
    	model->set_level_info(
        if( MultiMove ) {
            TInfo->SetLevelText( "Two Player" );
            return;
        }

        char info_string[80];

        switch( Level ) {
        case normal       : sprintf( info_string, "%1.0f sec / move", AverageTime ); break;
        case fullgametime : sprintf( info_string, "%2.2f min / game", AverageTime ); break;
        case easygame     : strcpy( info_string, "Easy" );                           break;
        case infinite     : strcpy( info_string, "Infinite" );                       break;
        case plysearch    : sprintf( info_string, "Ply-Depth = %d", MaxLevel );      break;
        case matesearch   : strcpy( info_string, "MateSearch" );                     break;
        case matching     : strcpy( info_string, "Match users time" );               break;
        }

        TInfo->SetLevelText( info_string );
    */

}

bool ChessEngine::choose_colours()
{
    return colour_chooser->choose_colours( );

}


void ChessEngine::CalculatePawnTable()
{
    unsigned char PawnTable[2][8];

    /*  Calculate PawnTable (indicates which squares contain pawns )  */
    for( unsigned char rank = 0; rank < 8; rank++ ) {
        PawnTable[ 0 ][ rank ] = 0;
        PawnTable[ 1 ][ rank ] = 0;
    }

    //FENTranslator translator;

    //map<STSquare,STPiece> pieces = translator.from_FEN( current_state.piece_positions );
    map<STSquare,STPiece> pieces = current_board.get_pieces();

    for( map<STSquare,STPiece>::iterator it = pieces.begin(); it != pieces.end(); it++ ) {

        STSquare square = (*it).first;
        STPiece piece = (*it).second;

        if( piece.code == 'P' ) {		// white pawn
            PawnTable[0][square.rank] |= (1 << square.file);
        }
        else if( piece.code == 'p' ) {		// black pawn
            PawnTable[1][7- square.rank] |= (1 << square.file);
        }
    }

}

void ChessEngine::CalcMaterial()
{


    //FENTranslator translator;

    //map<STSquare,STPiece> pieces = translator.from_FEN( current_state.piece_positions );
    map<STSquare,STPiece> pieces = current_board.get_pieces();

    int material = 0;
    int totalMaterial = 0;

    for( map<STSquare,STPiece>::iterator it = pieces.begin(); it != pieces.end(); it++ ) {

        STPiece piece = (*it).second;



        material += piece_values_object->get_piece_value( piece.code );
        totalMaterial += abs( piece_values_object->get_piece_value( piece.code ) );

    }
}

#include <unistd.h>
void ChessEngine::calculate_move()
{
    unsigned int microsecond = 1000000;

    usleep( 5 * microsecond );
}

bool ChessEngine::toggle_multiplayer()
{
    multi_player = !multi_player;

    if( multi_player )
        info.level = "Two Player";

    return multi_player;
}

void ChessEngine::do_demo()
{

}

#if 0
/*  Calculate attack values of a square (importance of a square)  */
int ChessEngine::calc_attack_value( STSquare square )
{
    const unsigned char squarerankvalue[8]     = { 0, 0, 0, 0, 3, 6, 12, 12 };
    const char    distan[8] = { 3, 2, 1, 0, 0, 1, 2, 3 };

    return ( ( squarerankvalue[ square.rank ] * ( materiallevel + 8 ) ) >> 5 ) /*  Rank importance  */
           + max( 0, 8 - 3 * ( distan[ square.rank ] + distan[ square.file ] ) ); /*  centre importance */
}




int bit_count( unsigned char n )
{
    static const char split_lookup[] = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4 };

    return split_lookup[n&0xF] + split_lookup[n>>4];
}



#endif // 0

