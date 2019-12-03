/*
 * Copyright 2014 Alwin Leerling <alwin@jambo>
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

#include "pods.h"

#include <sstream>
#include <vector>

using namespace std;

bool operator<( const STSquare lhs, const STSquare rhs )
{
    return (lhs.rank * 8 + lhs.file ) < (rhs.rank * 8 + rhs.file );
}

STSquare make_square( int file_in, int rank_in )
{
    STSquare square;

    square.file = file_in;
    square.rank = rank_in;

    return square;
}

string move_to_LAN( STMove& move_in )
{
    if( move_in.castling != '-' )
        return ( (move_in.castling == 'k') || (move_in.castling == 'K') ) ? "0 - 0" : "0 - 0 - 0";

    stringstream ss;

    if( !move_in.pawn_promo && (move_in.piece_type != 'P') )
        ss << move_in.piece_type;

    char file = (move_in.orig_square.file + 'a');
    char rank = (move_in.orig_square.rank + '1');

    ss << file << rank;
    ss << ((move_in.is_capture) ? 'x' : '-');

    file = (move_in.new_square.file + 'a');
    rank = (move_in.new_square.rank + '1');

    ss << file << rank;

    if( move_in.pawn_promo ) ss << move_in.piece_type;
    if( move_in.attack_king ) ss << '+';
    if( move_in.check_mate ) ss << "++";

    return ss.str();
}


/** \brief
 *
 * \param layout string
 * \return int
 *
 */
int parse_ForsythEdwards( STGameState& game_state, string layout )
{
    vector<string> fields;
    string buffer;

    // Forsyth Edward notation contains 6 fields separated by a space. First crack the string into separate fields
    stringstream layout_stream( layout );
    while( getline( layout_stream, buffer, ' ' ) )
        fields.push_back( buffer );

    // check we have enough fields
    if( fields.size() != 6 )
        return -1;

    // Initialise the game state
    game_state.white_can_castle_kingside =
    game_state.white_can_castle_queenside =
    game_state.black_can_castle_kingside =
    game_state.black_can_castle_queenside = false;
    game_state.en_passant_target.file =
    game_state.en_passant_target.rank = -1;

    // Extract information
    game_state.piece_positions = fields[0];
    game_state.is_white_move = (fields[1] == "w" );

    if( fields[2] != "-" ) {
        for( char& ch : fields[2] ) {
            switch( ch ) {
                case 'K': game_state.white_can_castle_kingside = true;
                case 'Q': game_state.white_can_castle_queenside = true;
                case 'k': game_state.black_can_castle_kingside = true;
                case 'q': game_state.black_can_castle_queenside = true;
            }
        }
    }

    if( fields[3] != "-" ) {
        game_state.en_passant_target.file = fields[3].at(0) - 'a';
        game_state.en_passant_target.rank = fields[3].at(1) - '0';
    }

    game_state.halfmove_clock = std::stoi( fields[4] );
    game_state.fullmove_number = std::stoi( fields[5] );

    return 0;
}

/** \brief
 *
 * \return string
 *
 */
string gamestate_to_ForsythEdwards( STGameState& game_state )
{
    string castle;
    string result;

    stringstream ss( result );
    stringstream ss_castl( castle );

    ss << game_state.piece_positions;
    ss << " ";              // field separator

    ss << (game_state.is_white_move ? 'w' : 'b');
    ss << " ";              // field separator

    if( game_state.white_can_castle_kingside  ) ss_castl << 'K';
    if( game_state.white_can_castle_queenside ) ss_castl << 'Q';
    if( game_state.black_can_castle_kingside  ) ss_castl << 'k';
    if( game_state.black_can_castle_queenside ) ss_castl << 'q';

    ss << ( castle.empty() ? "-" : castle );
    ss << " ";              // field separator

    if( game_state.en_passant_target.file != -1 ) {
        ss << ( 'a' +  game_state.en_passant_target.file ) << ( '0' +  game_state.en_passant_target.rank );
    } else
        ss << '-';

    ss << " ";              // field separator

    ss << game_state.halfmove_clock;
    ss << " ";              // field separator

    ss << game_state.fullmove_number;
    ss << " ";              // field separator

    return result;
}

STGameState make_game_state( std::string layout )
{
    STGameState game_state;

    parse_ForsythEdwards( game_state, layout );

    return game_state;
};

