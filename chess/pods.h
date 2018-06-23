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

#ifndef PODS_H_INCLUDED
#define PODS_H_INCLUDED

#include <string>

/**-----------------------------------------------------------------------------
 * \brief Struct to hold the colours we need in this application
 *
 * Note: each string holds a colour in the notation: rgb(r,g,b) where each colour
 * component is in the range 0..255
 */
struct STColours
{
    std::string bg;
    std::string fg;
    std::string white;
    std::string black;
};

/**-----------------------------------------------------------------------------
 * \brief Struct to hold
 *
 */
struct STInput
{
    std::string title;
    std::string prompt;
    std::string value;
};

struct STInfo
{
    std::string turn;
    std::string white;
    std::string black;
    std::string time;
    std::string level;
    std::string value;
    std::string nodes;
    std::string n_sec;
    std::string depth;
    std::string bestline;
};

struct STPieceValues
{
    int QueenValue;
    int RookValue;
    int BishopValue;
    int KnightValue;
    int PawnValue;
};



/**-----------------------------------------------------------------------------
 * \brief Struct to hold
 *
 */
struct STSquare
{
    int file;
    int rank;
};

/**-----------------------------------------------------------------------------
 * \brief
 */
struct STMove
{
    bool is_white;

    char castling;
    char piece_type;

    bool is_capture;
    bool pawn_promo;
    bool attack_king;
    bool check_mate;

    STSquare orig_square;      // first is file, second is rank
    STSquare new_square;      // first is file, second is rank
};

/**-----------------------------------------------------------------------------
 * \brief Snapshot of a game moment
 *
 * All state held in this struct are exactly the same as a Forsyth Edwards string
 * but more easily accessible in a programming construct.
 */
struct STGameState
{
    std::string piece_positions;    // Piece placement in FEN.
    bool is_white_move;             // is it whites next move?
    bool white_can_castle_kingside;
    bool white_can_castle_queenside;
    bool black_can_castle_kingside;
    bool black_can_castle_queenside;
    STSquare ep_square;
    int halfmove_clock;             // Number of half moves since last capture or pawn advance
    int fullmove_number;             // The number of the move, start at one increment after black move
};

// Utility functions
bool operator<( const STSquare lhs, const STSquare rhs );
STSquare make_square( int file_in, int rank_in );

std::string move_to_LAN( STMove& move_in );

int parse_ForsythEdwards( STGameState& game_state, std::string layout);
std::string gamestate_to_ForsythEdwards( STGameState& game_state );
STGameState make_game_state( std::string layout );

bool operator==( STColours& lhs, STColours& rhs );


#endif // PODS_H_INCLUDED
