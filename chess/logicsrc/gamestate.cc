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

#include "gamestate.h"

#include <algorithm>
#include <limits>
#include <sstream>

using namespace std;

GameState::GameState( std::string FEN )
{
	if( FEN.empty() )
		FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    vector<string> fields;
    string buffer;

    // Forsyth Edward notation contains 6 fields separated by a space. First crack the string into separate fields
    stringstream layout_stream( FEN );
	while( getline( layout_stream, buffer, ' ' ) )
		fields.push_back( buffer );

	process_placement( fields[0] );
	process_active_color( fields[1] );
	process_castling( fields[2] );
	process_ep( fields[3] );
	process_halfmoves( fields[4] );
	process_fullmoves( fields[5] );
}

std::string GameState::FEN() const
{
	return piece_placement() + " " + active_color() + " "  + castle_rights() + " "
					+ ep_square() + " " + halvemoves() + " " + fullmoves();
}

void GameState::process_placement( std::string PiecePlacement )
{
	unsigned int rank = 7;
	unsigned int file = 0;

	position.fill( Piece( Piece::none ) );

    for( char& code : PiecePlacement ) {

        if( (code > '0') && (code < '9') ) {       // Character can either be a number (squares to skip)
           file += (code - '0' );
        } else if( code == '/' ) {                      // ... or an end of rank indicator
            --rank;
            file = 0;
        } else {								  // or an piece designator
        	position[rank * 8 + file] = Piece( code );
            ++file;
        }
    }
}

std::string GameState::piece_placement() const
{
	string placement;

    for( int rank=7; rank >= 0; --rank ) {

        int blanks = 0;

        for( int file=0; file < 8; ++file ) {

			if( ! position[rank * 8 + file].is_of_type( Piece::none ) ) {
                if( blanks ) {
					placement += to_string( blanks );
                    blanks = 0;
                }
                placement += position[rank * 8 + file].get_code();
			} else
                ++blanks;
        }

        if( blanks )
			placement += to_string( blanks );

        if( rank )
            placement += "/";
    }

    return placement;
}

void GameState::process_active_color( std::string active_color )
{
    current_player = (active_color == "w" ) ? eColor::white : eColor::black;
}

std::string GameState::active_color() const
{
	return (current_player == eColor::white) ? "w" : "b";
}

void GameState::process_castling( std::string castle_rights )
{
	can_castle_kingside = map<eColor,bool> { { eColor::white, false}, {eColor::black, false} };
	can_castle_queenside = map<eColor,bool> { { eColor::white, false}, {eColor::black, false} };

    if( castle_rights == "-" )
		return;

	for( char& ch : castle_rights ) {
		switch( ch ) {
		case 'K': can_castle_kingside[eColor::white] = true;
		case 'Q': can_castle_queenside[eColor::white] = true;
		case 'k': can_castle_kingside[eColor::black] = true;
		case 'q': can_castle_queenside[eColor::black] = true;
		}
	}

}

std::string GameState::castle_rights() const
{
	string result;

	if( can_castle_kingside.at(eColor::white) ) result += 'K';
	if( can_castle_queenside.at(eColor::white) ) result += 'Q';
	if( can_castle_kingside.at(eColor::black) ) result += 'k';
	if( can_castle_queenside.at(eColor::black) ) result += 'q';

	return result.empty() ? "-" : result;
}

void GameState::process_ep( std::string ep_input )
{
	if( ep_input == "-" ) {
		en_passant_target =  (uint16_t)-1;
		return;
	}

	en_passant_target  = (ep_input[0] - 'a');
	en_passant_target += (ep_input[1] - '1') * 8;
}

std::string GameState::ep_square() const
{
	if( en_passant_target == (uint16_t)-1)
		return "-";

	std::string SAN;

	SAN.push_back( (en_passant_target % 8) + 'a' );
	SAN.push_back( (en_passant_target / 8) + '1' );

	return SAN;
}

void GameState::process_halfmoves( std::string halvemoves )
{
    halfmove_clock = std::stoi( halvemoves );
}

std::string GameState::halvemoves() const
{
	return to_string( halfmove_clock );
}

void GameState::process_fullmoves( std::string fullmoves )
{
    fullmove_number = std::stoi( fullmoves );
}

std::string GameState::fullmoves() const
{
	return to_string( fullmove_number );
}








GameState GameState::set_piece( uint16_t square, char code )
{
	position[ square ] = ( code == ' ' ) ? Piece(Piece::none) : Piece( code );
	return *this;
}

GameState GameState::set_active_color( eColor color )
{
	current_player = color;
	return *this;
}

GameState GameState::set_castle_rights( std::string castle_rights )
{
	can_castle_kingside[eColor::white] =
	can_castle_queenside[eColor::white] =
	can_castle_kingside[eColor::black] =
	can_castle_queenside[eColor::black] = false;

	for( char& ch : castle_rights ) {
		switch( ch ) {
		case 'K': can_castle_kingside[eColor::white] = true;
		case 'Q': can_castle_queenside[eColor::white] = true;
		case 'k': can_castle_kingside[eColor::black] = true;
		case 'q': can_castle_queenside[eColor::black] = true;
		}
	}
	return *this;
}

GameState GameState::set_ep_square( uint16_t square )
{
	en_passant_target = square;
	return *this;
}

GameState GameState::set_halvemoves( uint16_t number )
{
	halfmove_clock = number;
	return *this;
}

GameState GameState::set_fullmoves( uint16_t number )
{
	fullmove_number = number;
	return *this;
}

bool GameState::is_valid() const
{
	struct Counts {
		int KingCount;
		int TotalCount;
	};
	map<eColor,Counts> counts = { {eColor::white, { 0, 0} }, {eColor::black, {0,0} } };

    for( auto it = position.begin(); it != position.end(); it++ ) {

		if( (*it).is_of_type( Piece::none) )
			continue;

		Counts& cc = counts.at( (*it).get_color() );

		++cc.TotalCount;

		if( (*it).is_of_type( Piece::king ) )
			++cc.KingCount;
    }

    return ( counts.at(eColor::white).TotalCount <= 16 ) && ( counts.at(eColor::white).KingCount == 1 )
		&& ( counts.at(eColor::black).TotalCount <= 16 ) && ( counts.at(eColor::black).KingCount == 1 );
}










GameState GameState::make( Ply a_ply ) const
{
	GameState new_state( *this );

	uint16_t to = a_ply.square_to();
	uint16_t from = a_ply.square_from();

	if( a_ply.is_ep_capture() ) {

		new_state.position[to] = position[from];
		new_state.position[from] = Piece( Piece::none );

		new_state.position[a_ply.get_ep_captured_square()] = Piece( Piece::none );

	} else if( ! a_ply.check_promo_match( Piece::none ) ) {

		new_state.position[to] = position[from].make_promo_piece( a_ply.get_promo_type() );
		new_state.position[from] = Piece( Piece::none );

	} else if( a_ply.is_castling() ) {

		// move the king
		new_state.position[to] = position[from];
		new_state.position[from] = Piece( Piece::none );

		// reuse for rook
		to = a_ply.get_castling_rook_square_to();
		from = a_ply.get_castling_rook_square_from();

		// move the rook
		new_state.position[to] = position[from];
		new_state.position[from] = Piece( Piece::none );

		new_state.can_castle_kingside[current_player] = false;
		new_state.can_castle_queenside[current_player] = false;

	} else {
		new_state.position[to] = position[from];
		new_state.position[from] = Piece( Piece::none );

		if( current_player == eColor::white ) {
			if( (from == 0) || (from == 4) ) new_state.can_castle_queenside[eColor::white] = false;
			if( (from == 7) || (from == 4) ) new_state.can_castle_kingside[eColor::white]  = false;
		} else {
			if( (from == (0^56)) || (from == (4^56)) ) new_state.can_castle_queenside[eColor::black] = false;
			if( (from == (7^56)) || (from == (4^56)) ) new_state.can_castle_kingside[eColor::black]  = false;
		}
	}

	new_state.current_player = (current_player == eColor::white) ? eColor::black : eColor::white;
	new_state.en_passant_target = a_ply.get_ep_square();

    new_state.halfmove_clock = a_ply.halfclock_needs_reset() ? 0 : halfmove_clock + 1;
    if( current_player == eColor::black )
		new_state.fullmove_number = fullmove_number + 1;             // The number of the move, start at one increment after black move

	return new_state;
}

std::vector<Ply> GameState::generate_plys() const
{
	vector<Ply> plys;

	static int mailbox[120] = {
		 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		 -1,  0,  1,  2,  3,  4,  5,  6,  7, -1,
		 -1,  8,  9, 10, 11, 12, 13, 14, 15, -1,
		 -1, 16, 17, 18, 19, 20, 21, 22, 23, -1,
		 -1, 24, 25, 26, 27, 28, 29, 30, 31, -1,
		 -1, 32, 33, 34, 35, 36, 37, 38, 39, -1,
		 -1, 40, 41, 42, 43, 44, 45, 46, 47, -1,
		 -1, 48, 49, 50, 51, 52, 53, 54, 55, -1,
		 -1, 56, 57, 58, 59, 60, 61, 62, 63, -1,
		 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
	};

	static int mailbox64[64] = {
		21, 22, 23, 24, 25, 26, 27, 28,
		31, 32, 33, 34, 35, 36, 37, 38,
		41, 42, 43, 44, 45, 46, 47, 48,
		51, 52, 53, 54, 55, 56, 57, 58,
		61, 62, 63, 64, 65, 66, 67, 68,
		71, 72, 73, 74, 75, 76, 77, 78,
		81, 82, 83, 84, 85, 86, 87, 88,
		91, 92, 93, 94, 95, 96, 97, 98
	};

	for( uint16_t square = 0; square < 64; ++square) { /* loop over all squares (no piece list) */

		Piece piece = position[square];

		if( ! piece.is_color( current_player ) || piece.is_of_type( Piece::none ) )
			continue;

		if( ! piece.is_of_type( Piece::pawn ) ) {

			for( unsigned int ray = 0; ray < piece.ray_directions(); ++ray ) {

				uint16_t target_square = square;

				do {
					target_square = mailbox[ mailbox64[target_square] + piece.get_ray_offset( ray ) ];	/* next square in this direction */

					if( target_square == (uint16_t)-1 )	/* outside of board */
						break;

					if( position[target_square].is_of_type( Piece::none ) )	/* quiet move */
						plys.push_back( Ply::make_quiet_ply( square, target_square, piece.get_type() ) );
					else {
						if( ! position[target_square].is_color( current_player ) )
							plys.push_back( Ply::make_capture_ply(square, target_square, piece.get_type(), position[target_square].get_type()) );
						break;
					}

				} while( piece.is_sliding() );
			}
		}

		if( piece.is_of_type( Piece::pawn ) ) {

			uint16_t target_square = square;

			for( int counter = 0; counter < 2; ++counter ) {

				target_square = mailbox[ mailbox64[target_square] + (piece.is_color( eColor::white ) ? 10 : -10) ];	/* next square in this direction */

				if( ! position[target_square].is_of_type( Piece::none ) )	/* cannot_move */
					break;

				if( target_square / 8 == (piece.is_color( eColor::white ) ? 7: 0) ) {		// promotion ranks
					for( Piece::eType type = Piece::knight; type < Piece::Piece::king; type = Piece::eType(type + 1) )
						plys.push_back( Ply::make_promotion_ply( square, target_square, type ) );	// generate a promotion plys
					break;
				}

				plys.push_back( Ply::make_quiet_ply( square, target_square, Piece::pawn ) );

				if( square / 8 != (piece.is_color( eColor::white ) ? 1 : 6) )		// start ranks
					break;
			}

			// capture plys
			for( int counter = 0; counter < 2; ++counter ) {

				unsigned int offset = ((counter == 0) ? 9 : 11);

				if( piece.is_color( eColor::black ) )
					offset *= -1;

				target_square = mailbox[ mailbox64[square] + offset ];

				if( target_square == (uint16_t)-1 )				/* off the board... */
					continue;

				if( !position[target_square].is_of_type( Piece::none ) && ! position[target_square].is_color( current_player ) ) {	/* something is there, and its their piece */

					if( target_square / 8 == (piece.is_color( eColor::white ) ? 7: 0) ) {		// promotion ranks
						for( Piece::eType type = Piece::knight; type < Piece::king; type = Piece::eType(type + 1) )
							plys.push_back( Ply::make_promotion_ply( square, target_square, type, position[target_square].get_type() ) );	// generate promotion plys
					} else
						plys.push_back( Ply::make_capture_ply( square, target_square, Piece::pawn, position[target_square].get_type() ) );
				}

				if( en_passant_target == target_square )
					plys.push_back( Ply::make_ep_ply( square, target_square ) );
			}

		}

		if( piece.is_of_type( Piece::king ) ) {

			if( can_castle_kingside.at( current_player )
				&& position[square + 1].is_of_type(Piece::none)
				&& position[square + 2].is_of_type(Piece::none)
			)
				plys.push_back( Ply::make_castle_ply( square, uint16_t(square + 2) ) );

			if( can_castle_queenside.at( current_player )
				&& position[square - 1].is_of_type(Piece::none)
				&& position[square - 2].is_of_type(Piece::none)
				&& position[square - 3].is_of_type(Piece::none)
			)
				plys.push_back( Ply::make_castle_ply( square, uint16_t(square - 2) ) );
		}
	}

	return plys;
}

std::vector<Ply> GameState::generate_legal_plys() const
{
	vector<Ply> moves = generate_plys();	// grabs all pseudo legal moves

	// Filter out illegal moves
	auto illegal_move = [this](Ply& a_ply)
	{
		vector<Ply> opponent_moves = make( a_ply ).generate_plys();
		return ( find_if( opponent_moves.begin(), opponent_moves.end(), [](Ply& opp_move) { return opp_move.is_kingcapture(); }) != opponent_moves.end() );
	};
	moves.erase( remove_if(moves.begin(), moves.end(), illegal_move ), moves.end() );

	// Mark check (checkmate) plys
	auto mark_check = [this](Ply& a_ply)
	{
		GameState test = make( a_ply );
		test.current_player = current_player;
		vector<Ply> plys = test.generate_plys();
		if( find_if( plys.begin(), plys.end(), [](Ply& ply) { return ply.is_kingcapture(); }) != plys.end() )
			a_ply.set_check();
	};
	for_each( moves.begin(), moves.end(), mark_check );

	return moves;
}

int GameState::evaluate() const
{
	map<eColor,int> score = { {eColor::white, 0}, {eColor::black, 0} };

	for( uint16_t square = 0; square < 64; ++square)
		if( ! position[square].is_of_type( Piece::none ) )
			score.at( position[square].get_color() ) += position[square].get_score( square );

	return score.at(eColor::white) - score.at(eColor::black);
}
