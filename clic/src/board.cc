/*
 * Copyright 2023 Alwin Leerling <dna.leerling@gmail.com>
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
 */

#include "board.h"

#include <algorithm>
#include <limits>
#include <unordered_map>

#include "ply.h"

Board::Board( std::string PiecePlacement )
{
	side_to_move = eColor::white;
	ep_square = (uint8_t)-1;

	if( PiecePlacement.empty() )
		PiecePlacement = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";

	process_placement( PiecePlacement );
}

void Board::process_placement( std::string PiecePlacement )
{
	unsigned int rank = 7;
	unsigned int file = 0;

	position.fill( Piece( Piece::none ) );

	for( char &code : PiecePlacement ) {

		if( ( code > '0' ) && ( code < '9' ) ) { // Character can either be a number (squares to skip)
			file += ( code - '0' );
		} else if( code == '/' ) { // ... or an end of rank indicator
			--rank;
			file = 0;
		} else { // or an piece designator
			position[rank * 8 + file] = Piece( code );
			++file;
		}
	}
}

std::string Board::piece_placement() const
{
	std::string placement;

	for( int rank = 7; rank >= 0; --rank ) {

		int blanks = 0;

		for( int file = 0; file < 8; ++file ) {

			if( position[rank * 8 + file] != Piece( Piece::none ) ) {
				if( blanks ) {
					placement += std::to_string( blanks );
					blanks = 0;
				}
				placement += position[rank * 8 + file].get_code();
			} else
				++blanks;
		}

		if( blanks )
			placement += std::to_string( blanks );

		if( rank )
			placement += "/";
	}

	return placement;
}

Board Board::make( Ply a_ply ) const
{
	Board new_board( *this );

	new_board.update_board( a_ply );

	return new_board;
}

void Board::update_board( Ply a_ply )
{
	uint16_t to = a_ply.square_to();
	uint16_t from = a_ply.square_from();

	if( a_ply.is_ep_capture() ) {

		position[to] = position[from];
		position[from] = Piece( Piece::none );

		position[ep_square] = Piece( Piece::none );

	} else if( !a_ply.check_promo_match( Piece::none ) ) {

		position[to] = position[from].make_promo_piece( a_ply.get_promo_type() );
		position[from] = Piece( Piece::none );

	} else if( a_ply.is_castling() ) {

		// move the king
		position[to] = position[from];
		position[from] = Piece( Piece::none );

		position[to].moved();

		// reuse for rook
		to = a_ply.get_castling_rook_square_to();
		from = a_ply.get_castling_rook_square_from();

		// move the rook
		position[to] = position[from];
		position[from] = Piece( Piece::none );

		position[to].moved();

	} else {
		position[to] = position[from];
		position[from] = Piece( Piece::none );

		position[to].moved();
	}

	ep_square = a_ply.get_ep_square();
	side_to_move = !side_to_move;
}

std::vector<Ply> Board::generate_plys() const
{
	std::vector<Ply> plys;

	static int mailbox[120] = {
		// clang-format off
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
		// clang-format on
	};

	static int mailbox64[64] = {
		// clang-format off
		21, 22, 23, 24, 25, 26, 27, 28,
		31, 32, 33, 34, 35, 36, 37, 38,
		41, 42, 43, 44, 45, 46, 47, 48,
		51, 52, 53, 54, 55, 56, 57, 58,
		61, 62, 63, 64, 65, 66, 67, 68,
		71, 72, 73, 74, 75, 76, 77, 78,
		81, 82, 83, 84, 85, 86, 87, 88,
		91, 92, 93, 94, 95, 96, 97, 98
		// clang-format on
	};

	static int mailbox_offset[7][8] = {
		// clang-format off
		{   0,   0,  0,  0, 0,  0,  0,  0 }, /* none */
		{   0,   0,  0,  0, 0,  0,  0,  0 }, /* pawn */
		{ -21, -19,-12, -8, 8, 12, 19, 21 }, /* knight */
		{ -11,  -9,  9, 11, 0,  0,  0,  0 }, /* bishop */
		{ -10,  -1,  1, 10, 0,  0,  0,  0 }, /* rook */
		{ -11, -10, -9, -1, 1,  9, 10, 11 }, /* queen */
		{ -11, -10, -9, -1, 1,  9, 10, 11 }  /* king */
		// clang-format on
	};

	for( uint16_t square = 0; square < 64; ++square ) { /* loop over all squares (no piece list) */

		Piece piece = position[square];

		if( !piece.is_color( side_to_move ) || piece.is_of_type( Piece::none ) )
			continue;

		if( !piece.is_of_type( Piece::pawn ) ) {

			int *offset = &mailbox_offset[piece.get_type()][0];

			for( unsigned int ray = 0; ( ray < 8 ) && ( offset[ray] != 0 ); ++ray ) {

				for( uint16_t target_square = mailbox[mailbox64[square] + offset[ray]]; target_square != (uint16_t)-1;
					 target_square = mailbox[mailbox64[target_square] + offset[ray]] ) {

					if( position[target_square].is_of_type( Piece::none ) ) { /* quiet move */
						plys.push_back( Ply::create( piece, square, target_square ).build() );
					} else {
						if( !position[target_square].is_color( side_to_move ) ) { /* capture move */
							plys.push_back( Ply::create( piece, square, target_square )
												.setCaptureType( position[target_square].get_type() )
												.build() );
						}
						break; // cannot slide any further
					}

					if( !piece.is_sliding() )
						break;
				}
			}
		}

		if( piece.is_of_type( Piece::pawn ) ) {

			/* quiet moves */
			int offset = ( piece.is_color( eColor::white ) ? 10 : -10 );

			uint16_t target_square = mailbox[mailbox64[square] + offset]; /* next square in this direction */

			if( position[target_square].is_of_type( Piece::none ) ) {

				if( target_square / 8 == ( piece.is_color( eColor::white ) ? 7 : 0 ) ) { // promotion ranks
					for( Piece::eType type = Piece::knight; type < Piece::Piece::king; type = Piece::eType( type + 1 ) )
						plys.push_back( Ply::create( piece, square, target_square ).setPromoType( type ).build() );

				} else {

					plys.push_back( Ply::create( piece, square, target_square ).build() );

					if( square / 8 == ( piece.is_color( eColor::white ) ? 1 : 6 ) ) { // first pawn rank

						target_square = mailbox[mailbox64[square] + 2 * offset]; // try double move

						if( position[target_square].is_of_type( Piece::none ) )
							plys.push_back( Ply::create( piece, square, target_square ).build() );
					}
				}
			}

			/* capture moves */
			for( int offset : {9, 11} ) {

				if( piece.is_color( eColor::black ) )
					offset *= -1;

				target_square = mailbox[mailbox64[square] + offset];

				if( target_square == (uint16_t)-1 ) /* off the board... */
					continue;

				if( !position[target_square].is_of_type( Piece::none ) &&
					!position[target_square].is_color( side_to_move ) ) { /* something is there, and its their piece */

					if( target_square / 8 == ( piece.is_color( eColor::white ) ? 7 : 0 ) ) { // promotion ranks
						for( Piece::eType type = Piece::knight; type < Piece::king; type = Piece::eType( type + 1 ) )
							plys.push_back( Ply::create( piece, square, target_square )
												.setCaptureType( position[target_square].get_type() )
												.setPromoType( type )
												.build() );
					} else
						plys.push_back( Ply::create( piece, square, target_square )
											.setCaptureType( position[target_square].get_type() )
											.build() );
				}

				if( ep_square == target_square )
					plys.push_back( Ply::create( piece, square, target_square ).setEPMove().build() );
			}
		}

		if( piece.is_of_type( Piece::king ) && !piece.has_moved() ) {

			// Check king side castle
			if( !position[square + 3].has_moved() && position[square + 1].is_of_type( Piece::none ) &&
				position[square + 2].is_of_type( Piece::none ) )
				plys.push_back( Ply::create( piece, square, uint16_t( square + 2 ) ).build() );

			// Check queen side castle
			if( !position[square - 4].has_moved() && position[square - 1].is_of_type( Piece::none ) &&
				position[square - 2].is_of_type( Piece::none ) && position[square - 3].is_of_type( Piece::none ) )
				plys.push_back( Ply::create( piece, square, uint16_t( square - 2 ) ).build() );
		}
	}

	return plys;
}

std::vector<Ply> Board::generate_legal_plys() const
{
	auto king_capture = [](Ply& move) { return move.is_kingcapture(); };

	std::vector<Ply> moves = generate_plys(); // grabs all pseudo legal moves

	auto illegal_move = [this, king_capture](Ply& a_ply)
	{
		Board test = make( a_ply );
		std::vector<Ply> opponent_moves = test.generate_plys();
		auto it = find_if( opponent_moves.begin(), opponent_moves.end(), king_capture );

		return (  it != opponent_moves.end() );
	};
	moves.erase( remove_if(moves.begin(), moves.end(), illegal_move ), moves.end() );

	// Mark check (checkmate) plys
	auto mark_check = [this, king_capture](Ply& a_ply)
	{
		Board test = make( a_ply );
		test.side_to_move = ! test.side_to_move;
		std::vector<Ply> plys = test.generate_plys();
		auto it = find_if( plys.begin(), plys.end(), king_capture );
		if( it != plys.end() )
			a_ply.set_check();
	};
	for_each( moves.begin(), moves.end(), mark_check );

	return moves;
}

int Board::evaluate() const
{
	std::unordered_map<eColor, int> score = { {eColor::white, 0}, {eColor::black, 0 } };

	for( uint16_t square = 0; square < 64; ++square )
		if( !position[square].is_of_type( Piece::none ) ) {
			eColor index = position[square].is_color( eColor::white ) ? eColor::white : eColor::black;
			score[index] += position[square].get_score( square );
		}

	return score[eColor::white] - score[eColor::black];
}

/*
 *	Alpha is the best value that the maximizer currently can guarantee at that level or above.
 *	Beta is the best value that the minimizer currently can guarantee at that level or below.
 */
int Board::alpha_beta( int alpha, int beta, int depth_left ) const
{
	if( !depth_left )
		return evaluate();

	std::vector<Ply> plys = generate_legal_plys(); // grabs all legal moves

	int best_score;

	if( side_to_move == eColor::white ) { // maximiser
		best_score = std::numeric_limits<int>::min();
		for( Ply &ply : plys ) {
			best_score = std::max( best_score, make( ply ).alpha_beta( alpha, beta, depth_left - 1 ) );
			alpha = std::max( alpha, best_score );

			if( beta <= alpha )
				break;
		}

	} else { // minimiser
		best_score = std::numeric_limits<int>::max();
		for( Ply &ply : plys ) {
			best_score = std::min( best_score, make( ply ).alpha_beta( alpha, beta, depth_left - 1 ) );
			beta = std::min( beta, best_score );

			if( beta <= alpha )
				break;
		}
	}

	return best_score;
}

int Board::evaluate_ply( const Ply &ply, int depth ) const
{
	return make( ply ).alpha_beta( std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), depth );
}