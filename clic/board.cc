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

#include "ply.h"
#include "display.h"

Board::Board( std::string PiecePlacement )
{
	if( PiecePlacement.empty() )
		PiecePlacement = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";

	process_placement( PiecePlacement );
}

void Board::process_placement( std::string PiecePlacement )
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

std::string Board::piece_placement() const
{
	std::string placement;

    for( int rank=7; rank >= 0; --rank ) {

        int blanks = 0;

        for( int file=0; file < 8; ++file ) {

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

void Board::print_board( Display& display ) const
{
	display.print_board_header();

    for( int rank=8; rank; --rank ) {

		display.print_rank_header( rank );

		for( int file=0; file<8; ++file ) {

			int index = (rank - 1) * 8 + file;

			display.print_square( rank, file, position[index].get_type(), position[index].is_color( white ) );
		}

		display.print_rank_footer( rank );
    }

    display.print_board_footer();
}

void Board::update_board( Ply a_ply )
{
	if( a_ply.en_passant ) {
		Piece attacking_piece = position[a_ply.from];

		position[a_ply.to] = position[a_ply.from];
		position[a_ply.from] = Piece( Piece::none );

		a_ply.to += ( attacking_piece.is_color( white ) ? -8 : 8 );

		position[a_ply.to] = Piece();

	} else if( a_ply.promo_type != Piece::none ) {
		Piece promo_piece = position[a_ply.from];

		promo_piece.promote_pawn( Piece::eType(a_ply.promo_type) );

		position[a_ply.to] = promo_piece;
		position[a_ply.from] = Piece( Piece::none );

	} else if( a_ply.castling ) {

		// move the king
		position[a_ply.to] = position[a_ply.from];
		position[a_ply.from] = Piece( Piece::none );

		position[a_ply.to].moved();

		// adjust move structure to the rook
		if( a_ply.to > a_ply.from ) {	// king side
			a_ply.from += 3;
			a_ply.to -= 1;
		} else { 							// queen side
			a_ply.from -= 4;
			a_ply.to += 1;
		}

		// move the rook
		position[a_ply.to] = position[a_ply.from];
		position[a_ply.from] = Piece( Piece::none );

		position[a_ply.to].moved();

	} else {
		position[a_ply.to] = position[a_ply.from];
		position[a_ply.from] = Piece( Piece::none );

		position[a_ply.to].moved();
	}
}





std::vector<Ply> Board::generate_plys( eColor side, uint16_t ep_square ) const
{
	std::vector<Ply> plys;

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

		if( ! piece.is_color( side ) )
			continue;

		if( ! piece.is_of_type( Piece::pawn ) ) {

			for( unsigned int ray = 0; ray < piece.ray_directions(); ++ray ) {

				uint16_t target_square = square;

				do {
					target_square = mailbox[ mailbox64[target_square] + piece.get_ray_offset( ray ) ];	/* next square in this direction */

					if( target_square == (uint16_t)-1 )	/* outside of board */
						break;

					if( position[target_square].is_of_type( Piece::none ) )	/* quiet move */
						plys.push_back( Ply::standard_move( square, target_square, piece.get_type(), position[target_square].get_type(), Piece::none ) );
					else {
						if( ! position[target_square].is_color( side ) )
							plys.push_back( Ply::standard_move( square, target_square, piece.get_type(), position[target_square].get_type(), Piece::none ) );
						break;
					}

				} while( piece.is_sliding() );
			}
		}

		if( piece.is_of_type( Piece::pawn ) ) {

			uint16_t target_square = square;

			for( int counter = 0; counter < 2; ++counter ) {

				target_square = mailbox[ mailbox64[target_square] + (piece.is_color( white ) ? 10 : -10) ];	/* next square in this direction */

				if( ! position[target_square].is_of_type( Piece::none ) )	/* cannot_move */
					break;

				if( target_square / 8 == (piece.is_color( white ) ? 7: 0) ) {		// promotion ranks
					for( Piece::eType type = Piece::knight; type < Piece::Piece::king; type = Piece::eType(type + 1) )
						plys.push_back( Ply::standard_move( square, target_square, Piece::pawn, Piece::none, type ) );	// generate a promotion plys
					break;
				}

				plys.push_back( Ply::standard_move( square, target_square, Piece::pawn, Piece::none, Piece::none ) );

				if( piece.has_moved() )
					break;
			}

			// capture plys
			for( int counter = 0; counter < 2; ++counter ) {

				unsigned int offset = ((counter == 0) ? 9 : 11);

				if( piece.is_color( black ) )
					offset *= -1;

				target_square = mailbox[ mailbox64[square] + offset ];

				if( target_square == (uint16_t)-1 )				/* off the board... */
					continue;

				if( !position[target_square].is_of_type( Piece::none ) && ! position[target_square].is_color( side ) ) {	/* something is there, and its their piece */

					if( target_square / 8 == (piece.is_color( white ) ? 7: 0) ) {		// promotion ranks
						for( Piece::eType type = Piece::knight; type < Piece::king; type = Piece::eType(type + 1) )
							plys.push_back( Ply::standard_move( square, target_square, Piece::pawn, position[target_square].get_type(), type ) );	// generate a promotion plys
					} else
						plys.push_back( Ply::standard_move( square, target_square, Piece::pawn, position[target_square].get_type(), Piece::none ) );
				}

				if( ep_square == target_square )
					plys.push_back( Ply::ep_move( square, target_square ) );
			}

		}

		if( piece.is_of_type( Piece::king ) && !piece.has_moved() ) {

			// Check king side castle
			if(   !position[square + 3].has_moved()
				&& position[square + 3].is_of_type(Piece::rook)
				&& position[square + 1].is_of_type(Piece::none)
				&& position[square + 2].is_of_type(Piece::none)
			)
				plys.push_back( Ply::standard_move( square, uint16_t(square + 2), Piece::king, Piece::none, Piece::none ) );

			// Check queen side castle
			if(   !position[square - 4].has_moved()
				&& position[square - 4].is_of_type(Piece::rook)
				&& position[square - 1].is_of_type(Piece::none)
				&& position[square - 2].is_of_type(Piece::none)
				&& position[square - 3].is_of_type(Piece::none)
			)
				plys.push_back( Ply::standard_move( square, uint16_t(square - 2), Piece::king, Piece::none, Piece::none ) );
		}
	}

	return plys;
}

bool Board::illegal_move( Ply& a_ply ) const
{
	// Deduce the color of the player making this move
	eColor player = (position[a_ply.from].is_color( white ) ? white : black );
	Board test_board(*this);

	test_board.update_board( a_ply );

	std::vector<Ply> opponent_moves = test_board.generate_plys( eColor(player ^ 1), (uint16_t)-1 );

	return ( find_if( opponent_moves.begin(), opponent_moves.end(), [](Ply& opp_move) { return opp_move.king_capture; }) != opponent_moves.end() );

}

std::vector<Ply> Board::generate_legal_plys( eColor side, uint16_t ep_square ) const
{
	std::vector<Ply> moves = generate_plys( side, ep_square );	// grabs all pseudo legal moves

	moves.erase(remove_if(moves.begin(), moves.end(), [this](Ply& a_ply) { return this->illegal_move(a_ply); }), moves.end());	// filter out illegal moves

	return moves;
}
