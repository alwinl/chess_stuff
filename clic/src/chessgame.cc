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

#include "chessgame.h"

#include <algorithm>
#include <chrono>
#include <thread>

int main( int argc, char *argv[] )
{
	ChessGame game;

	if( game.setup() )
		return 0;

	for( ;; )
		if( game.game_loop() )
			break;

	return 0;
}

bool ChessGame::setup()
{
	unsigned int gametype = -1;

	while( gametype == (unsigned int)-1 ) {
		disp.new_game_menu();
		gametype = disp.select_gametype();
	}

	is_human[0] = ( ( gametype == 1 ) || ( gametype == 4 ) );
	is_human[1] = ( ( gametype == 2 ) || ( gametype == 4 ) );

	return ( gametype == 5 );
}

bool ChessGame::game_loop()
{
	bool quit;

	print_board();

	std::vector<Ply> plys = board.generate_legal_plys();
	if( plys.empty() ) // checkmate
		return true;

	disp.print_total_possible_moves( plys.size() );

	if( is_human[current_player] )
		quit = human_move( current_player, plys );
	else
		quit = ai_move( current_player, plys );

	current_player = eColor( current_player ^ 1 );

	return quit;
}

void ChessGame::print_board()
{
	disp.print_board_header();

	for( int rank = 8; rank; --rank ) {

		disp.print_rank_header( rank );

		for( int file = 0; file < 8; ++file ) {

			int index = ( rank - 1 ) * 8 + file;

			Piece piece = board.get_piece( index );

			disp.print_square( rank, file, piece.get_type(), piece.is_color( white ) );
		}

		disp.print_rank_footer( rank );
	}

	disp.print_board_footer();
}

bool ChessGame::human_move( eColor player, std::vector<Ply> plys )
{
	unsigned int square_from;
	unsigned int square_to;

	for( ;; ) {
		disp.print_input_header( player == white );

		if( ( square_from = disp.get_square() ) == uint16_t( -1 ) )
			return true;

		if( square_from == 64 )
			continue;

		disp.print_bar();

		if( ( square_to = disp.get_square() ) == uint16_t( -1 ) )
			return true;

		if( square_to == 64 )
			continue;

		auto ply_it = std::find_if( plys.begin(), plys.end(), [square_from, square_to]( Ply ply ) {
			return ply.check_square_match( square_from, square_to );
		} );
		if( ply_it == plys.end() ) {
			disp.print_invalid_move();
			continue;
		}

		// If it is not a promotion, we have all relevant information. Process the move
		if( ( *ply_it ).check_promo_match( Piece::none ) ) {
			apply_move( *ply_it );
			return false;
		}

		// we need to find all possible promotions and ask the player to select one
		unsigned int index;

		for( ;; ) {
			disp.promo_menu( false );

			if( ( index = disp.select_promo_type() ) != (unsigned int)-1 ) {
				disp.promo_menu( true );
				break;
			}
		}

		Piece::eType promo_type =
			( ( Piece::eType[] ){ Piece::knight, Piece::bishop, Piece::rook, Piece::queen } )[index];

		ply_it = std::find_if( plys.begin(), plys.end(), [promo_type]( Ply the_ply ) {
			return the_ply.check_promo_match( promo_type );
		} ); // should always find one

		apply_move( *ply_it );

		return false;
	}
}

bool ChessGame::ai_move( eColor player, std::vector<Ply> plys )
{
	sort( plys.begin(), plys.end(), [this]( const Ply &lhs, const Ply &rhs ) {
		if( current_player == white )
			return board.evaluate_ply( lhs, 0 ) > board.evaluate_ply( rhs, 0 );
		else
			return board.evaluate_ply( lhs, 0 ) < board.evaluate_ply( rhs, 0 );
	} );

	apply_move( plys[0] );

	return false;
}

void ChessGame::apply_move( Ply ply )
{
	disp.print_move( ply.print_LAN() );

	std::this_thread::sleep_for( std::chrono::seconds( 1 ) );

	game_moves.push_back( ply );

	board.update_board( ply );
}